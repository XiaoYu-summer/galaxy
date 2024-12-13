#pragma once
#include <memory>
#include <string>
#include <vector>

#include "logger.h"
#include "memory_pool.h"

struct UDPConfig {
    std::string bind_ip{"0.0.0.0"};
    uint16_t bind_port{0};
    bool broadcast{false};
    size_t send_buffer_size{65535};
    size_t recv_buffer_size{65535};
    int timeout_ms{1000};
};

class UDPSocket {
   public:
    explicit UDPSocket(const UDPConfig& config);
    ~UDPSocket();

    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;

    bool sendTo(const void* data, size_t len, const std::string& ip, uint16_t port);

    bool sendTo(const std::vector<uint8_t>& data, const std::string& ip, uint16_t port) {
        return sendTo(data.data(), data.size(), ip, port);
    }

    bool broadcast(const void* data, size_t len, uint16_t port);

    bool broadcast(const std::vector<uint8_t>& data, uint16_t port) {
        return broadcast(data.data(), data.size(), port);
    }

    bool recvFrom(void* buffer, size_t& len, std::string& from_ip, uint16_t& from_port, int timeout_ms = -1);

    bool recvFrom(std::vector<uint8_t>& data, std::string& from_ip, uint16_t& from_port, int timeout_ms = -1);

    std::string getLastError() const { return last_error_; }
    bool getLocalAddress(std::string& ip, uint16_t& port) const;

   private:
    bool init();
    bool setSocketOptions();
    void setError(const char* msg);

    int sock_{-1};
    UDPConfig config_;
    std::string last_error_;
};