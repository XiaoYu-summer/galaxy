#include "AsyncProtocol.h"
#include "Logger.h"
#include "code/ErrorCode.h"

namespace aoip
{

uint32_t RequestManager::AddRequest(std::shared_ptr<Request> request)
{
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t requestId = nextRequestId_++;
    requests_[requestId] = request;
    return requestId;
}

bool RequestManager::MatchResponse(const std::vector<uint8_t>& response)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string recvFunctionCode;
    if (udpCallback_.lock())
    {
        recvFunctionCode = udpCallback_.lock()->GetFunctionCode(response);
    }
    else
    {
        AOIP_LOG_ERROR("udp callback is null!");
        return false;
    }

    for (auto it = requests_.begin(); it != requests_.end(); ++it)
    {
        if (it->second->functionCode_ == recvFunctionCode)
        {
            it->second->promise_.set_value(response);
            requests_.erase(it);
            return true;
        }
    }
    return false;
}

void RequestManager::CleanTimeouts()
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();

    for (auto it = requests_.begin(); it != requests_.end();)
    {
        auto& request = it->second;
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - request->timestamp_).count();

        if (elapsed > request->timeoutMs_)
        {
            request->promise_.set_exception(std::make_exception_ptr(std::runtime_error("Request timeout")));
            it = requests_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void RequestManager::SetUdpCallback(std::shared_ptr<UdpCallback> cb)
{
    udpCallback_ = cb;
}

AsyncProtocol::AsyncProtocol(const ProtocolConfig& config)
    : config_(config),
      socket_(std::make_unique<UdpSocket>(MakeUDPConfig(config))),
      requestManager_(std::make_unique<RequestManager>())
{
}

AsyncProtocol::~AsyncProtocol() { Stop(); }

void AsyncProtocol::Start()
{
    if (running_) return;

    running_ = true;
    receiverThread_ = std::thread(&AsyncProtocol::ReceiverLoop, this);
    timeoutThread_ = std::thread(&AsyncProtocol::TimeoutLoop, this);
}

void AsyncProtocol::Stop()
{
    if (!running_) return;

    running_ = false;
    if (receiverThread_.joinable())
    {
        receiverThread_.join();
    }
    if (timeoutThread_.joinable())
    {
        timeoutThread_.join();
    }
}

std::future<std::vector<uint8_t>> AsyncProtocol::SendRequest(const std::string& functionCode, const void* data, size_t len)
{
    if (!running_)
    {
        RUNTIME_EXCEPTION("Protocol not started");
    }

    auto request = std::make_shared<Request>(functionCode, config_.timeoutMs_);
    auto requestId = requestManager_->AddRequest(request);

    if (config_.broadcast_)
    {
        socket_->Broadcast(data, len, config_.masterPort_);
    } 
    else
    {
        socket_->SendTo(data, len, config_.masterIp_, config_.masterPort_);
    }

    AOIP_LOG_DEBUG("Sent request: " << request->functionCode_);

    return request->promise_.get_future();
}

void AsyncProtocol::SetUdpCallback(std::shared_ptr<UdpCallback> cb)
{
    if (requestManager_)
    {
        requestManager_->SetUdpCallback(cb);
    }
}

void AsyncProtocol::ReceiverLoop()
{
    std::vector<uint8_t> buffer;
    std::string fromIp;
    uint16_t fromPort;

    while (running_)
    {
        try {
            if (!socket_->RecvFrom(buffer, fromIp, fromPort))
            {
                continue;
            }

            AOIP_LOG_DEBUG("Received response: ");

            if (!requestManager_->MatchResponse(buffer))
            {
                AOIP_LOG_WARN("Unmatched response received");
            }

        }
        catch (const std::exception& e)
        {
            AOIP_LOG_ERROR("Error in receiver loop: " << e.what());
        }
    }
}

void AsyncProtocol::TimeoutLoop() {
    while (running_)
    {
        try
        {
            requestManager_->CleanTimeouts();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        catch (const std::exception& e)
        {
            AOIP_LOG_ERROR("Error in timeout loop: " << e.what());
        }
    }
}

UdpConfig AsyncProtocol::MakeUDPConfig(const ProtocolConfig& config)
{
    UdpConfig udpConfig;
    udpConfig.bindIp_ = config.masterIp_;
    udpConfig.bindPort_ = config.masterPort_;
    udpConfig.broadcast_ = config.broadcast_;
    udpConfig.timeoutMs_ = config.timeoutMs_;
    return udpConfig;
}

}  // namespace aoip
