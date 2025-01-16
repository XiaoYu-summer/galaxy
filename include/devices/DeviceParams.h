#pragma once
#include <string>

enum class DeviceType : uint8_t
{
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
    DeviceType deviceType;     // 设备类型
    DeviceVendor deviceVendor; // 设备厂商
    std::string deviceId;
    std::string unicastIp;   // 单播IP
    uint16_t unicastPort;    // 单播端口
    std::string multicastIp; // 组播IP
    uint16_t multicastPort;  // 组播端口
};

struct DeviceInfo
{
    
};

struct DeviceAddress
{

};

struct DeviceVersion
{};
