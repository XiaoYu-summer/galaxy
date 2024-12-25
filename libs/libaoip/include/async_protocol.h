#pragma once

#include <atomic>
#include <chrono>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include "protocol.h"
#include "udp_socket.h"

namespace aoip {

struct Request {
    Frame frame_;
    std::promise<std::vector<uint32_t>> promise_;
    std::chrono::steady_clock::time_point timestamp_;
    uint32_t timeoutMs_;

    Request(const Frame& frame, uint32_t timeoutMs)
        : frame_(frame), timestamp_(std::chrono::steady_clock::now()), timeoutMs_(timeoutMs) {}
};

class RequestManager {
   public:
    uint32_t AddRequest(std::shared_ptr<Request> request);
    bool MatchResponse(const Frame& response);
    void CleanTimeouts();

   private:
    std::mutex mutex_;
    uint32_t nextRequestId_{0};
    std::map<uint32_t, std::shared_ptr<Request>> requests_;
};

struct ProtocolConfig {
    uint32_t productId_{0};
    uint32_t deviceId_{0};
    uint16_t masterPort_{0};
    uint16_t slavePort_{0};
    bool broadcast_{false};
    uint32_t timeoutMs_{1000};
    uint32_t recvBufferSize_{4096};
    bool enableLogging_{false};
    std::string logFile_;
    LogLevel logLevel_{LogLevel::INFO};
};

class AsyncProtocol {
   public:
    explicit AsyncProtocol(const ProtocolConfig& config);
    ~AsyncProtocol();

    void Start();
    void Stop();
    std::future<std::vector<uint32_t>> SendRequest(uint16_t funcCode, const std::vector<uint32_t>& data);

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