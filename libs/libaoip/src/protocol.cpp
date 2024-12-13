#include <iomanip>
#include <sstream>

#include "protocol.h"

Frame::Frame(uint16_t func_code, const std::vector<uint32_t>& data) : function_code_(func_code), data_(data) {
    checksum_ = calculateChecksum();
}

std::vector<uint8_t> Frame::serialize() const {
    // 使用内存池分配序列化缓冲区
    auto buffer = MemoryPool<4096>::instance().getBuffer();
    size_t pos = 0;

    // 帧头
    buffer.data()[pos++] = FRAME_HEADER & 0xFF;
    buffer.data()[pos++] = (FRAME_HEADER >> 8) & 0xFF;
    buffer.data()[pos++] = (FRAME_HEADER >> 16) & 0xFF;
    buffer.data()[pos++] = (FRAME_HEADER >> 24) & 0xFF;

    // 产品ID
    buffer.data()[pos++] = product_id_ & 0xFF;
    buffer.data()[pos++] = (product_id_ >> 8) & 0xFF;
    buffer.data()[pos++] = (product_id_ >> 16) & 0xFF;
    buffer.data()[pos++] = (product_id_ >> 24) & 0xFF;

    // 设备ID
    buffer.data()[pos++] = device_id_ & 0xFF;
    buffer.data()[pos++] = (device_id_ >> 8) & 0xFF;

    // 功能码
    buffer.data()[pos++] = function_code_ & 0xFF;
    buffer.data()[pos++] = (function_code_ >> 8) & 0xFF;

    // 数据长度
    uint32_t data_len = data_.size();
    buffer.data()[pos++] = data_len & 0xFF;
    buffer.data()[pos++] = (data_len >> 8) & 0xFF;
    buffer.data()[pos++] = (data_len >> 16) & 0xFF;
    buffer.data()[pos++] = (data_len >> 24) & 0xFF;

    // 数据
    for (uint32_t d : data_) {
        buffer.data()[pos++] = d & 0xFF;
        buffer.data()[pos++] = (d >> 8) & 0xFF;
        buffer.data()[pos++] = (d >> 16) & 0xFF;
        buffer.data()[pos++] = (d >> 24) & 0xFF;
    }

    // 校验和
    uint32_t checksum = calculateChecksum();
    buffer.data()[pos++] = checksum & 0xFF;
    buffer.data()[pos++] = (checksum >> 8) & 0xFF;
    buffer.data()[pos++] = (checksum >> 16) & 0xFF;
    buffer.data()[pos++] = (checksum >> 24) & 0xFF;

    return std::vector<uint8_t>(buffer.data(), buffer.data() + pos);
}

bool Frame::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() < 20) return false;

    // 使用内存池分配临时缓冲区
    auto buffer = MemoryPool<4096>::instance().getBuffer();
    std::memcpy(buffer.data(), data.data(), data.size());

    size_t pos = 0;

    // 检查帧头
    uint32_t header = (buffer.data()[pos + 3] << 24) | (buffer.data()[pos + 2] << 16) | (buffer.data()[pos + 1] << 8) |
                      buffer.data()[pos];
    if (header != FRAME_HEADER) return false;
    pos += 4;

    // 解析产品ID
    product_id_ = (buffer.data()[pos + 3] << 24) | (buffer.data()[pos + 2] << 16) | (buffer.data()[pos + 1] << 8) |
                  buffer.data()[pos];
    pos += 4;

    // 解析设备ID
    device_id_ = (buffer.data()[pos + 1] << 8) | buffer.data()[pos];
    pos += 2;

    // 解析功能码
    function_code_ = (buffer.data()[pos + 1] << 8) | buffer.data()[pos];
    pos += 2;

    // 解析数据长度
    uint32_t data_len = (buffer.data()[pos + 3] << 24) | (buffer.data()[pos + 2] << 16) |
                        (buffer.data()[pos + 1] << 8) | buffer.data()[pos];
    pos += 4;

    if (data.size() < pos + data_len * 4 + 4) return false;

    // 解析数据
    data_.clear();
    for (uint32_t i = 0; i < data_len; i++) {
        uint32_t d = (buffer.data()[pos + 3] << 24) | (buffer.data()[pos + 2] << 16) | (buffer.data()[pos + 1] << 8) |
                     buffer.data()[pos];
        data_.push_back(d);
        pos += 4;
    }

    // 解析校验和
    checksum_ = (buffer.data()[pos + 3] << 24) | (buffer.data()[pos + 2] << 16) | (buffer.data()[pos + 1] << 8) |
                buffer.data()[pos];

    return calculateChecksum() == checksum_;
}

uint32_t Frame::calculateChecksum() const {
    // 1. 从数据长度开始
    uint32_t sum = static_cast<uint32_t>(data_.size());

    // 2. 依次加上所有数据
    for (uint32_t d : data_) {
        // 确保只取低32位
        sum = (sum + d) & 0xFFFFFFFF;
    }

    // 3. 取反加1
    return (~sum + 1) & 0xFFFFFFFF;
}

std::string Frame::toString() const {
    std::stringstream ss;
    ss << "Frame{func_code=0x" << std::hex << function_code_ << ", data_len=" << std::dec << data_.size()
       << ", checksum=0x" << std::hex << checksum_ << "}";
    return ss.str();
}

bool Frame::validate() const {
    if (data_.size() > 1024) return false;
    return calculateChecksum() == checksum_;
}