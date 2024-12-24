#include "async_protocol.h"

uint32_t RequestManager::addRequest(std::shared_ptr<Request> request) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t request_id = next_request_id_++;
    requests_[request_id] = request;
    return request_id;
}

bool RequestManager::matchResponse(const Frame& response) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto it = requests_.begin(); it != requests_.end(); ++it) {
        if (it->second->frame.getFunctionCode() == response.getFunctionCode()) {
            it->second->promise.set_value(response.getData());
            requests_.erase(it);
            return true;
        }
    }
    return false;
}

void RequestManager::cleanTimeouts() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();

    for (auto it = requests_.begin(); it != requests_.end();) {
        auto& request = it->second;
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - request->timestamp).count();

        if (elapsed > request->timeout_ms) {
            request->promise.set_exception(std::make_exception_ptr(std::runtime_error("Request timeout")));
            it = requests_.erase(it);
        } else {
            ++it;
        }
    }
}

AsyncProtocol::AsyncProtocol(const ProtocolConfig& config)
    : config_(config), socket_(std::make_unique<UDPSocket>(makeUDPConfig(config))), running_(false) {
    if (config.enable_logging) {
        aoip::DefaultLogger::instance().setLogFile(config.log_file);
        aoip::DefaultLogger::instance().setLogLevel(config.log_level);
    }
}

AsyncProtocol::~AsyncProtocol() { stop(); }

void AsyncProtocol::start() {
    if (running_) return;
    running_ = true;
    receiver_thread_ = std::thread(&AsyncProtocol::receiverLoop, this);
    timeout_thread_ = std::thread(&AsyncProtocol::timeoutLoop, this);
    AOIP_LOG_INFO("AsyncProtocol started");
}

void AsyncProtocol::stop() {
    if (!running_) return;
    running_ = false;

    if (receiver_thread_.joinable()) {
        receiver_thread_.join();
    }
    if (timeout_thread_.joinable()) {
        timeout_thread_.join();
    }

    AOIP_LOG_INFO("AsyncProtocol stopped");
}

std::future<std::vector<uint32_t>> AsyncProtocol::sendRequest(uint16_t func_code, const std::vector<uint32_t>& data) {
    auto request = std::make_shared<Request>(Frame(func_code, data), config_.timeout_ms);

    auto future = request->promise.get_future();

    request_manager_.addRequest(request);

    auto frame_data = request->frame.serialize();
    if (config_.broadcast) {
        socket_->broadcast(frame_data, config_.slave_port);
    } else {
        socket_->sendTo(frame_data, "127.0.0.1", config_.slave_port);
    }

    AOIP_LOG_DEBUG("Sent request: " + request->frame.toString());

    return future;
}

void AsyncProtocol::receiverLoop() {
    std::vector<uint8_t> buffer;
    std::string from_ip;
    uint16_t from_port;

    while (running_) {
        try {
            if (!socket_->recvFrom(buffer, from_ip, from_port)) {
                continue;
            }

            Frame response;
            if (!response.deserialize(buffer)) {
                AOIP_LOG_ERROR("Failed to parse response");
                continue;
            }

            AOIP_LOG_DEBUG("Received response: " + response.toString());

            if (!request_manager_.matchResponse(response)) {
                AOIP_LOG_WARN("Unmatched response received");
            }

        } catch (const std::exception& e) {
            AOIP_LOG_ERROR(std::string("Error in receiver loop: ") + e.what());
        }
    }
}

void AsyncProtocol::timeoutLoop() {
    while (running_) {
        try {
            request_manager_.cleanTimeouts();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } catch (const std::exception& e) {
            AOIP_LOG_ERROR(std::string("Error in timeout loop: ") + e.what());
        }
    }
}

UDPConfig AsyncProtocol::makeUDPConfig(const ProtocolConfig& config) {
    UDPConfig udp_config;
    udp_config.bind_port = config.master_port;
    udp_config.broadcast = config.broadcast;
    udp_config.timeout_ms = config.timeout_ms;
    return udp_config;
}