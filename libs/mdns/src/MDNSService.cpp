#include "mdns/MDNSService.h"
#include "mdns/mdns.h"
#include <boost/thread.hpp>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>

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
                result.emplace_back(
                    item.substr(0, pos),
                    item.substr(pos + 1)
                );
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
        }
        else if (addr->sa_family == AF_INET6) {
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
            mdns_socket_listen(data->socket, data->buffer, data->buffer_size,
                             data->callback, data->user_data);
            boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
        }
        delete data;
        return nullptr;
    }

    void* browse_thread_func(void* arg) {
        auto* data = static_cast<ThreadData*>(arg);
        while (*data->running) {
            mdns_socket_listen(data->socket, data->buffer, data->buffer_size,
                             data->callback, data->user_data);
            boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
        }
        delete data;
        return nullptr;
    }
}

class MDNSServiceImpl {
public:
    MDNSServiceImpl() 
        : running_(false)
        , service_socket_(-1)
        , service_thread_()
        , browse_thread_() {
        memset(buffer_, 0, sizeof(buffer_));
        memset(&service_, 0, sizeof(service_));
    }

    ~MDNSServiceImpl() {
        stopBrowse();
        stopPublish();
    }

    bool publish(const std::string& name,
                const std::string& type,
                uint16_t port,
                const std::vector<std::pair<std::string, std::string>>& txt,
                const std::string& domain) {
        if (running_) {
            stopPublish();
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

        memset(&service_, 0, sizeof(service_));
        service_.service = mdns_string_t{name.c_str(), name.length()};
        service_.hostname = mdns_string_t{name.c_str(), name.length()};
        service_.service_instance = mdns_string_t{(name + "." + type).c_str(), name.length() + type.length() + 1};
        service_.hostname_qualified = mdns_string_t{(name + "." + domain).c_str(), name.length() + domain.length() + 1};
        service_.port = port;
        
        std::string txt_str = txtToString(txt);
        if (!txt_str.empty()) {
            service_.txt_record[0].name = service_.service_instance;
            service_.txt_record[0].type = MDNS_RECORDTYPE_TXT;
            service_.txt_record[0].data.txt = mdns_string_t{txt_str.c_str(), txt_str.length()};
        }

        running_ = true;
        
        ThreadData* thread_data = new ThreadData{
            &running_,
            service_socket_,
            query_callback,
            &service_,
            buffer_,
            sizeof(buffer_)
        };

        try {
            service_thread_ = boost::thread(service_thread_func, thread_data);
        } catch (const boost::thread_resource_error&) {
            delete thread_data;
            running_ = false;
            return false;
        }

        return true;
    }

    bool browse(const std::string& type,
               std::function<void(const ServiceInfo&)> callback,
               const std::string& domain) {
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

        ThreadData* thread_data = new ThreadData{
            &running_,
            service_socket_,
            query_callback,
            this,
            buffer_,
            sizeof(buffer_)
        };

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

    bool isRunning() const {
        return running_;
    }

private:
    static int query_callback(int sock, const struct sockaddr* from, size_t addrlen,
                            mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype,
                            uint16_t rclass, uint32_t ttl, const void* data, size_t size,
                            size_t name_offset, size_t name_length, size_t record_offset,
                            size_t record_length, void* user_data) {
        auto* self = static_cast<MDNSServiceImpl*>(user_data);
        if (!self || !self->callback_) return 0;

        ServiceInfo info;
        char namebuffer[256];
        char addrbuffer[128];

        if (rtype == MDNS_RECORDTYPE_PTR) {
            mdns_string_t service_instance = mdns_record_parse_ptr(data, size, record_offset, 
                                                                 record_length, namebuffer, sizeof(namebuffer));
            info.type = std::string(service_instance.str, service_instance.length);
            info.name = std::string(service_instance.str, service_instance.length);
        }
        else if (rtype == MDNS_RECORDTYPE_SRV) {
            mdns_record_srv_t srv = mdns_record_parse_srv(data, size, record_offset, record_length,
                                                        namebuffer, sizeof(namebuffer));
            info.host = std::string(srv.name.str, srv.name.length);
            info.port = srv.port;
        }
        else if (rtype == MDNS_RECORDTYPE_A) {
            struct sockaddr_in addr;
            mdns_record_parse_a(data, size, record_offset, record_length, &addr);
            if (mdns_string_from_sockaddr(reinterpret_cast<const struct sockaddr*>(&addr), 
                                        sizeof(struct sockaddr_in), addrbuffer, sizeof(addrbuffer)) > 0) {
                info.ip = std::string(addrbuffer);
            }
        }
        else if (rtype == MDNS_RECORDTYPE_AAAA) {
            struct sockaddr_in6 addr;
            mdns_record_parse_aaaa(data, size, record_offset, record_length, &addr);
            if (mdns_string_from_sockaddr(reinterpret_cast<const struct sockaddr*>(&addr), 
                                        sizeof(struct sockaddr_in6), addrbuffer, sizeof(addrbuffer)) > 0) {
                info.ip = std::string(addrbuffer);
            }
        }
        else if (rtype == MDNS_RECORDTYPE_TXT) {
            mdns_record_txt_t txt_records[128];
            size_t txt_count = mdns_record_parse_txt(data, size, record_offset, record_length,
                                                   txt_records, sizeof(txt_records) / sizeof(txt_records[0]));
            
            for (size_t i = 0; i < txt_count; ++i) {
                std::string key(txt_records[i].key.str, txt_records[i].key.length);
                std::string value(txt_records[i].value.str, txt_records[i].value.length);
                info.txt.emplace_back(key, value);
            }
        }

        self->callback_(info);
        return 0;
    }

    static int service_callback(int sock, const struct sockaddr* from, size_t addrlen,
                              mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype,
                              uint16_t rclass, uint32_t ttl, const void* data, size_t size,
                              size_t name_offset, size_t name_length, size_t record_offset,
                              size_t record_length, void* user_data) {
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

bool MDNSService::publish(const std::string& name,
                         const std::string& type,
                         uint16_t port,
                         const std::vector<std::pair<std::string, std::string>>& txt,
                         const std::string& domain) {
    return impl_->publish(name, type, port, txt, domain);
}

bool MDNSService::browse(const std::string& type,
                        std::function<void(const ServiceInfo&)> callback,
                        const std::string& domain) {
    return impl_->browse(type, std::move(callback), domain);
}

void MDNSService::stopBrowse() {
    impl_->stopBrowse();
}

void MDNSService::stopPublish() {
    impl_->stopPublish();
}

bool MDNSService::isRunning() const {
    return impl_->isRunning();
}

} // namespace mdns
