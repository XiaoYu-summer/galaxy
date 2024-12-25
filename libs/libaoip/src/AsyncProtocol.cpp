#include "AsyncProtocol.h"

namespace aoip {

uint32_t RequestManager::AddRequest(std::shared_ptr<Request> request) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t requestId = nextRequestId_++;
    requests_[requestId] = request;
    return requestId;
}

bool RequestManager::MatchResponse(const Frame& response) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto it = requests_.begin(); it != requests_.end(); ++it) {
        if (it->second->frame_.GetFunctionCode() == response.GetFunctionCode()) {
            it->second->promise_.set_value(response.GetData());
            requests_.erase(it);
            return true;
        }
    }
    return false;
}

void RequestManager::CleanTimeouts() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();

    for (auto it = requests_.begin(); it != requests_.end();) {
        auto& request = it->second;
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - request->timestamp_).count();

        if (elapsed > request->timeoutMs_) {
            request->promise_.set_exception(std::make_exception_ptr(std::runtime_error("Request timeout")));
            it = requests_.erase(it);
        } else {
            ++it;
        }
    }
}

AsyncProtocol::AsyncProtocol(const ProtocolConfig& config)
    : config_(config),
      socket_(std::make_unique<UdpSocket>(MakeUDPConfig(config))),
      requestManager_(std::make_unique<RequestManager>()) {
    if (config.enableLogging_) {
        DefaultLogger::GetInstance().SetLogFile(config.logFile_);
        DefaultLogger::GetInstance().SetLogLevel(config.logLevel_);
    }
}

AsyncProtocol::~AsyncProtocol() { Stop(); }

void AsyncProtocol::Start() {
    if (running_) return;

    running_ = true;
    receiverThread_ = std::thread(&AsyncProtocol::ReceiverLoop, this);
    timeoutThread_ = std::thread(&AsyncProtocol::TimeoutLoop, this);
}

void AsyncProtocol::Stop() {
    if (!running_) return;

    running_ = false;
    if (receiverThread_.joinable()) {
        receiverThread_.join();
    }
    if (timeoutThread_.joinable()) {
        timeoutThread_.join();
    }
}

std::future<std::vector<uint32_t>> AsyncProtocol::SendRequest(uint16_t funcCode, const std::vector<uint32_t>& data) {
    if (!running_) {
        throw std::runtime_error("Protocol not started");
    }

    auto request = std::make_shared<Request>(Frame(funcCode, data), config_.timeoutMs_);
    auto requestId = requestManager_->AddRequest(request);

    auto frameData = request->frame_.Serialize();
    if (config_.broadcast_) {
        socket_->Broadcast(frameData, config_.slavePort_);
    } else {
        socket_->SendTo(frameData, "127.0.0.1", config_.slavePort_);
    }

    AOIP_LOG_DEBUG("Sent request: " + request->frame_.ToString());

    return request->promise_.get_future();
}

void AsyncProtocol::ReceiverLoop() {
    std::vector<uint8_t> buffer;
    std::string fromIp;
    uint16_t fromPort;

    while (running_) {
        try {
            if (!socket_->RecvFrom(buffer, fromIp, fromPort)) {
                continue;
            }

            Frame response;
            if (!response.Deserialize(buffer)) {
                AOIP_LOG_ERROR("Failed to parse response");
                continue;
            }

            AOIP_LOG_DEBUG("Received response: " + response.ToString());

            if (!requestManager_->MatchResponse(response)) {
                AOIP_LOG_WARN("Unmatched response received");
            }

        } catch (const std::exception& e) {
            AOIP_LOG_ERROR(std::string("Error in receiver loop: ") + e.what());
        }
    }
}

void AsyncProtocol::TimeoutLoop() {
    while (running_) {
        try {
            requestManager_->CleanTimeouts();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } catch (const std::exception& e) {
            AOIP_LOG_ERROR(std::string("Error in timeout loop: ") + e.what());
        }
    }
}

UdpConfig AsyncProtocol::MakeUDPConfig(const ProtocolConfig& config) {
    UdpConfig udpConfig;
    udpConfig.bindPort_ = config.masterPort_;
    udpConfig.broadcast_ = config.broadcast_;
    udpConfig.timeoutMs_ = config.timeoutMs_;
    return udpConfig;
}

}  // namespace aoip