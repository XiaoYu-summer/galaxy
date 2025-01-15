#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <atomic>
#include <boost/thread.hpp>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <sstream>

#include "mdns/MDNSService.h"
#include "mdns/mdns.h"

namespace mdns {

namespace {
struct mdns_service_t {
    mdns_string_t service;
    mdns_string_t hostname;
    mdns_string_t service_instance;
    mdns_string_t hostname_qualified;
    struct {
        mdns_string_t name;
        mdns_record_type_t type;
        union {
            mdns_string_t txt;
        } data;
    } txt_record[1];
    uint16_t port;
    uint32_t ttl;
};

// 辅助函数：将TXT记录转换为字符串
std::string txtToString(const std::vector<std::pair<std::string, std::string>>& txt) {
    std::stringstream ss;
    for (const auto& pair : txt) {
        if (!ss.str().empty()) {
            ss << ";";
        }
        ss << pair.first << "=" << pair.second;
    }
    return ss.str();
}

// 辅助函数：解析TXT记录字符串
std::vector<std::pair<std::string, std::string>> parseTxt(const std::string& txt) {
    std::vector<std::pair<std::string, std::string>> result;
    std::stringstream ss(txt);
    std::string item;

    while (std::getline(ss, item, ';')) {
        size_t pos = item.find('=');
        if (pos != std::string::npos) {
            result.emplace_back(item.substr(0, pos), item.substr(pos + 1));
        }
    }
    return result;
}

// 辅助函数：将 sockaddr 转换为字符串
int mdns_string_from_sockaddr(const struct sockaddr* addr, size_t addrlen, char* buffer, size_t capacity) {
    if (addr->sa_family == AF_INET) {
        struct sockaddr_in* saddr = (struct sockaddr_in*)addr;
        char host[NI_MAXHOST] = {0};
        if (getnameinfo(addr, addrlen, host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) == 0) {
            if (strlen(host) < capacity) {
                strcpy(buffer, host);
                return (int)strlen(buffer);
            }
        }
    } else if (addr->sa_family == AF_INET6) {
        struct sockaddr_in6* saddr = (struct sockaddr_in6*)addr;
        char host[NI_MAXHOST] = {0};
        if (getnameinfo(addr, addrlen, host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) == 0) {
            if (strlen(host) < capacity) {
                strcpy(buffer, host);
                return (int)strlen(buffer);
            }
        }
    }
    return 0;
}

struct ThreadData {
    std::atomic<bool>* running;
    int socket;
    mdns_record_callback_fn callback;
    void* user_data;
    uint8_t* buffer;
    size_t buffer_size;
};

void* service_thread_func(void* arg) {
    auto* data = static_cast<ThreadData*>(arg);
    while (*data->running) {
        mdns_socket_listen(data->socket, data->buffer, data->buffer_size, data->callback, data->user_data);
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
    delete data;
    return nullptr;
}

void* browse_thread_func(void* arg) {
    auto* data = static_cast<ThreadData*>(arg);
    while (*data->running) {
        mdns_socket_listen(data->socket, data->buffer, data->buffer_size, data->callback, data->user_data);
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
    delete data;
    return nullptr;
}
}  // namespace

class MDNSServiceImpl {
   public:
    MDNSServiceImpl() : running_(false), service_socket_(-1), service_thread_(), browse_thread_() {
        memset(buffer_, 0, sizeof(buffer_));
        memset(&service_, 0, sizeof(service_));
    }

    ~MDNSServiceImpl() {
        stopBrowse();
        stopPublish();
    }

    bool publish(const std::string& name, const std::string& type, uint16_t port,
                 const std::vector<std::pair<std::string, std::string>>& txt, const std::string& domain) {
        if (running_) {
            stopPublish();
        }

        // 确保服务类型格式正确
        std::string service_type = type;
        if (type.find("._") != 0) {
            service_type = "_" + type;
        }
        if (service_type.find("._tcp") == std::string::npos) {
            service_type += "._tcp";
        }
        if (service_type.find(".local.") == std::string::npos) {
            service_type += ".local.";
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(MDNS_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;

        service_socket_ = mdns_socket_open_ipv4(&addr);
        if (service_socket_ < 0) {
            return false;
        }

        mdns_string_t service_string = mdns_string_t{service_type.c_str(), service_type.length()};
        mdns_string_t hostname_string = mdns_string_t{name.c_str(), name.length()};

        // 构建服务实例字符串 "<hostname>.<_service-name>._tcp.local."
        std::string service_instance = name + "." + service_type;
        mdns_string_t service_instance_string = mdns_string_t{service_instance.c_str(), service_instance.length()};

        // 构建 "<hostname>.local." 字符串
        std::string hostname_qualified = name + ".local.";
        mdns_string_t hostname_qualified_string =
            mdns_string_t{hostname_qualified.c_str(), hostname_qualified.length()};

        memset(&service_, 0, sizeof(service_));
        service_.service = service_string;
        service_.hostname = hostname_string;
        service_.service_instance = service_instance_string;
        service_.hostname_qualified = hostname_qualified_string;
        service_.port = port;

        // 设置 PTR 记录
        mdns_record_t record_ptr = {.name = service_.service,
                                    .type = MDNS_RECORDTYPE_PTR,
                                    .data = {.ptr = {.name = service_.service_instance}},
                                    .rclass = MDNS_CLASS_IN,
                                    .ttl = 3600};

        // 设置 SRV 记录
        mdns_record_t record_srv = {
            .name = service_.service_instance,
            .type = MDNS_RECORDTYPE_SRV,
            .data = {.srv = {.name = service_.hostname_qualified, .port = service_.port, .priority = 0, .weight = 0}},
            .rclass = MDNS_CLASS_IN,
            .ttl = 3600};

        // 设置 TXT 记录
        mdns_record_t record_txt = {
            .name = service_.service_instance, .type = MDNS_RECORDTYPE_TXT, .rclass = MDNS_CLASS_IN, .ttl = 3600};

        std::string txt_str = txtToString(txt);
        if (!txt_str.empty()) {
            record_txt.data.txt.key = mdns_string_t{txt_str.c_str(), txt_str.length()};
        }

        // 发布服务
        uint8_t announce_buffer[1024];
        mdns_record_t additional[2] = {record_srv, record_txt};
        int res = mdns_announce_multicast(service_socket_, announce_buffer, sizeof(announce_buffer), record_ptr,
                                          nullptr, 0, additional, 2);
        if (res < 0) {
            mdns_socket_close(service_socket_);
            return false;
        }

        running_ = true;

        // 启动服务线程以处理传入的查询
        ThreadData* thread_data =
            new ThreadData{&running_, service_socket_, service_callback, &service_, buffer_, sizeof(buffer_)};

        try {
            service_thread_ = boost::thread(service_thread_func, thread_data);
        } catch (const boost::thread_resource_error&) {
            delete thread_data;
            running_ = false;
            mdns_socket_close(service_socket_);
            return false;
        }

        return true;
    }

    bool browse(const std::string& type, std::function<void(const ServiceInfo&)> callback, const std::string& domain) {
        if (running_) {
            stopBrowse();
        }

        callback_ = std::move(callback);

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(MDNS_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;

        service_socket_ = mdns_socket_open_ipv4(&addr);
        if (service_socket_ < 0) {
            return false;
        }

        running_ = true;

        ThreadData* thread_data =
            new ThreadData{&running_, service_socket_, query_callback, this, buffer_, sizeof(buffer_)};

        try {
            browse_thread_ = boost::thread(browse_thread_func, thread_data);
        } catch (const boost::thread_resource_error&) {
            delete thread_data;
            running_ = false;
            return false;
        }

        return true;
    }

    void stopBrowse() {
        if (!running_) return;
        running_ = false;
        if (browse_thread_.joinable()) {
            browse_thread_.join();
        }
    }

    void stopPublish() {
        if (!running_) return;
        running_ = false;
        if (service_thread_.joinable()) {
            service_thread_.join();
        }
        if (service_socket_ >= 0) {
            mdns_socket_close(service_socket_);
            service_socket_ = -1;
        }
    }

    bool isRunning() const { return running_; }

   private:
    static int query_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
                              uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data,
                              size_t size, size_t name_offset, size_t name_length, size_t record_offset,
                              size_t record_length, void* user_data) {
        auto* self = static_cast<MDNSServiceImpl*>(user_data);
        if (!self || !self->callback_) return 0;

        ServiceInfo info;
        char namebuffer[256];
        char addrbuffer[128];

        if (rtype == MDNS_RECORDTYPE_PTR) {
            mdns_string_t service_instance =
                mdns_record_parse_ptr(data, size, record_offset, record_length, namebuffer, sizeof(namebuffer));
            info.type = std::string(service_instance.str, service_instance.length);
            info.name = std::string(service_instance.str, service_instance.length);
        } else if (rtype == MDNS_RECORDTYPE_SRV) {
            mdns_record_srv_t srv =
                mdns_record_parse_srv(data, size, record_offset, record_length, namebuffer, sizeof(namebuffer));
            info.host = std::string(srv.name.str, srv.name.length);
            info.port = srv.port;
        } else if (rtype == MDNS_RECORDTYPE_A) {
            struct sockaddr_in addr;
            mdns_record_parse_a(data, size, record_offset, record_length, &addr);
            if (mdns_string_from_sockaddr(reinterpret_cast<const struct sockaddr*>(&addr), sizeof(struct sockaddr_in),
                                          addrbuffer, sizeof(addrbuffer)) > 0) {
                info.ip = std::string(addrbuffer);
            }
        } else if (rtype == MDNS_RECORDTYPE_AAAA) {
            struct sockaddr_in6 addr;
            mdns_record_parse_aaaa(data, size, record_offset, record_length, &addr);
            if (mdns_string_from_sockaddr(reinterpret_cast<const struct sockaddr*>(&addr), sizeof(struct sockaddr_in6),
                                          addrbuffer, sizeof(addrbuffer)) > 0) {
                info.ip = std::string(addrbuffer);
            }
        } else if (rtype == MDNS_RECORDTYPE_TXT) {
            mdns_record_txt_t txt_records[128];
            size_t txt_count = mdns_record_parse_txt(data, size, record_offset, record_length, txt_records,
                                                     sizeof(txt_records) / sizeof(txt_records[0]));

            for (size_t i = 0; i < txt_count; ++i) {
                std::string key(txt_records[i].key.str, txt_records[i].key.length);
                std::string value(txt_records[i].value.str, txt_records[i].value.length);
                info.txt.emplace_back(key, value);
            }
        }

        self->callback_(info);
        return 0;
    }

    static int service_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
                                uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data,
                                size_t size, size_t name_offset, size_t name_length, size_t record_offset,
                                size_t record_length, void* user_data) {
        // 处理传入的查询，如果需要，可以在这里回答查询
        return 0;
    }

    std::atomic<bool> running_;
    int service_socket_;
    boost::thread service_thread_;
    boost::thread browse_thread_;
    mdns_service_t service_;
    uint8_t buffer_[2048];
    std::function<void(const ServiceInfo&)> callback_;
};

MDNSService::MDNSService() : impl_(new MDNSServiceImpl()) {}
MDNSService::~MDNSService() = default;

bool MDNSService::publish(const std::string& name, const std::string& type, uint16_t port,
                          const std::vector<std::pair<std::string, std::string>>& txt, const std::string& domain) {
    return impl_->publish(name, type, port, txt, domain);
}

bool MDNSService::browse(const std::string& type, std::function<void(const ServiceInfo&)> callback,
                         const std::string& domain) {
    return impl_->browse(type, std::move(callback), domain);
}

void MDNSService::stopBrowse() { impl_->stopBrowse(); }

void MDNSService::stopPublish() { impl_->stopPublish(); }

bool MDNSService::isRunning() const { return impl_->isRunning(); }

}  // namespace mdns
