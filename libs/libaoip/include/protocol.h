#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

#include "logger.h"
#include "memory_pool.h"

struct ProtocolConfig {
    uint16_t slave_port{50000};
    uint16_t master_port{60000};
    uint32_t timeout_ms{1000};
    uint32_t max_retries{3};
    uint32_t product_id{0x02020483};
    uint16_t device_id{0xFFFF};

    bool enable_logging{true};
    std::string log_file{"protocol.log"};
    aoip::LogLevel log_level{aoip::LogLevel::INFO};
    bool broadcast{true};
    size_t recv_buffer_size{4096};
};

class Frame {
   public:
    static constexpr uint32_t FRAME_HEADER = 0x5A1AA1A5;

    Frame() = default;
    Frame(uint16_t func_code, const std::vector<uint32_t>& data);

    std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& data);
    uint32_t calculateChecksum() const;

    uint16_t getFunctionCode() const { return function_code_; }
    const std::vector<uint32_t>& getData() const { return data_; }

    std::string toString() const;
    bool validate() const;

   private:
    uint32_t product_id_{0x02020483};
    uint16_t device_id_{0xFFFF};
    uint16_t function_code_{0};
    std::vector<uint32_t> data_;
    uint32_t checksum_{0};
};