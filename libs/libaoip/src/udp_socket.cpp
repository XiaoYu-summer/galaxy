#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "udp_socket.h"

UDPSocket::UDPSocket(const UDPConfig& config) : config_(config) {
    if (!init()) {
        throw std::runtime_error("Failed to initialize UDP socket: " + last_error_);
    }
}

UDPSocket::~UDPSocket() {
    if (sock_ >= 0) {
        close(sock_);
        sock_ = -1;
    }
}

bool UDPSocket::init() {
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ < 0) {
        setError("Failed to create socket");
        return false;
    }

    if (!setSocketOptions()) {
        close(sock_);
        sock_ = -1;
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config_.bind_port);
    addr.sin_addr.s_addr = inet_addr(config_.bind_ip.c_str());

    if (bind(sock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        setError("Failed to bind socket");
        close(sock_);
        sock_ = -1;
        return false;
    }

    AOIP_LOG_INFO("UDP socket initialized on " + config_.bind_ip + ":" + std::to_string(config_.bind_port));
    return true;
}

bool UDPSocket::setSocketOptions() {
    if (setsockopt(sock_, SOL_SOCKET, SO_SNDBUF, &config_.send_buffer_size, sizeof(config_.send_buffer_size)) < 0) {
        setError("Failed to set send buffer size");
        return false;
    }

    if (setsockopt(sock_, SOL_SOCKET, SO_RCVBUF, &config_.recv_buffer_size, sizeof(config_.recv_buffer_size)) < 0) {
        setError("Failed to set receive buffer size");
        return false;
    }

    if (config_.broadcast) {
        int broadcast = 1;
        if (setsockopt(sock_, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
            setError("Failed to set broadcast option");
            return false;
        }
    }

    int flags = fcntl(sock_, F_GETFL, 0);
    if (flags < 0) {
        setError("Failed to get socket flags");
        return false;
    }
    if (fcntl(sock_, F_SETFL, flags | O_NONBLOCK) < 0) {
        setError("Failed to set non-blocking mode");
        return false;
    }

    return true;
}

bool UDPSocket::sendTo(const void* data, size_t len, const std::string& ip, uint16_t port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    ssize_t sent = sendto(sock_, data, len, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (sent < 0) {
        setError("Failed to send data");
        return false;
    }
    if (static_cast<size_t>(sent) != len) {
        setError("Incomplete send");
        return false;
    }

    AOIP_LOG_DEBUG("Sent " + std::to_string(len) + " bytes to " + ip + ":" + std::to_string(port));
    return true;
}

bool UDPSocket::broadcast(const void* data, size_t len, uint16_t port) {
    if (!config_.broadcast) {
        setError("Broadcast not enabled");
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    ssize_t sent = sendto(sock_, data, len, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (sent < 0) {
        setError("Failed to broadcast data");
        return false;
    }
    if (static_cast<size_t>(sent) != len) {
        setError("Incomplete broadcast");
        return false;
    }

    AOIP_LOG_DEBUG("Broadcast " + std::to_string(len) + " bytes to port " + std::to_string(port));
    return true;
}

bool UDPSocket::recvFrom(void* buffer, size_t& len, std::string& from_ip, uint16_t& from_port, int timeout_ms) {
    struct pollfd pfd;
    pfd.fd = sock_;
    pfd.events = POLLIN;

    int timeout = (timeout_ms < 0) ? config_.timeout_ms : timeout_ms;
    int ret = poll(&pfd, 1, timeout);

    if (ret < 0) {
        setError("Poll failed");
        return false;
    }
    if (ret == 0) {
        setError("Timeout");
        return false;
    }

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    ssize_t received = recvfrom(sock_, buffer, len, 0, (struct sockaddr*)&addr, &addr_len);
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            setError("Would block");
            return false;
        }
        setError("Failed to receive data");
        return false;
    }

    len = received;
    from_ip = inet_ntoa(addr.sin_addr);
    from_port = ntohs(addr.sin_port);

    AOIP_LOG_DEBUG("Received " + std::to_string(len) + " bytes from " + from_ip + ":" + std::to_string(from_port));
    return true;
}

bool UDPSocket::recvFrom(std::vector<uint8_t>& data, std::string& from_ip, uint16_t& from_port, int timeout_ms) {
    // 使用内存池分配接收缓冲区
    auto buffer = MemoryPool<65536>::instance().getBuffer();
    size_t len = buffer.size();

    if (!recvFrom(buffer.data(), len, from_ip, from_port, timeout_ms)) {
        return false;
    }

    data.assign(buffer.data(), buffer.data() + len);
    return true;
}

bool UDPSocket::getLocalAddress(std::string& ip, uint16_t& port) const {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (getsockname(sock_, (struct sockaddr*)&addr, &addr_len) < 0) {
        return false;
    }

    ip = inet_ntoa(addr.sin_addr);
    port = ntohs(addr.sin_port);
    return true;
}

void UDPSocket::setError(const char* msg) {
    last_error_ = std::string(msg) + ": " + strerror(errno);
    AOIP_LOG_ERROR(last_error_);
}