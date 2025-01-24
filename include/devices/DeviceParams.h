#pragma once
#include <string>

enum class DeviceType : uint8_t
{
    MASTER_HOST,    // 主控主机
    WIRELESS_HOST,  // 无线主机
    WIRED_MIC,      // 有线MIC
    WIRELESS_MIC,   // 无线MIC
    POE_SPEAKER,    // POE音箱
    PAT71,
    BM50,
    UNKNOW  
};

enum class DeviceVendor : uint8_t
{
    KINGRAY,
    DIGISYN,
    UNKNOW
};

struct DeviceNetworkInfo
{
    DeviceType   deviceType;     // 设备类型
    DeviceVendor deviceVendor;   // 设备厂商
    std::string  deviceId;       // 设备ID
    std::string  unicastIp;      // 单播IP
    uint16_t     unicastPort;    // 单播端口
    std::string  multicastIp;    // 组播IP
    uint16_t     multicastPort;  // 组播端口
};

struct DeviceInfo
{
    std::string id;
};

struct DeviceAddress
{

};

struct DeviceVersion
{};
