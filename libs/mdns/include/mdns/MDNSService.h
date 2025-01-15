#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mdns
{

struct ServiceInfo
{
    std::string name;
    std::string type;
    std::string host;
    std::string ip;
    uint16_t port;
    std::vector<std::pair<std::string, std::string>> txt;
};

class MDNSServiceImpl;

class MDNSService
{
   public:
    MDNSService();
    ~MDNSService();

    bool Publish(const std::string& name, const std::string& type, uint16_t port,
                 const std::vector<std::pair<std::string, std::string>>& txt, const std::string& domain = "local");

    bool Browse(const std::string& type, std::function<void(const ServiceInfo&)> callback,
                const std::string& domain = "local");

    void StopBrowse();
    void StopPublish();
    bool IsRunning() const;

   private:
    std::unique_ptr<MDNSServiceImpl> impl_;
};

}  // namespace mdns