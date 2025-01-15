#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <atomic>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <sstream>

#include "MDNSService.h"
#include "Poco/Logger.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include "Poco/Timestamp.h"
#include "mdns.h"

namespace mdns
{

namespace
{

// 辅助函数：将sockaddr转换为字符串
int MdnsStringFromSockaddr(const struct sockaddr* addr, size_t addrlen, char* buffer, size_t capacity)
{
    if (addr->sa_family == AF_INET)
    {
        char host[NI_MAXHOST] = {0};
        if (getnameinfo(addr, addrlen, host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) == 0)
        {
            if (strlen(host) < capacity)
            {
                strcpy(buffer, host);
                return (int)strlen(buffer);
            }
        }
    }
    else if (addr->sa_family == AF_INET6)
    {
        char host[NI_MAXHOST] = {0};
        if (getnameinfo(addr, addrlen, host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) == 0)
        {
            if (strlen(host) < capacity)
            {
                strcpy(buffer, host);
                return (int)strlen(buffer);
            }
        }
    }
    return 0;
}

// 辅助函数：设置服务记录 只处理PTR和SRV记录
void SetBaseRecords(mdns_record_t& record_ptr, mdns_record_t& record_srv, const std::string& service_type,
                    const std::string& full_service_name, const std::string& full_hostname, uint16_t port)
{
    // 设置PTR记录
    record_ptr.name = mdns_string_t{service_type.c_str(), service_type.length()};
    record_ptr.type = MDNS_RECORDTYPE_PTR;
    record_ptr.data.ptr.name = mdns_string_t{full_service_name.c_str(), full_service_name.length()};
    record_ptr.rclass = MDNS_CLASS_IN;
    record_ptr.ttl = 10;

    // 设置SRV记录
    record_srv.name = mdns_string_t{full_service_name.c_str(), full_service_name.length()};
    record_srv.type = MDNS_RECORDTYPE_SRV;
    record_srv.data.srv.name = mdns_string_t{full_hostname.c_str(), full_hostname.length()};
    record_srv.data.srv.port = port;
    record_srv.data.srv.priority = 0;
    record_srv.data.srv.weight = 0;
    record_srv.rclass = MDNS_CLASS_IN | MDNS_CACHE_FLUSH;
    record_srv.ttl = 10;
}

// 创建单个TXT记录的辅助函数
mdns_record_t CreateTxtRecord(const std::string& full_service_name, const std::string& key, const std::string& value)
{
    mdns_record_t record_txt = {0};
    record_txt.name = mdns_string_t{full_service_name.c_str(), full_service_name.length()};
    record_txt.type = MDNS_RECORDTYPE_TXT;
    record_txt.rclass = MDNS_CLASS_IN | MDNS_CACHE_FLUSH;
    record_txt.ttl = 10;
    record_txt.data.txt.key = mdns_string_t{key.c_str(), key.length()};
    record_txt.data.txt.value = mdns_string_t{value.c_str(), value.length()};
    return record_txt;
}

class ServiceThread : public Poco::Runnable
{
   public:
    struct ServiceData
    {
        std::string name;
        std::string type;
        std::string full_service_name;
        std::string full_hostname;
        uint16_t port;
        std::vector<std::pair<std::string, std::string>> txt;
    };

    ServiceThread(std::atomic<bool>& running, int socket, const ServiceData& serviceData,
                  mdns_record_callback_fn callback, void* user_data)
        : running_(running),
          socket_(socket),
          serviceData_(serviceData),
          callback_(callback),
          user_data_(user_data),
          logger_(Poco::Logger::get("MDNSService.ServiceThread"))
    {
    }

    void run() override
    {
        uint8_t buffer[2048];
        Poco::Timestamp lastAnnounceTime;

        while (running_)
        {
            // 监听查询请求
            mdns_socket_listen(socket_, buffer, sizeof(buffer), callback_, user_data_);
            Poco::Thread::sleep(100);

            // 定时广播服务
            if (lastAnnounceTime.isElapsed(5000000))
            {  // 5秒 = 5000000微秒
                BroadcastService(buffer, sizeof(buffer));
                lastAnnounceTime.update();
            }
        }
    }

   private:
    void BroadcastService(uint8_t* buffer, size_t buffer_size)
    {
        try
        {
            mdns_record_t record_ptr = {0};
            mdns_record_t record_srv = {0};

            // 设置基础记录
            SetBaseRecords(record_ptr, record_srv, serviceData_.type, serviceData_.full_service_name,
                           serviceData_.full_hostname, serviceData_.port);

            // 创建additional records数组
            std::vector<mdns_record_t> additional;
            additional.push_back(record_srv);  // 添加SRV记录

            // 添加所有TXT记录
            for (const auto& txt_pair : serviceData_.txt)
            {
                additional.push_back(CreateTxtRecord(serviceData_.full_service_name, txt_pair.first, txt_pair.second));
            }

            // 发送广播
            int result = mdns_announce_multicast(socket_, buffer, buffer_size, record_ptr, nullptr, 0,
                                                 additional.data(), additional.size());

            if (result < 0)
            {
                logger_.error("Failed to broadcast service: %d", result);
            }
        }
        catch (const std::exception& e)
        {
            logger_.error("Error in BroadcastService: %s", e.what());
        }
    }

    std::atomic<bool>& running_;
    int socket_;
    ServiceData serviceData_;
    mdns_record_callback_fn callback_;
    void* user_data_;
    Poco::Logger& logger_;
};

}  // namespace

class MDNSServiceImpl
{
   public:
    MDNSServiceImpl() : running_(false), serviceSocket_(-1), logger_(Poco::Logger::get("MDNSService")) {}

    ~MDNSServiceImpl()
    {
        StopBrowse();
        StopPublish();
    }

    bool Publish(const std::string& name, const std::string& type, uint16_t port,
                 const std::vector<std::pair<std::string, std::string>>& txt, const std::string& domain)
    {
        if (running_)
        {
            StopPublish();
        }

        try
        {
            // 保存服务信息
            ServiceThread::ServiceData serviceData;
            serviceData.name = name;
            serviceData.type = FormatServiceType(type);
            serviceData.port = port;
            serviceData.txt = txt;
            serviceData.full_service_name = name + "." + serviceData.type;
            serviceData.full_hostname = name + ".local";

            // 创建socket
            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(MDNS_PORT);
            addr.sin_addr.s_addr = INADDR_ANY;

            serviceSocket_ = mdns_socket_open_ipv4(&addr);
            if (serviceSocket_ < 0)
            {
                logger_.error("Failed to open socket");
                return false;
            }

            // 初始广播
            uint8_t buffer[1024];
            mdns_record_t record_ptr = {0};
            mdns_record_t record_srv = {0};

            // 设置基础记录
            SetBaseRecords(record_ptr, record_srv, serviceData.type, serviceData.full_service_name,
                           serviceData.full_hostname, serviceData.port);

            // 创建additional records数组
            std::vector<mdns_record_t> additional;
            additional.push_back(record_srv);  // 添加SRV记录

            // 添加所有TXT记录
            for (const auto& txt_pair : serviceData.txt)
            {
                additional.push_back(CreateTxtRecord(serviceData.full_service_name, txt_pair.first, txt_pair.second));
            }

            // 发送初始广播
            int result = mdns_announce_multicast(serviceSocket_, buffer, sizeof(buffer), record_ptr, nullptr, 0,
                                                 additional.data(), additional.size());

            if (result < 0)
            {
                logger_.error("Failed to announce service: %d", result);
                mdns_socket_close(serviceSocket_);
                return false;
            }

            running_ = true;

            // 启动服务线程
            serviceRunnable_ =
                std::make_shared<ServiceThread>(running_, serviceSocket_, serviceData, ServiceCallback, this);
            serviceThread_ = std::make_unique<Poco::Thread>();
            serviceThread_->start(*serviceRunnable_);

            return true;
        }
        catch (const std::exception& e)
        {
            logger_.error("Error in Publish: %s", e.what());
            StopPublish();
            return false;
        }
    }

    bool Browse(const std::string& type, std::function<void(const ServiceInfo&)> callback, const std::string& domain)
    {
        if (running_)
        {
            StopBrowse();
        }

        callback_ = std::move(callback);

        try
        {
            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(MDNS_PORT);
            addr.sin_addr.s_addr = INADDR_ANY;

            serviceSocket_ = mdns_socket_open_ipv4(&addr);
            if (serviceSocket_ < 0)
            {
                logger_.error("Failed to open socket for browsing");
                return false;
            }

            running_ = true;

            // 启动浏览线程
            ServiceThread::ServiceData serviceData;
            serviceData.type = FormatServiceType(type);

            browseRunnable_ =
                std::make_shared<ServiceThread>(running_, serviceSocket_, serviceData, QueryCallback, this);
            browseThread_ = std::make_unique<Poco::Thread>();
            browseThread_->start(*browseRunnable_);

            return true;
        }
        catch (const std::exception& e)
        {
            logger_.error("Error in Browse: %s", e.what());
            return false;
        }
    }

    void StopBrowse()
    {
        if (!running_) return;
        running_ = false;
        if (browseThread_)
        {
            browseThread_->join();
            browseThread_.reset();
        }
        if (serviceSocket_ >= 0)
        {
            mdns_socket_close(serviceSocket_);
            serviceSocket_ = -1;
        }
    }

    void StopPublish()
    {
        if (!running_) return;
        running_ = false;
        if (serviceThread_)
        {
            serviceThread_->join();
            serviceThread_.reset();
        }
        if (serviceSocket_ >= 0)
        {
            mdns_socket_close(serviceSocket_);
            serviceSocket_ = -1;
        }
    }

    bool IsRunning() const { return running_; }

   private:
    static std::string FormatServiceType(const std::string& type)
    {
        std::string service_type = type;
        if (type.find("._") != 0)
        {
            service_type = "_" + type;
        }
        if (service_type.find("._tcp") == std::string::npos)
        {
            service_type += "._tcp";
        }
        if (service_type.find(".local.") == std::string::npos)
        {
            service_type += ".local.";
        }
        return service_type;
    }

    struct ServiceInfoCache
    {
        ServiceInfo info;
        bool hasPTR = false;
        bool hasSRV = false;
        bool hasTXT = false;
        bool hasIP = false;  // 添加IP标志
        Poco::Timestamp lastUpdate;
    };

    static int QueryCallback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
                             uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data,
                             size_t size, size_t name_offset, size_t name_length, size_t record_offset,
                             size_t record_length, void* user_data)
    {
        auto* self = static_cast<MDNSServiceImpl*>(user_data);
        if (!self || !self->callback_) return 0;

        char namebuffer[256];
        char addrbuffer[128];
        std::string service_instance;

        mdns_string_t name = mdns_string_extract(data, size, &name_offset, namebuffer, sizeof(namebuffer));
        std::string record_name(name.str, name.length);

        std::lock_guard<std::mutex> lock(self->cacheMutex_);

        if (rtype == MDNS_RECORDTYPE_PTR)
        {
            // PTR记录处理保持不变
            mdns_string_t service_instance_str =
                mdns_record_parse_ptr(data, size, record_offset, record_length, namebuffer, sizeof(namebuffer));
            service_instance = std::string(service_instance_str.str, service_instance_str.length);

            auto& cache = self->serviceCache_[service_instance];
            cache.info.type = record_name;
            cache.info.name = service_instance;
            cache.hasPTR = true;
            cache.lastUpdate.update();
        }
        else if (rtype == MDNS_RECORDTYPE_SRV)
        {
            service_instance = record_name;

            mdns_record_srv_t srv =
                mdns_record_parse_srv(data, size, record_offset, record_length, namebuffer, sizeof(namebuffer));

            auto& cache = self->serviceCache_[service_instance];
            cache.info.host = std::string(srv.name.str, srv.name.length);
            cache.info.port = srv.port;
            cache.hasSRV = true;
            cache.lastUpdate.update();

            // 生成随机query_id
            uint16_t query_id = static_cast<uint16_t>(rand() & 0xFFFF);

            // 主动查询A记录
            uint8_t buffer[2048];
            mdns_query_send(sock, MDNS_RECORDTYPE_A, cache.info.host.c_str(), strlen(cache.info.host.c_str()), buffer,
                            sizeof(buffer), query_id);
        }
        else if (rtype == MDNS_RECORDTYPE_TXT)
        {
            // TXT记录处理保持不变
            service_instance = record_name;

            auto& cache = self->serviceCache_[service_instance];
            mdns_record_txt_t txt_records[128];
            size_t txt_count = mdns_record_parse_txt(data, size, record_offset, record_length, txt_records,
                                                     sizeof(txt_records) / sizeof(txt_records[0]));

            for (size_t i = 0; i < txt_count; ++i)
            {
                std::string key(txt_records[i].key.str, txt_records[i].key.length);
                std::string value(txt_records[i].value.str, txt_records[i].value.length);
                cache.info.txt.emplace_back(key, value);
            }
            cache.hasTXT = true;
            cache.lastUpdate.update();
        }
        else if (rtype == MDNS_RECORDTYPE_A || rtype == MDNS_RECORDTYPE_AAAA)
        {
            // 遍历缓存查找匹配的hostname
            for (auto& [instance_name, cache] : self->serviceCache_)
            {
                if (cache.info.host == record_name)
                {
                    if (rtype == MDNS_RECORDTYPE_A)
                    {
                        struct sockaddr_in addr;
                        mdns_record_parse_a(data, size, record_offset, record_length, &addr);
                        if (MdnsStringFromSockaddr((struct sockaddr*)&addr, sizeof(addr), addrbuffer,
                                                   sizeof(addrbuffer)) > 0)
                        {
                            cache.info.ip = addrbuffer;
                            cache.hasIP = true;  // 标记已获取IP
                        }
                    }
                    else
                    {  // AAAA
                        struct sockaddr_in6 addr;
                        mdns_record_parse_aaaa(data, size, record_offset, record_length, &addr);
                        if (MdnsStringFromSockaddr((struct sockaddr*)&addr, sizeof(addr), addrbuffer,
                                                   sizeof(addrbuffer)) > 0)
                        {
                            cache.info.ip = addrbuffer;
                            cache.hasIP = true;  // 标记已获取IP
                        }
                    }
                    cache.lastUpdate.update();
                }
            }
        }

        // 修改完整性检查条件
        auto it = self->serviceCache_.find(service_instance);
        if (it != self->serviceCache_.end())
        {
            auto& cache = it->second;
            // 等待所有必要信息都收集完毕
            if (cache.hasPTR && cache.hasSRV && cache.hasTXT && cache.hasIP)
            {
                self->callback_(cache.info);
                self->serviceCache_.erase(it);
            }
            // 如果超过一定时间还没有收到IP，也触发回调
            else if (cache.hasPTR && cache.hasSRV && cache.hasTXT && cache.lastUpdate.isElapsed(1000000))
            {  // 1秒后
                self->callback_(cache.info);
                self->serviceCache_.erase(it);
            }
        }

        // 清理过期缓存
        auto now = Poco::Timestamp();
        for (auto it = self->serviceCache_.begin(); it != self->serviceCache_.end();)
        {
            if (now.isElapsed(it->second.lastUpdate.epochMicroseconds() + 5000000))
            {
                it = self->serviceCache_.erase(it);
            }
            else
            {
                ++it;
            }
        }

        return 0;
    }

    static int ServiceCallback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
                               uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data,
                               size_t size, size_t name_offset, size_t name_length, size_t record_offset,
                               size_t record_length, void* user_data)
    {
        return 0;  // 简单返回，因为广播由ServiceThread处理
    }

    std::atomic<bool> running_;
    int serviceSocket_;
    std::shared_ptr<ServiceThread> serviceRunnable_;
    std::shared_ptr<ServiceThread> browseRunnable_;
    std::unique_ptr<Poco::Thread> serviceThread_;
    std::unique_ptr<Poco::Thread> browseThread_;
    std::function<void(const ServiceInfo&)> callback_;
    Poco::Logger& logger_;
    std::map<std::string, ServiceInfoCache> serviceCache_;  // 服务信息缓存
    std::mutex cacheMutex_;                                 // 保护缓存的互斥锁
};

MDNSService::MDNSService() : impl_(new MDNSServiceImpl()) {}
MDNSService::~MDNSService() = default;

bool MDNSService::Publish(const std::string& name, const std::string& type, uint16_t port,
                          const std::vector<std::pair<std::string, std::string>>& txt, const std::string& domain)
{
    return impl_->Publish(name, type, port, txt, domain);
}

bool MDNSService::Browse(const std::string& type, std::function<void(const ServiceInfo&)> callback,
                         const std::string& domain)
{
    return impl_->Browse(type, std::move(callback), domain);
}

void MDNSService::StopBrowse() { impl_->StopBrowse(); }

void MDNSService::StopPublish() { impl_->StopPublish(); }

bool MDNSService::IsRunning() const { return impl_->IsRunning(); }

}  // namespace mdns