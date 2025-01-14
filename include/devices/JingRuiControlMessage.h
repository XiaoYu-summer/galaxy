#pragma once
#include <algorithm>
#include "common/Packet.h"
#include "code/StringUtils.h"


// 协议头
#define PROTOCOL_HEADER 0x5A1AA1A5


// 功能号
enum class FunctionCode : uint16_t
{
    PL_FUN_NETINFO_GET = 0x00C0,         // 获取MCU网络信息
    PL_FUN_NETINFO_SET = 0x00C1,         // 设置MCU网络信息
    PL_FUN_DEVICE_MARK = 0x0064,         // 设备寻址
    PL_FUN_DEVICE_NAME_GET = 0x020B,     // 设备名称获取
};

const std::map<std::string, FunctionCode> FUNCTION_CODE_MAP = {
    std::pair<std::string, FunctionCode>("get netInfo",                FunctionCode::PL_FUN_NETINFO_GET),
    std::pair<std::string, FunctionCode>("set netInfo",                FunctionCode::PL_FUN_NETINFO_SET),
    std::pair<std::string, FunctionCode>("device mark",                FunctionCode::PL_FUN_DEVICE_MARK),
    std::pair<std::string, FunctionCode>("get device name",            FunctionCode::PL_FUN_DEVICE_NAME_GET),
};
	
inline std::string GetFunctionCodeStr(const uint16_t code)
{
    std::string codeStr = "unknowCode";
    auto it = std::find_if(FUNCTION_CODE_MAP.begin(), FUNCTION_CODE_MAP.end(),
        [&code](const std::pair<const std::string, FunctionCode>& pair) {
        return FunctionCode(code) == pair.second;
    });

    if (it != FUNCTION_CODE_MAP.end()) {
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
uint32_t CalculateChecksum(uint32_t dataLen, const uint32_t* data)
{
    auto sum = dataLen; 
    for (auto i = 0; i < dataLen; i++) 
    { 
        sum += data[i];
    } 
    sum = ~ sum + 1;
    return sum;
}

// 验证检验和
void VerifyChecksum(uint32_t currentChecksum, uint32_t checksum)
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
    CommonMessage() = default;
    virtual ~CommonMessage() = default;
    virtual bool Serialize(Binary::Pack& pack)
    {
        try
        {
            // 消息头
            SerializeHeader(pack);
            // 消息体
            SerializeBody(pack);
        }
        catch (const std::exception& e)
        {
            AOIP_LOG_ERROR("pack serialize fail! " << e.what());
            return false;
        }
        return true;
    }
    virtual bool Deserialize(const Binary::Unpack& unpack)
    {
        try
        {
            // 消息头
            DeserializeHeader(unpack);
            // 消息体
            DeserializeBody(unpack);
        }
        catch(const std::exception& e)
        {
            AOIP_LOG_ERROR("pack deserialize error! " << e.what());
            return false;
        }
        return true;
    }
    virtual void SerializeBody(Binary::Pack& pack) {}
    virtual void DeserializeBody(const Binary::Unpack& unpack) {}

    virtual void SerializeHeader(Binary::Pack& pack)
    {
        pack << messageHeader_.frameHeader_ << messageHeader_.productID_
             << messageHeader_.deviceID_ << messageHeader_.functionCode_;
        headerSize_ = pack.size();
    }

    virtual void DeserializeHeader(const Binary::Unpack& unpack)
    {
        const auto totalSize = unpack.size();
        unpack >> messageHeader_.frameHeader_ >> messageHeader_.productID_
             >> messageHeader_.deviceID_ >> messageHeader_.functionCode_;
        headerSize_ = totalSize - unpack.size();
    }

    MessageHeader messageHeader_;
    int8_t headerSize_ = 0;
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
    virtual void DeserializeBody(const Binary::Unpack& unpack) override
    {
        uint32_t checksum = 0;
        uint32_t dataLen = 0;
        unpack >> dataLen;
        const auto sum = CalculateChecksum(dataLen, reinterpret_cast<const uint32_t*>(unpack.data()));
        Binary::ReadArray(unpack, netInfo_.mac_, sizeof(netInfo_.mac_));
        Binary::ReadArray(unpack, netInfo_.ip_, sizeof(netInfo_.ip_));
        Binary::ReadArray(unpack, netInfo_.mask_, sizeof(netInfo_.mask_));
        Binary::ReadArray(unpack, netInfo_.gw_, sizeof(netInfo_.gw_));
        unpack >> netInfo_.dhcpMode_ >> netInfo_.unused1 >> netInfo_.unused2 >> netInfo_.unused3 >> checksum;
        // 验证检验和
        VerifyChecksum(sum, checksum);
    }

    McuNetInfo netInfo_;
};

// 配置MCU网络信息请求消息
class McuNetInfoSetRequestMsg : public CommonMessage
{
public:
    virtual void SerializeBody(Binary::Pack& pack) override
    {
        // 消息体大小
        const auto dataLen = sizeof(netInfo_) / sizeof(uint32_t);
        pack << dataLen;
        const auto bodySize = pack.size();
        // 消息体
        WriteArray(pack, netInfo_.mac_, sizeof(netInfo_.mac_));
        WriteArray(pack, netInfo_.ip_, sizeof(netInfo_.ip_));
        WriteArray(pack, netInfo_.mask_, sizeof(netInfo_.mask_));
        WriteArray(pack, netInfo_.gw_, sizeof(netInfo_.gw_));
        pack << netInfo_.dhcpMode_ << netInfo_.unused1 << netInfo_.unused2 << netInfo_.unused3;
        // 计算校验和
        pack << CalculateChecksum(dataLen, reinterpret_cast<const uint32_t*>(pack.data() + bodySize));
    }
    
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
    virtual void DeserializeBody(const Binary::Unpack& unpack) override
    {
        uint32_t checksum = 0;
        uint32_t dataLen = 0;
        unpack >> dataLen;
        const auto sum = CalculateChecksum(dataLen, reinterpret_cast<const uint32_t*>(unpack.data()));
        uint32_t temp[6] = {0};
        ReadArray(unpack, temp, sizeof(temp));
        // 验证检验和
        VerifyChecksum(sum, checksum);
        name_ = StringUtils::Split(temp, sizeof(temp), ' ');
    }
    std::string name_;
};

