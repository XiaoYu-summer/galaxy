#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <atomic>
#include <boost/chrono.hpp>
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
std::string TxtToString(const std::vector<std::pair<std::string, std::string>>& txt) {
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
std::vector<std::pair<std::string, std::string>> ParseTxt(const std::string& txt) {
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
int MdnsStringFromSockaddr(const struct sockaddr* addr, size_t addrlen, char* buffer, size_t capacity) {
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

void* ServiceThreadFunc(void* arg) {
    auto* data = static_cast<ThreadData*>(arg);
    while (*data->running) {
        mdns_socket_listen(data->socket, data->buffer, data->buffer_size, data->callback, data->user_data);
        boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
    delete data;
    return nullptr;
}

void* BrowseThreadFunc(void* arg) {
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
    MDNSServiceImpl() : running_(false), serviceSocket_(-1), serviceThread_(), browseThread_() {
        memset(buffer_, 0, sizeof(buffer_));
        memset(&service_, 0, sizeof(service_));
    }

    ~MDNSServiceImpl() {
        StopBrowse();
        StopPublish();
    }

    bool Publish(const std::string& name, const std::string& type, uint16_t port,
                 const std::vector<std::pair<std::string, std::string>>& txt, const std::string& domain) {
        if (running_) {
            StopPublish();
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

        serviceSocket_ = mdns_socket_open_ipv4(&addr);
        if (serviceSocket_ < 0) {
            return false;
        }

        // 设置服务记录
        mdns_record_t record_ptr = {0};
        mdns_record_t record_srv = {0};
        mdns_record_t record_txt = {0};

        std::string full_service_name = name + "." + service_type;
        std::string full_hostname = name + ".local";

        record_ptr.name = mdns_string_t{service_type.c_str(), service_type.length()};
        record_ptr.type = MDNS_RECORDTYPE_PTR;
        record_ptr.data.ptr.name = mdns_string_t{full_service_name.c_str(), full_service_name.length()};
        record_ptr.rclass = MDNS_CLASS_IN;
        record_ptr.ttl = 3600;

        record_srv.name = mdns_string_t{full_service_name.c_str(), full_service_name.length()};
        record_srv.type = MDNS_RECORDTYPE_SRV;
        record_srv.data.srv.name = mdns_string_t{full_hostname.c_str(), full_hostname.length()};
        record_srv.data.srv.port = port;
        record_srv.data.srv.priority = 0;
        record_srv.data.srv.weight = 0;
        record_srv.rclass = MDNS_CLASS_IN | MDNS_CACHE_FLUSH;
        record_srv.ttl = 120;

        // 设置 TXT 记录
        std::string txt_str = TxtToString(txt);
        record_txt.name = mdns_string_t{full_service_name.c_str(), full_service_name.length()};
        record_txt.type = MDNS_RECORDTYPE_TXT;
        record_txt.data.txt.key = mdns_string_t{txt_str.c_str(), txt_str.length()};
        record_txt.rclass = MDNS_CLASS_IN | MDNS_CACHE_FLUSH;
        record_txt.ttl = 4500;

        // 发布服务
        uint8_t buffer[1024];
        mdns_record_t additional[2] = {record_srv, record_txt};
        int result =
            mdns_announce_multicast(serviceSocket_, buffer, sizeof(buffer), record_ptr, nullptr, 0, additional, 2);
        if (result < 0) {
            mdns_socket_close(serviceSocket_);
            return false;
        }

        running_ = true;

        // 创建一个线程来处理mDNS服务
        try {
            serviceThread_ = boost::thread([this]() {
                uint8_t buffer[2048];
                while (running_) {
                    int nfds = serviceSocket_ + 1;
                    fd_set readfs;
                    FD_ZERO(&readfs);
                    FD_SET(serviceSocket_, &readfs);

                    struct timeval timeout;
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 100000;

                    if (select(nfds, &readfs, 0, 0, &timeout) >= 0) {
                        if (FD_ISSET(serviceSocket_, &readfs)) {
                            mdns_socket_listen(serviceSocket_, buffer, sizeof(buffer), ServiceCallback, &service_);
                        }
                    }
                }
            });
        } catch (const boost::thread_resource_error&) {
            running_ = false;
            mdns_socket_close(serviceSocket_);
            return false;
        }

        return true;
    }

    bool Browse(const std::string& type, std::function<void(const ServiceInfo&)> callback, const std::string& domain) {
        if (running_) {
            StopBrowse();
        }

        callback_ = std::move(callback);

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(MDNS_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;

        serviceSocket_ = mdns_socket_open_ipv4(&addr);
        if (serviceSocket_ < 0) {
            return false;
        }

        running_ = true;

        ThreadData* thread_data =
            new ThreadData{&running_, serviceSocket_, QueryCallback, this, buffer_, sizeof(buffer_)};

        try {
            browseThread_ = boost::thread(BrowseThreadFunc, thread_data);
        } catch (const boost::thread_resource_error&) {
            delete thread_data;
            running_ = false;
            return false;
        }

        return true;
    }

    void StopBrowse() {
        if (!running_) return;
        running_ = false;
        if (browseThread_.joinable()) {
            browseThread_.join();
        }
    }

    void StopPublish() {
        if (!running_) return;
        running_ = false;
        if (serviceThread_.joinable()) {
            serviceThread_.join();
        }
        if (serviceSocket_ >= 0) {
            mdns_socket_close(serviceSocket_);
            serviceSocket_ = -1;
        }
    }

    bool IsRunning() const { return running_; }

   private:
    static int QueryCallback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
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
            if (MdnsStringFromSockaddr(reinterpret_cast<const struct sockaddr*>(&addr), sizeof(struct sockaddr_in),
                                       addrbuffer, sizeof(addrbuffer)) > 0) {
                info.ip = std::string(addrbuffer);
            }
        } else if (rtype == MDNS_RECORDTYPE_AAAA) {
            struct sockaddr_in6 addr;
            mdns_record_parse_aaaa(data, size, record_offset, record_length, &addr);
            if (MdnsStringFromSockaddr(reinterpret_cast<const struct sockaddr*>(&addr), sizeof(struct sockaddr_in6),
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

    static int ServiceCallback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
                               uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data,
                               size_t size, size_t name_offset, size_t name_length, size_t record_offset,
                               size_t record_length, void* user_data) {
        // 处理传入的查询，如果需要，可以在这里回答查询
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
    int serviceSocket_;
    boost::thread serviceThread_;
    boost::thread browseThread_;
    mdns_service_t service_;
    uint8_t buffer_[2048];
    std::function<void(const ServiceInfo&)> callback_;
};

MDNSService::MDNSService() : impl_(new MDNSServiceImpl()) {}
MDNSService::~MDNSService() = default;

bool MDNSService::Publish(const std::string& name, const std::string& type, uint16_t port,
                          const std::vector<std::pair<std::string, std::string>>& txt, const std::string& domain) {
    return impl_->Publish(name, type, port, txt, domain);
}

bool MDNSService::Browse(const std::string& type, std::function<void(const ServiceInfo&)> callback,
                         const std::string& domain) {
    return impl_->Browse(type, std::move(callback), domain);
}

void MDNSService::StopBrowse() { impl_->StopBrowse(); }

void MDNSService::StopPublish() { impl_->StopPublish(); }

bool MDNSService::IsRunning() const { return impl_->IsRunning(); }

}  // namespace mdns
