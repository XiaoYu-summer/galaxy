#include <iomanip>
#include <sstream>

#include "protocol.h"

Frame::Frame(uint16_t functionCode, const std::vector<uint32_t>& data)
    : functionCode_(functionCode), data_(data), checksum_(0) {
    checksum_ = CalculateChecksum();
}

std::vector<uint8_t> Frame::Serialize() const {
    auto& pool = MemoryPool<4096>::GetInstance();
    auto buffer = pool.GetBuffer();
    size_t pos = 0;

    // Write frame header
    auto* data = buffer.GetData();
    data[pos++] = FrameHeader & 0xFF;
    data[pos++] = (FrameHeader >> 8) & 0xFF;
    data[pos++] = (FrameHeader >> 16) & 0xFF;
    data[pos++] = (FrameHeader >> 24) & 0xFF;

    // Write product ID
    data[pos++] = productId_ & 0xFF;
    data[pos++] = (productId_ >> 8) & 0xFF;
    data[pos++] = (productId_ >> 16) & 0xFF;
    data[pos++] = (productId_ >> 24) & 0xFF;

    // Write device ID
    data[pos++] = deviceId_ & 0xFF;
    data[pos++] = (deviceId_ >> 8) & 0xFF;

    // Write function code
    data[pos++] = functionCode_ & 0xFF;
    data[pos++] = (functionCode_ >> 8) & 0xFF;

    // Write data length
    uint32_t dataLen = data_.size() * sizeof(uint32_t);
    data[pos++] = dataLen & 0xFF;
    data[pos++] = (dataLen >> 8) & 0xFF;
    data[pos++] = (dataLen >> 16) & 0xFF;
    data[pos++] = (dataLen >> 24) & 0xFF;

    // Write data
    for (uint32_t value : data_) {
        data[pos++] = value & 0xFF;
        data[pos++] = (value >> 8) & 0xFF;
        data[pos++] = (value >> 16) & 0xFF;
        data[pos++] = (value >> 24) & 0xFF;
    }

    // Write checksum
    data[pos++] = checksum_ & 0xFF;
    data[pos++] = (checksum_ >> 8) & 0xFF;
    data[pos++] = (checksum_ >> 16) & 0xFF;
    data[pos++] = (checksum_ >> 24) & 0xFF;

    return std::vector<uint8_t>(data, data + pos);
}

bool Frame::Deserialize(const std::vector<uint8_t>& data) {
    if (data.size() < 16) {  // Minimum frame size without data
        return false;
    }

    size_t pos = 0;

    // Read and verify frame header
    uint32_t header = (data[pos + 3] << 24) | (data[pos + 2] << 16) | (data[pos + 1] << 8) | data[pos];
    if (header != FrameHeader) {
        return false;
    }
    pos += 4;

    // Read product ID
    productId_ = (data[pos + 3] << 24) | (data[pos + 2] << 16) | (data[pos + 1] << 8) | data[pos];
    pos += 4;

    // Read device ID
    deviceId_ = (data[pos + 1] << 8) | data[pos];
    pos += 2;

    // Read function code
    functionCode_ = (data[pos + 1] << 8) | data[pos];
    pos += 2;

    // Read data length
    uint32_t dataLen = (data[pos + 3] << 24) | (data[pos + 2] << 16) | (data[pos + 1] << 8) | data[pos];
    pos += 4;

    if (dataLen % 4 != 0 || pos + dataLen + 4 > data.size()) {
        return false;
    }

    // Read data
    data_.clear();
    for (size_t i = 0; i < dataLen; i += 4) {
        uint32_t value = (data[pos + i + 3] << 24) | (data[pos + i + 2] << 16) | (data[pos + i + 1] << 8) | data[pos + i];
        data_.push_back(value);
    }
    pos += dataLen;

    // Read checksum
    checksum_ = (data[pos + 3] << 24) | (data[pos + 2] << 16) | (data[pos + 1] << 8) | data[pos];

    return Validate();
}

uint32_t Frame::CalculateChecksum() const {
    uint32_t sum = 0;
    sum += FrameHeader;
    sum += productId_;
    sum += deviceId_;
    sum += functionCode_;

    for (uint32_t d : data_) {
        sum += d;
    }

    return sum;
}

std::string Frame::ToString() const {
    std::stringstream ss;
    ss << "Frame[func=" << std::hex << std::setw(4) << std::setfill('0') << functionCode_ << ", data=" << data_.size()
       << " words]";
    return ss.str();
}

bool Frame::Validate() const { return checksum_ == CalculateChecksum(); }