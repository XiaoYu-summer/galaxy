#pragma once
#include <memory>
#include <string>
#include <vector>

namespace aoip
{

struct UdpConfig
{
    std::string bindIp_{"0.0.0.0"};
    uint16_t bindPort_{0};
    bool broadcast_{false};
    size_t sendBufferSize_{65535};
    size_t recvBufferSize_{65535};
    int timeoutMs_{1000};
};

class UdpSocket
{
   public:
    explicit UdpSocket(const UdpConfig& config);
    ~UdpSocket();

    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;

    bool SendTo(const void* data, size_t len, const std::string& ip, uint16_t port);

    bool SendTo(const std::vector<uint8_t>& data, const std::string& ip, uint16_t port)
    {
        return SendTo(data.data(), data.size(), ip, port);
    }

    bool Broadcast(const void* data, size_t len, uint16_t port);

    bool Broadcast(const std::vector<uint8_t>& data, uint16_t port)
    {
        return Broadcast(data.data(), data.size(), port);
    }

    bool RecvFrom(void* buffer, size_t& len, std::string& fromIp, uint16_t& fromPort, int timeoutMs = -1);

    bool RecvFrom(std::vector<uint8_t>& data, std::string& fromIp, uint16_t& fromPort, int timeoutMs = -1);

    std::string GetLastError() const { return lastError_; }
    bool GetLocalAddress(std::string& ip, uint16_t& port) const;

   private:
    bool Init();
    bool SetSocketOptions();
    void SetError(const char* msg);

    int socket_{-1};
    UdpConfig config_;
    std::string lastError_;
};

}  // namespace aoip