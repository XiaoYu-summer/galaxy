#pragma once
#include <algorithm>
#include <map>
#include "Poco/Logger.h"

#include "code/StringUtils.h"
#include "common/Packet.h"

// 协议头
#define PROTOCOL_HEADER 0x5A1AA1A5

// 功能号
enum class FunctionCode : uint16_t
{
    PL_FUN_NETINFO_GET = 0x00C0,      // 获取MCU网络信息
    PL_FUN_NETINFO_SET = 0x00C1,      // 设置MCU网络信息
    PL_FUN_DEVICE_MARK = 0x0064,      // 设备寻址
    PL_FUN_DEVICE_NAME_GET = 0x020B,  // 设备名称获取
};

const std::map<std::string, FunctionCode> FUNCTION_CODE_MAP = {
    std::pair<std::string, FunctionCode>("get netInfo", FunctionCode::PL_FUN_NETINFO_GET),
    std::pair<std::string, FunctionCode>("set netInfo", FunctionCode::PL_FUN_NETINFO_SET),
    std::pair<std::string, FunctionCode>("device mark", FunctionCode::PL_FUN_DEVICE_MARK),
    std::pair<std::string, FunctionCode>("get device name", FunctionCode::PL_FUN_DEVICE_NAME_GET),
};

inline std::string GetFunctionCodeStr(const uint16_t code)
{
    std::string codeStr = "unknowCode";
    auto it = std::find_if(FUNCTION_CODE_MAP.begin(), FUNCTION_CODE_MAP.end(),
                           [&code](const std::pair<const std::string, FunctionCode>& pair)
                           { return FunctionCode(code) == pair.second; });

    if (it != FUNCTION_CODE_MAP.end())
    {
        codeStr = it->first;
    }
    return codeStr;
}

struct MessageHeader
{
    uint32_t frameHeader_ = PROTOCOL_HEADER;
    uint32_t productID_ = 0;
    uint16_t deviceID_ = 0;
    uint16_t functionCode_ = 0;
};

struct McuNetInfo
{
    uint8_t mac_[6] = {0};
    uint8_t ip_[4] = {0};
    uint8_t mask_[4] = {0};
    uint8_t gw_[4] = {0};
    uint8_t dhcpMode_ = 0;
    uint8_t unused1 = 0;
    uint8_t unused2 = 0;
    uint8_t unused3 = 0;
};

// 计算校验和
inline uint32_t CalculateChecksum(uint32_t dataLen, const uint32_t* data)
{
    auto sum = dataLen;
    for (auto i = 0; i < dataLen; i++)
    {
        sum += data[i];
    }
    sum = ~sum + 1;
    return sum;
}

// 验证检验和
inline void VerifyChecksum(uint32_t currentChecksum, uint32_t checksum)
{
    if (currentChecksum != checksum)
    {
        std::runtime_error("data error!");
    }
}

// 公共部分，消息头
class CommonMessage
{
public:
    CommonMessage();
    virtual ~CommonMessage() = default;
    virtual bool Serialize(Binary::Pack& pack);
    virtual bool Deserialize(const Binary::Unpack& unpack);
    virtual void SerializeBody(Binary::Pack& pack) {}
    virtual void DeserializeBody(const Binary::Unpack& unpack) {}

    virtual void SerializeHeader(Binary::Pack& pack);

    virtual void DeserializeHeader(const Binary::Unpack& unpack);

    MessageHeader messageHeader_;
    int8_t headerSize_ = 0;
protected:
    Poco::Logger& logger_;
};

/********************************************通信消息********************************************************/
// 获取MCU网络信息请求消息
class McuNetInfoGetRequestMsg : public CommonMessage
{
};

// 获取MCU网络信息响应消息
class McuNetInfoGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    McuNetInfo netInfo_;
};

// 配置MCU网络信息请求消息
class McuNetInfoSetRequestMsg : public CommonMessage
{
public:
    virtual void SerializeBody(Binary::Pack& pack) override;
    
    McuNetInfo netInfo_;
};

// 设备名称获取请求消息
class DeviceNameGetRequestMsg : public CommonMessage
{
};

// 设备名称获取请求消息
class DeviceNameGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    std::string name_;
};
