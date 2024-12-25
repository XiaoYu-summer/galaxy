#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

#include "logger.h"
#include "memory_pool.h"

struct ProtocolConfig {
    uint16_t slavePort_{50000};
    uint16_t masterPort_{60000};
    uint32_t timeoutMs_{1000};
    uint32_t maxRetries_{3};
    uint32_t productId_{0x02020483};
    uint16_t deviceId_{0xFFFF};

    bool enableLogging_{true};
    std::string logFile_{"protocol.log"};
    aoip::LogLevel logLevel_{aoip::LogLevel::INFO};
    bool broadcast_{true};
    size_t recvBufferSize_{4096};
};

class Frame {
   public:
    static constexpr uint32_t FrameHeader = 0x5A1AA1A5;

    Frame() = default;
    Frame(uint16_t functionCode_, const std::vector<uint32_t>& data_);

    std::vector<uint8_t> Serialize() const;
    bool Deserialize(const std::vector<uint8_t>& data_);
    uint32_t CalculateChecksum() const;

    uint16_t GetFunctionCode() const { return functionCode_; }
    const std::vector<uint32_t>& GetData() const { return data_; }

    std::string ToString() const;
    bool Validate() const;

   private:
    uint32_t productId_{0x02020483};
    uint16_t deviceId_{0xFFFF};
    uint16_t functionCode_{0};
    std::vector<uint32_t> data_;
    uint32_t checksum_{0};
};