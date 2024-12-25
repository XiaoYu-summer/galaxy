#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>

#include "UdpSocket.h"

namespace aoip {

UdpSocket::UdpSocket(const UdpConfig& config) : config_(config) {
    if (!Init()) {
        throw std::runtime_error(GetLastError());
    }
}

UdpSocket::~UdpSocket() {
    if (socket_ >= 0) {
        close(socket_);
        socket_ = -1;
    }
}

bool UdpSocket::Init() {
    // Create socket
    socket_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_ < 0) {
        SetError("Failed to create socket");
        return false;
    }

    // Set socket options
    if (!SetSocketOptions()) {
        close(socket_);
        socket_ = -1;
        return false;
    }

    // Bind socket
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config_.bindPort_);
    addr.sin_addr.s_addr = inet_addr(config_.bindIp_.c_str());

    if (bind(socket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        SetError("Failed to bind socket");
        close(socket_);
        socket_ = -1;
        return false;
    }

    return true;
}

bool UdpSocket::SetSocketOptions() {
    // Set reuse address
    int option = 1;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
        SetError("Failed to set SO_REUSEADDR");
        return false;
    }

    // Set broadcast if enabled
    if (config_.broadcast_) {
        if (setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, &option, sizeof(option)) < 0) {
            SetError("Failed to set SO_BROADCAST");
            return false;
        }
    }

    // Set send buffer size
    int sendBufSize = config_.sendBufferSize_;
    if (setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, &sendBufSize, sizeof(sendBufSize)) < 0) {
        SetError("Failed to set SO_SNDBUF");
        return false;
    }

    // Set receive buffer size
    int recvBufSize = config_.recvBufferSize_;
    if (setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, &recvBufSize, sizeof(recvBufSize)) < 0) {
        SetError("Failed to set SO_RCVBUF");
        return false;
    }

    // Set receive timeout
    struct timeval tv;
    tv.tv_sec = config_.timeoutMs_ / 1000;
    tv.tv_usec = (config_.timeoutMs_ % 1000) * 1000;
    if (setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        SetError("Failed to set SO_RCVTIMEO");
        return false;
    }

    // Set send timeout
    if (setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        SetError("Failed to set SO_SNDTIMEO");
        return false;
    }

    return true;
}

bool UdpSocket::SendTo(const void* data, size_t len, const std::string& ip, uint16_t port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    ssize_t sent = sendto(socket_, data, len, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (sent < 0) {
        SetError("Failed to send data");
        return false;
    }

    if (static_cast<size_t>(sent) != len) {
        SetError("Failed to send all data");
        return false;
    }

    return true;
}

bool UdpSocket::Broadcast(const void* data, size_t len, uint16_t port) {
    if (!config_.broadcast_) {
        SetError("Broadcast not enabled");
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_BROADCAST;

    ssize_t sent = sendto(socket_, data, len, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (sent < 0) {
        SetError("Failed to broadcast data");
        return false;
    }

    if (static_cast<size_t>(sent) != len) {
        SetError("Failed to broadcast all data");
        return false;
    }

    return true;
}

bool UdpSocket::RecvFrom(void* buffer, size_t& len, std::string& fromIp, uint16_t& fromPort, int timeoutMs) {
    if (timeoutMs >= 0) {
        struct pollfd fds;
        fds.fd = socket_;
        fds.events = POLLIN;
        int ret = poll(&fds, 1, timeoutMs);
        if (ret < 0) {
            SetError("Poll failed");
            return false;
        }
        if (ret == 0) {
            SetError("Receive timeout");
            return false;
        }
    }

    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    memset(&addr, 0, addrLen);

    ssize_t received = recvfrom(socket_, buffer, len, 0, (struct sockaddr*)&addr, &addrLen);
    if (received < 0) {
        SetError("Failed to receive data");
        return false;
    }

    len = received;
    fromIp = inet_ntoa(addr.sin_addr);
    fromPort = ntohs(addr.sin_port);

    return true;
}

bool UdpSocket::RecvFrom(std::vector<uint8_t>& data, std::string& fromIp, uint16_t& fromPort, int timeoutMs) {
    data.resize(config_.recvBufferSize_);
    size_t len = data.size();

    if (!RecvFrom(data.data(), len, fromIp, fromPort, timeoutMs)) {
        return false;
    }

    data.resize(len);
    return true;
}

void UdpSocket::SetError(const char* msg) {
    lastError_ = msg;
    if (errno != 0) {
        lastError_ += ": ";
        lastError_ += strerror(errno);
    }
    AOIP_LOG_ERROR(lastError_);
}

bool UdpSocket::GetLocalAddress(std::string& ip, uint16_t& port) const {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    if (getsockname(socket_, (struct sockaddr*)&addr, &addrLen) < 0) {
        return false;
    }

    ip = inet_ntoa(addr.sin_addr);
    port = ntohs(addr.sin_port);
    return true;
}

}  // namespace aoip