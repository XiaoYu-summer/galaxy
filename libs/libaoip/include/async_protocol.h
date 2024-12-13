#pragma once
#include <chrono>
#include <future>
#include <map>
#include <mutex>

#include "protocol.h"
#include "udp_socket.h"

// 请求上下文
struct Request {
    Frame frame;
    std::chrono::steady_clock::time_point timestamp;
    uint32_t timeout_ms;
    std::promise<std::vector<uint32_t>> promise;

    Request(Frame f, uint32_t timeout)
        : frame(std::move(f)), timestamp(std::chrono::steady_clock::now()), timeout_ms(timeout) {}
};

// 请求管理器
class RequestManager {
   public:
    uint32_t addRequest(std::shared_ptr<Request> request);
    bool matchResponse(const Frame& response);
    void cleanTimeouts();

   private:
    std::mutex mutex_;
    std::map<uint32_t, std::shared_ptr<Request>> requests_;
    uint32_t next_request_id_{0};
};

// 异步协议类
class AsyncProtocol {
   public:
    explicit AsyncProtocol(const ProtocolConfig& config);
    ~AsyncProtocol();

    void start();
    void stop();

    std::future<std::vector<uint32_t>> sendRequest(uint16_t func_code, const std::vector<uint32_t>& data);

   private:
    void receiverLoop();
    void timeoutLoop();
    static UDPConfig makeUDPConfig(const ProtocolConfig& config);

    ProtocolConfig config_;
    std::unique_ptr<UDPSocket> socket_;
    std::atomic<bool> running_;
    std::thread receiver_thread_;
    std::thread timeout_thread_;
    RequestManager request_manager_;
};