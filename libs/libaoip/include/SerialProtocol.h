#pragma once
#include <boost/asio.hpp>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/Logger.h>
#include <vector>
#include <memory>

namespace aoip
{

class ResponseCallback
{
public:
    virtual ~ResponseCallback() = default;
    virtual void OnRecvResponse(const std::vector<uint8_t>& data) = 0;
};

class SerialTask : public Poco::Runnable
{
public:
    SerialTask(std::shared_ptr<ResponseCallback> cb, const std::string& port, uint32_t baudRate, uint32_t readTimeoutMs);
    ~SerialTask();
    virtual void run() override;

    void Start();
    void Stop();

    void Write(const void* data, uint32_t len);

private:
    void handleRead(const boost::system::error_code& ec, std::size_t bytesRead, std::vector<uint8_t>& buffer);
    void handleTimeout(const boost::system::error_code& ec);

    std::weak_ptr<ResponseCallback> cb_;
    std::atomic<bool> running_;
    std::atomic<bool> readingInProgress_;  // 是否存在读取任务
    std::atomic<bool> writingInProgress_;  // 是否存在写任务
    boost::asio::io_service io_;
    boost::asio::serial_port serial_;
    boost::asio::steady_timer timeoutTimer_;
    uint32_t readTimeoutSecond_; //读取超时时间 单位s
    Poco::Thread thread_;
    Poco::Logger& logger_;
};

}