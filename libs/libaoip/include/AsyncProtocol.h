#pragma once

#include <atomic>
#include <chrono>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "UdpSocket.h"

namespace aoip
{
struct ProtocolConfig
{
    std::string masterIp_{"0.0.0.0"};
    uint16_t slavePort_{50000};
    uint16_t masterPort_{60000};
    uint32_t timeoutMs_{1000};
    uint32_t maxRetries_{3};
    uint32_t productId_{0x02020483};
    uint16_t deviceId_{0xFFFF};
    bool broadcast_{true};
    size_t recvBufferSize_{4096};
};

struct Request
{
    std::string functionCode_;
    std::promise<std::vector<uint8_t>> promise_;
    std::chrono::steady_clock::time_point timestamp_;
    uint32_t timeoutMs_;

    Request(const std::string& functionCode, uint32_t timeoutMs)
        : functionCode_(functionCode), timestamp_(std::chrono::steady_clock::now()), timeoutMs_(timeoutMs) {}
};

class UdpCallback
{
public:
    virtual std::string GetFunctionCode(const std::vector<uint8_t>& response) const = 0;
};

class RequestManager
{
   public:
    uint32_t AddRequest(std::shared_ptr<Request> request);
    bool MatchResponse(const std::vector<uint8_t>& response);
    void CleanTimeouts();
    void SetUdpCallback(std::shared_ptr<UdpCallback> cb);
   private:
    std::mutex mutex_;
    uint32_t nextRequestId_{0};
    std::map<uint32_t, std::shared_ptr<Request>> requests_;
    std::weak_ptr<UdpCallback> udpCallback_;
};

class AsyncProtocol
{
   public:
    explicit AsyncProtocol(const ProtocolConfig& config);
    ~AsyncProtocol();

    void Start();
    void Stop();
    std::future<std::vector<uint8_t>> SendRequest(const std::string& funcCode, const void* data, size_t len);
    void SetUdpCallback(std::shared_ptr<UdpCallback> cb);

   private:
    static UdpConfig MakeUDPConfig(const ProtocolConfig& config);
    void ReceiverLoop();
    void TimeoutLoop();

    ProtocolConfig config_;
    std::unique_ptr<UdpSocket> socket_;
    std::unique_ptr<RequestManager> requestManager_;
    std::atomic<bool> running_{false};
    std::thread receiverThread_;
    std::thread timeoutThread_;
};

}  // namespace aoip