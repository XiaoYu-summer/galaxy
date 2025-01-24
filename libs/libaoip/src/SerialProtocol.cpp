#include <functional>
#include "SerialProtocol.h"
#include "common/LoggerWrapper.h"

namespace aoip
{
DEFINE_FILE_NAME("SerialTask.cpp")

SerialTask::SerialTask(std::shared_ptr<ResponseCallback> cb, const std::string& port, uint32_t baudRate, uint32_t readTimeoutSecond)
    : cb_(cb)
    , running_(false)
    , readingInProgress_(false)
    , writingInProgress_(false)
    , io_()
    , serial_(io_, port)
    , timeoutTimer_(io_)
    , readTimeoutSecond_(readTimeoutSecond)
    , logger_(Poco::Logger::get("SerialTask"))
{
    serial_.set_option(boost::asio::serial_port_base::baud_rate(baudRate));
    serial_.set_option(boost::asio::serial_port_base::character_size(8));
    serial_.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial_.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial_.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
}

SerialTask::~SerialTask()
{
    Stop();
}

void SerialTask::Start()
{
    LOG_INFO_THIS("start serial task");
    if (!running_.load())
    {
        running_.store(true);
        thread_.start(*this);
    }
}

void SerialTask::Stop()
{
    if (running_.load())
    {
        running_.store(false);
        thread_.join();
    }
}


void SerialTask::run()
{
    try
    {
        while (running_.load())
        {

            if (serial_.is_open() && !readingInProgress_)
            {
                std::vector<uint8_t> buffer(4096);
                readingInProgress_ = true;
                boost::asio::async_read(serial_, boost::asio::buffer(buffer),
                    std::bind(&SerialTask::handleRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, buffer));
                // 启动超时定时器
                timeoutTimer_.expires_after(boost::asio::chrono::seconds(readTimeoutSecond_));
                timeoutTimer_.async_wait(std::bind(&SerialTask::handleTimeout, this, boost::asio::placeholders::error));
                io_.run_one();
                LOG_INFO_THIS("serial task read finished!");
            }
            Poco::Thread::sleep(1);
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR_THIS("Error: " << e.what());
    }
}

void SerialTask::handleRead(const boost::system::error_code& ec, std::size_t bytesRead, std::vector<uint8_t>& buffer)
{
    readingInProgress_ = false;
    writingInProgress_ = false;
    if (!ec && cb_.lock())
    {
        buffer.resize(bytesRead);
        cb_.lock()->OnRecvResponse(buffer);
    }
    else
    {
        LOG_ERROR_THIS("Read error: " << ec.message());
    }
    timeoutTimer_.cancel();

}

void SerialTask::handleTimeout(const boost::system::error_code& ec)
{
    if (!ec)
    {
        LOG_ERROR_THIS("Read timeout");
        // 取消正在进行的异步读取操作
        timeoutTimer_.cancel();
        readingInProgress_ = false;
        writingInProgress_ = false;
        // TO DO 超时处理
    }
}

void SerialTask::Write(const void* data, uint32_t size)
{
    LOG_INFO_THIS("serial task write is open=" << serial_.is_open() << ", writingInProgress=" << writingInProgress_);
    // 收到请求响应或者响应超时后 才能继续发送请求
    if (serial_.is_open() && !writingInProgress_)
    {
        writingInProgress_ = true;
        boost::asio::write(serial_, boost::asio::buffer(data, size));
    }
}

}
