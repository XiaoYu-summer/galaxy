#pragma once
#include <string>

// 设备类型
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

// 设备厂商
enum class DeviceVendor : uint8_t
{
    KINGRAY,
    DIGISYN,
    UNKNOW
};

// 身份标识
enum class IdType : uint16_t
{
    REGULAR_USER = 0,  // 普通用户
    VIP          = 1,  // VIP
    CHAIRMAN     = 2   // 主席
};

// 滤波器类型
enum class FilterType : uint8_t
{
    BUTTERWORTH_6DB  = 0,
    BUTTERWORTH_12DB = 1,
    BUTTERWORTH_18DB = 2,
    BUTTERWORTH_24DB = 3,
    BUTTERWORTH_36DB = 4,
    BUTTERWORTH_48DB = 5,
    BESSEL_6DB       = 6,
    BESSEL_12DB      = 7,
    BESSEL_18DB      = 8,
    BESSEL_24DB      = 9,
    BESSEL_36DB      = 10,
    BESSEL_48DB      = 11,
    LINKWITZ_12DB    = 12,
    LINKWITZ_24DB    = 13,
    LINKWITZ_36DB    = 14,
    LINKWITZ_48DB    = 15
};

// 输入模式
enum class InputMode : uint32_t
{
    SIMULATED_INPUT      = 0,   // 模拟输入
    SINE_WAVE            = 1,   // 正弦波
    PINK_NOISE           = 2,   // 分红噪声
    WHITE_NOISE          = 3,   // 白噪声
    DANTE                = 4,   // dante
    SOUND_CARD_INPUT     = 5    // 声卡输入
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
