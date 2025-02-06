#pragma once
#include <algorithm>
#include <map>
#include <vector>
#include "Poco/Logger.h"
#include "code/StringUtils.h"
#include "common/Packet.h"

// 协议头
#define PROTOCOL_HEADER 0x5A1AA1A5

// 功能号
enum class FunctionCode : uint16_t
{
    PL_FUN_SLAVE_RESPONSE                  = 0x0001,    // 从机响应
    PL_FUN_AUDIO_CONFIG_GET                = 0x0005,    // 获取音频模块配置信息
    PL_FUN_AUDIO_CONFIG_SET                = 0x0006,    // 设置音频模块配置信息
    PL_FUN_STARTUP_PRESET_SET              = 0x0007,    // 设置开机存档
    PL_FUN_PRESET_INFO_GET                 = 0x0008,    // 获取存档信息
    PL_FUN_PRESET_SAVE                     = 0x0009,    // 存档保存
    PL_FUN_PRESET_CALL                     = 0x000A,    // 存档调用
    PL_FUN_PRESET_NAME_GET                 = 0x0011,    // 获取存档名称
    PL_FUN_PRESET_DELETE                   = 0x0012,    // 存档删除
    PL_FUN_NETINFO_GET                     = 0x00C0,    // 获取MCU网络信息
    PL_FUN_NETINFO_SET                     = 0x00C1,    // 设置MCU网络信息
    PL_FUN_DEVICE_MARK                     = 0x0064,    // 设备寻址
    PL_FUN_GROUP_CODE_GET                  = 0x0130,    // 获取群组编码
    PL_FUN_GROUP_CODE_SET                  = 0x0131,    // 设置群组编码
    PL_FUN_MEETING_PARAM_GET               = 0x0132,    // 获取会议参数（会议模式、无线MIC最大发言数、有线MIC最大发言数）
    PL_FUN_MEETING_PARAM_SET               = 0x0133,    // 设置会议参数
    PL_FUN_WL_MIC_FREQ_ALLOW_SET           = 0x0135,    // 设置无线MIC对频
    PL_FUN_PAIR_MODE_GET                   = 0x0136,    // 获取配对模式
    PL_FUN_PAIR_MODE_SET                   = 0x0137,    // 设置配对模式
    PL_FUN_MANUAL_PAIRING_SET              = 0x0138,    // 手动配对设置
    PL_FUN_WL_DEV_REMOVE                   = 0x0139,    // 移除设备
    PL_FUN_CODE_REASSIGN_SET               = 0x013A,    // 编码ID重新排序
    PL_FUN_RST_FACT_SET                    = 0x013B,    // 恢复出厂设置
    PL_FUN_WL_HOST_INFO_GET                = 0x013C,    // 获取无线主机信息
    PL_FUN_AES_MDL_INFO_GET                = 0x013D,    // 获取AES模块信息(全部主控主机/无线主机/有线MIC/POE音箱)
    PL_FUN_MEETING_DEV_CODE_GET            = 0x013E,    // 获取全部会议设备编码
    PL_FUN_SINGLE_DEV_CODE_SET             = 0x013F,    // 设置单一设备编码修改
    PL_FUN_WL_HOST_NAME_GET                = 0x0140,    // 获取无线主机设备名称
    PL_FUN_WL_HOST_NAME_SET                = 0x0141,    // 设置无线主机设备名称
    PL_FUN_ALL_MIC_ID_TYPE_GET             = 0x0142,    // 获取身份类别(全部无线/有线MIC)
    PL_FUN_SINGLE_MIC_ID_TYPE_SET          = 0x0143,    // 设置单一MIC身份类别
    PL_FUN_ALL_WL_MIC_BTERY_LVL_GET        = 0x0144,    // 获取全部无线MIC电量
    PL_FUN_ALL_MIC_SPEAKER_VOL_GET         = 0x0145,    // 获取音量状态(全部有线MIC/无线MIC/POE音箱)
    PL_FUN_SINGLE_DEV_VOL_SET              = 0x0146,    // 设置单一设备音量状态
    PL_FUN_ALL_MIC_SPEAKER_VER_GET         = 0x0147,    // 获取版本信息(全部有线MIC/无线MIC/POE音箱)
    PL_FUN_MEETING_DEV_CLK_STA_GET         = 0x0148,    // 获取会议设备时钟状态
    PL_FUN_SINGLE_DEV_CLK_STA_SET          = 0x0149,    // 设置单一设备时钟状态
    PL_FUN_MEETING_DEV_NET_STA_GET         = 0x014A,    // 获取会议设备网络状态
    PL_FUN_SINGLE_DEV_NET_STA_SET          = 0x014B,    // 设置单一设备网络状态
    PL_FUN_MEETING_DEV_EVENT_STA_GET       = 0x014C,    // 获取会议设备事件状态
    PL_FUN_ALL_WL_WD_MIC_SPEAKER_TYPE_GET  = 0x014F,    // 获取细分类别(全部无线MIC/有线MIC/POE音箱)
    PL_FUN_ALL_MIC_SPEAKER_DEV_NAME_GET    = 0x0151,    // 获取设备名称(全部有线MIC/无线MIC/POE音箱)
    PL_FUN_SINGLE_MIC_SPEAKER_DEV_NAME_SET = 0x0152,    // 设置单一设备名称(有线MIC/无线MIC/POE音箱)
    PL_FUN_ALL_DEV_ONLINE_GET              = 0x0153,    // 获取在线状态(全部无线主机/有线MIC/无线MIC/POE音箱)
    PL_FUN_ALL_DEV_CHN_CFG_GET             = 0x0154,    // 获取通道配置(全部主控主机/无线主机/有线MIC/无线MIC/POE音箱)
    PL_FUN_SINGLE_DEV_CHN_CFG_SET          = 0x0155,    // 设置单一设备通道配置
    PL_FUN_SINGLE_DEVICE_NAME_GET          = 0x020B,    // 获取单一设备名称
    PL_FUN_UNKNOW
};

const std::map<std::string, FunctionCode> FUNCTION_CODE_MAP =
{
    std::pair<std::string, FunctionCode>("PL_FUN_SLAVE_RESPONSE",                  FunctionCode::PL_FUN_SLAVE_RESPONSE),
    std::pair<std::string, FunctionCode>("PL_FUN_NETINFO_GET",                     FunctionCode::PL_FUN_NETINFO_GET),
    std::pair<std::string, FunctionCode>("PL_FUN_NETINFO_SET",                     FunctionCode::PL_FUN_NETINFO_SET),
    std::pair<std::string, FunctionCode>("PL_FUN_DEVICE_MARK",                     FunctionCode::PL_FUN_DEVICE_MARK),
    std::pair<std::string, FunctionCode>("PL_FUN_AUDIO_CONFIG_GET",                FunctionCode::PL_FUN_AUDIO_CONFIG_GET),               
    std::pair<std::string, FunctionCode>("PL_FUN_AUDIO_CONFIG_SET",                FunctionCode::PL_FUN_AUDIO_CONFIG_SET),               
    std::pair<std::string, FunctionCode>("PL_FUN_STARTUP_PRESET_SET",              FunctionCode::PL_FUN_STARTUP_PRESET_SET),            
    std::pair<std::string, FunctionCode>("PL_FUN_PRESET_INFO_GET",                 FunctionCode::PL_FUN_PRESET_INFO_GET),               
    std::pair<std::string, FunctionCode>("PL_FUN_PRESET_SAVE",                     FunctionCode::PL_FUN_PRESET_SAVE),                   
    std::pair<std::string, FunctionCode>("PL_FUN_PRESET_CALL",                     FunctionCode::PL_FUN_PRESET_CALL),                   
    std::pair<std::string, FunctionCode>("PL_FUN_PRESET_NAME_GET",                 FunctionCode::PL_FUN_PRESET_NAME_GET),               
    std::pair<std::string, FunctionCode>("PL_FUN_PRESET_DELETE",                   FunctionCode::PL_FUN_PRESET_DELETE),                 
    std::pair<std::string, FunctionCode>("PL_FUN_NETINFO_GET",                     FunctionCode::PL_FUN_NETINFO_GET),                    
    std::pair<std::string, FunctionCode>("PL_FUN_NETINFO_SET",                     FunctionCode::PL_FUN_NETINFO_SET),                    
    std::pair<std::string, FunctionCode>("PL_FUN_DEVICE_MARK",                     FunctionCode::PL_FUN_DEVICE_MARK),                    
    std::pair<std::string, FunctionCode>("PL_FUN_GROUP_CODE_GET",                  FunctionCode::PL_FUN_GROUP_CODE_GET),                 
    std::pair<std::string, FunctionCode>("PL_FUN_GROUP_CODE_SET",                  FunctionCode::PL_FUN_GROUP_CODE_SET),                 
    std::pair<std::string, FunctionCode>("PL_FUN_MEETING_PARAM_GET",               FunctionCode::PL_FUN_MEETING_PARAM_GET),              
    std::pair<std::string, FunctionCode>("PL_FUN_MEETING_PARAM_SET",               FunctionCode::PL_FUN_MEETING_PARAM_SET),              
    std::pair<std::string, FunctionCode>("PL_FUN_WL_MIC_FREQ_ALLOW_SET",           FunctionCode::PL_FUN_WL_MIC_FREQ_ALLOW_SET),          
    std::pair<std::string, FunctionCode>("PL_FUN_PAIR_MODE_GET",                   FunctionCode::PL_FUN_PAIR_MODE_GET),                  
    std::pair<std::string, FunctionCode>("PL_FUN_PAIR_MODE_SET",                   FunctionCode::PL_FUN_PAIR_MODE_SET),                  
    std::pair<std::string, FunctionCode>("PL_FUN_MANUAL_PAIRING_SET",              FunctionCode::PL_FUN_MANUAL_PAIRING_SET),             
    std::pair<std::string, FunctionCode>("PL_FUN_WL_DEV_REMOVE",                   FunctionCode::PL_FUN_WL_DEV_REMOVE),                  
    std::pair<std::string, FunctionCode>("PL_FUN_CODE_REASSIGN_SET",               FunctionCode::PL_FUN_CODE_REASSIGN_SET),              
    std::pair<std::string, FunctionCode>("PL_FUN_RST_FACT_SET",                    FunctionCode::PL_FUN_RST_FACT_SET),                   
    std::pair<std::string, FunctionCode>("PL_FUN_WL_HOST_INFO_GET",                FunctionCode::PL_FUN_WL_HOST_INFO_GET),               
    std::pair<std::string, FunctionCode>("PL_FUN_AES_MDL_INFO_GET",                FunctionCode::PL_FUN_AES_MDL_INFO_GET),              
    std::pair<std::string, FunctionCode>("PL_FUN_MEETING_DEV_CODE_GET",            FunctionCode::PL_FUN_MEETING_DEV_CODE_GET),           
    std::pair<std::string, FunctionCode>("PL_FUN_SINGLE_DEV_CODE_SET",             FunctionCode::PL_FUN_SINGLE_DEV_CODE_SET),            
    std::pair<std::string, FunctionCode>("PL_FUN_WL_HOST_NAME_GET",                FunctionCode::PL_FUN_WL_HOST_NAME_GET),               
    std::pair<std::string, FunctionCode>("PL_FUN_WL_HOST_NAME_SET",                FunctionCode::PL_FUN_WL_HOST_NAME_SET),               
    std::pair<std::string, FunctionCode>("PL_FUN_ALL_MIC_ID_TYPE_GET",             FunctionCode::PL_FUN_ALL_MIC_ID_TYPE_GET),            
    std::pair<std::string, FunctionCode>("PL_FUN_SINGLE_MIC_ID_TYPE_SET",          FunctionCode::PL_FUN_SINGLE_MIC_ID_TYPE_SET),         
    std::pair<std::string, FunctionCode>("PL_FUN_ALL_WL_MIC_BTERY_LVL_GET",        FunctionCode::PL_FUN_ALL_WL_MIC_BTERY_LVL_GET),       
    std::pair<std::string, FunctionCode>("PL_FUN_ALL_MIC_SPEAKER_VOL_GET",         FunctionCode::PL_FUN_ALL_MIC_SPEAKER_VOL_GET),        
    std::pair<std::string, FunctionCode>("PL_FUN_SINGLE_DEV_VOL_SET",              FunctionCode::PL_FUN_SINGLE_DEV_VOL_SET),             
    std::pair<std::string, FunctionCode>("PL_FUN_ALL_MIC_SPEAKER_VER_GET",         FunctionCode::PL_FUN_ALL_MIC_SPEAKER_VER_GET),        
    std::pair<std::string, FunctionCode>("PL_FUN_MEETING_DEV_CLK_STA_GET",         FunctionCode::PL_FUN_MEETING_DEV_CLK_STA_GET),        
    std::pair<std::string, FunctionCode>("PL_FUN_SINGLE_DEV_CLK_STA_SET",          FunctionCode::PL_FUN_SINGLE_DEV_CLK_STA_SET),         
    std::pair<std::string, FunctionCode>("PL_FUN_MEETING_DEV_NET_STA_GET",         FunctionCode::PL_FUN_MEETING_DEV_NET_STA_GET),        
    std::pair<std::string, FunctionCode>("PL_FUN_SINGLE_DEV_NET_STA_SET",          FunctionCode::PL_FUN_SINGLE_DEV_NET_STA_SET),         
    std::pair<std::string, FunctionCode>("PL_FUN_MEETING_DEV_EVENT_STA_GET",       FunctionCode::PL_FUN_MEETING_DEV_EVENT_STA_GET),      
    std::pair<std::string, FunctionCode>("PL_FUN_ALL_WL_WD_MIC_SPEAKER_TYPE_GET",  FunctionCode::PL_FUN_ALL_WL_WD_MIC_SPEAKER_TYPE_GET), 
    std::pair<std::string, FunctionCode>("PL_FUN_ALL_MIC_SPEAKER_DEV_NAME_GET",    FunctionCode::PL_FUN_ALL_MIC_SPEAKER_DEV_NAME_GET),   
    std::pair<std::string, FunctionCode>("PL_FUN_SINGLE_MIC_SPEAKER_DEV_NAME_SET", FunctionCode::PL_FUN_SINGLE_MIC_SPEAKER_DEV_NAME_SET),
    std::pair<std::string, FunctionCode>("PL_FUN_ALL_DEV_ONLINE_GET",              FunctionCode::PL_FUN_ALL_DEV_ONLINE_GET),             
    std::pair<std::string, FunctionCode>("PL_FUN_ALL_DEV_CHN_CFG_GET",             FunctionCode::PL_FUN_ALL_DEV_CHN_CFG_GET),            
    std::pair<std::string, FunctionCode>("PL_FUN_SINGLE_DEV_CHN_CFG_SET",          FunctionCode::PL_FUN_SINGLE_DEV_CHN_CFG_SET),         
    std::pair<std::string, FunctionCode>("PL_FUN_SINGLE_DEVICE_NAME_GET",          FunctionCode::PL_FUN_SINGLE_DEVICE_NAME_GET)                
};

enum class ResponseCode : uint32_t
{
    NO_ERROR            = 0x00000000,  // 无错误
    DATA_ERROR          = 0x00000001,  // 数据错误
    CHECKSUM_ERROR      = 0x00000002,  // 校验和错误
    FUNCTION_CODE_ERROR = 0x00000005   // 功能号错误
};

// 功能号转字符串
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
    uint32_t frameHeader_  = PROTOCOL_HEADER;
    uint32_t productID_    = 0;
    uint16_t deviceID_     = 0;
    uint16_t functionCode_ = 0;
};

struct NetworkInfo
{
    uint8_t mac_[6]   = {0};
    uint8_t ip_[4]    = {0};
    uint8_t mask_[4]  = {0};
    uint8_t gw_[4]    = {0};
    uint8_t dhcpMode_ = 0;
    uint8_t reserve_  = 0;
};
// 设备类型基础信息
struct DeviceTypeBaseInfo
{
    uint8_t deviceType_ = 0;
    uint8_t reserve_[3] = {0};
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
    CommonMessage(uint16_t functionCode = 0);
    virtual ~CommonMessage() = default;
    bool Serialize(Binary::Pack& pack);
    bool Deserialize(const Binary::Unpack& unpack);
    virtual void SerializeBody(Binary::Pack& pack) {}
    virtual void DeserializeBody(const Binary::Unpack& unpack) {}

    virtual void SerializeHeader(Binary::Pack& pack);

    virtual void DeserializeHeader(const Binary::Unpack& unpack);

    MessageHeader messageHeader_;
    int8_t headerSize_ = 0;
protected:
    Poco::Logger& logger_;
};

// 获取功能号
inline uint16_t GetFunctionCodeByData(const std::vector<uint8_t>& data)
{
    Binary::Unpack unpack(data.data(), data.size());
    CommonMessage message;
    message.Deserialize(unpack);

    return message.messageHeader_.functionCode_;
}

/********************************************通信消息********************************************************/
// 从机响应消息
class SlaveResponseMsg : CommonMessage
{
public:
    SlaveResponseMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_SLAVE_RESPONSE))
        : CommonMessage(functionCode)
    {
    }
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    ResponseCode code_;    // 响应码
};

// 设置开机存档请求消息
class StartupPresetSetRequestMsg : public CommonMessage
{
public:
    StartupPresetSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_STARTUP_PRESET_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t startupPresetCode_ = 0;  // 开机存档号
};

// 获取存档信息请求消息
class PresetInfoGetRequestMsg : public CommonMessage
{
public:
    PresetInfoGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_PRESET_INFO_GET))
        : CommonMessage(functionCode)
    {
    }
};

// 获取存档信息响应消息
class PresetInfoGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    uint32_t startupPresetCode_  = 0;  // 开机存档号
    uint32_t validPresetFlags_   = 0;  // 有效存档标记
    uint32_t currentPresetCall_  = 0;  // 当前调用存档
};

// 存档保存请求消息
class PresetSaveRequestMsg : public CommonMessage
{
public:
    PresetSaveRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_PRESET_SAVE))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;
    struct PresetInfo
    {
        uint32_t savePresetCode_ = 0;   // 保存存档号
        uint8_t  presetName_[20] = {0}; // 存档名称（不超过20字节）
    };

    PresetInfo presetInfo_;
};

// 存档调用请求消息
class PresetCallRequestMsg : public CommonMessage
{
public:
    PresetCallRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_PRESET_CALL))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t presetCode_  = 0;  // 调用的存档号
};

// 获取存档名称请求消息
class PresetNameGetRequestMsg : public CommonMessage
{
public:
    PresetNameGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_PRESET_NAME_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t presetCode_  = 0;  // 存档号
};

// 获取存档名称响应消息
class PresetNameGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    uint32_t    presetCode_  = 0;  // 存档号
    std::string presetName_;       // 存档名称
};

// 删除存档请求消息
class DeletePresetRequestMsg : public CommonMessage
{
public:
    DeletePresetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_PRESET_DELETE))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t presetCode_  = 0;  // 存档号
};

// 获取MCU网络信息请求消息
class McuNetInfoGetRequestMsg : public CommonMessage
{
public:
    McuNetInfoGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_NETINFO_GET))
        : CommonMessage(functionCode)
    {
    }
};

// 获取MCU网络信息响应消息
class McuNetInfoGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    NetworkInfo netInfo_;
};

// 配置MCU网络信息请求消息
class McuNetInfoSetRequestMsg : public CommonMessage
{
public:
    McuNetInfoSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_NETINFO_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;
    
    NetworkInfo netInfo_;
};

// 设备寻址请求消息
class DeviceMarkRequestMsg : public CommonMessage
{
public:
    DeviceMarkRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_DEVICE_MARK))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct DeviceMark
    {
        uint8_t  action_     = 0;   // 0：停止， 1：开始
        uint8_t  deviceType_ = 0;   // 设备类型
        uint16_t deviceCode_ = 0;   // 设备编码
    };
    DeviceMark deviceMark_;
};

// 获取群组编码请求消息
class GroupCodeGetRequestMsg : public CommonMessage
{
public:
    GroupCodeGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_GROUP_CODE_GET))
        : CommonMessage(functionCode)
    {
    }
};

struct GroupInfo
{
    uint8_t  groupCode_[2] = {0};  // 群组编码
    uint16_t reserve_      = 0;    // 保留
};

// 获取群组编码响应消息
class GroupCodeGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& pack) override;

    GroupInfo groupInfo_;
};

// 设置群组编码请求消息
class GroupCodeSetRequestMsg : public CommonMessage
{
public:
    GroupCodeSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_GROUP_CODE_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    GroupInfo groupInfo_;
};

// 获取会议参数请求消息
class MeetingParamGetRequestMsg : public CommonMessage
{
public:
    MeetingParamGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_MEETING_PARAM_GET))
        : CommonMessage(functionCode)
    {
    }
};

struct MeetingParam
{
    uint8_t meetingMode_    = 0;  // 会议模式
    uint8_t wlMicSpeechMax_ = 0;  // 无线麦克最大发言数
    uint8_t wdMicSpeechMax_ = 0;  // 有线麦克最大发言数
    uint8_t reserve_        = 0;  // 保留
};

// 获取会议参数响应消息
class MeetingParamGetResponsetMsg : public CommonMessage
{
public:
    MeetingParamGetResponsetMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_MEETING_PARAM_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void DeserializeBody(const Binary::Unpack& pack) override;

    MeetingParam meetingParam_;
};

// 设置会议参数请求消息
class MeetingParamSetRequestMsg : public CommonMessage
{
public:
    MeetingParamSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_MEETING_PARAM_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    MeetingParam meetingParam_;
};

// 设置无线MIC对频请求消息
class WlMicFreqAllowSetRequestMsg : public CommonMessage
{
public:
    WlMicFreqAllowSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_WL_MIC_FREQ_ALLOW_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct FreqAllowInfo
    {
        uint8_t action_     = 0;    // 1: 发起对频，0：停止对频
        uint8_t reserve_[3] = {0};  // 保留
    };
    FreqAllowInfo freqAllowInfo_;
};

// 获取配对模式请求消息
class PairModeGetRequestMsg : public CommonMessage
{
public:
    PairModeGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_PAIR_MODE_GET))
        : CommonMessage(functionCode)
    {
    }
};


struct PairModeInfo
{
    uint8_t  deviceType_ = 0;  // 设备类型
    uint8_t  pairMode_   = 0;  // 0：自动配对，1：手动配对
    uint16_t reserve_    = 0;  // 保留
};

// 获取配对模式响应消息
class PairModeGetResponseMsg : public CommonMessage
{
public:
    PairModeSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_PAIR_MODE_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void DeserializeBody(const Binary::Unpack& pack) override;

    PairModeInfo pairModeInfo_;
};

// 设置配对模式请求消息
class PairModeSetRequestMsg : public CommonMessage
{
public:
    PairModeSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_PAIR_MODE_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    PairModeInfo pairModeInfo_;
};

// 手动配对设置请求消息
class ManualPairingSetRequestMsg : public CommonMessage
{
public:
    ManualPairingSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_MANUAL_PAIRING_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct ManualPairingInfo
    {
        uint8_t  deviceType_ = 0;  // 设备类型
        uint8_t  action_     = 0;  // 1：发起配对，0：停止配对
        uint16_t reserve_    = 0;  // 保留
    };
    ManualPairingInfo manualPairingInfo_;
};

// 移除设备请求消息
class DeviceRemoveRequestMsg : public CommonMessage
{
public:
    DeviceRemoveRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_WL_DEV_REMOVE))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct DeviceCodeInfo
    {
        uint8_t  deviceType_ = 0;  // 设备类型
        uint8_t  reserve_    = 0;  // 保留
        uint16_t deviceCode_ = 0;  // 设备编码  
    };
    DeviceCodeInfo deviceCodeInfo_;
};

// 编码ID重新排序
class CodeReassignSetRequestMsg : public CommonMessage
{
public:
    CodeReassignSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_WL_MIC_FREQ_ALLOW_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 恢复出厂设置
class RestoreFactorySetRequestMsg : public CommonMessage
{
public:
    RestoreFactorySetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_RST_FACT_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取无线主机信息请求消息
class WlHostInfoGetRequestMsg : public CommonMessage
{
public:
    WlHostInfoGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_WL_HOST_INFO_GET))
        : CommonMessage(functionCode)
    {
    }
};

// 获取无线主机信息响应消息
class WlHostInfoGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& pack) override;

    uint8_t ip_[4]   = {0};      // IP地址
    uint8_t reserve_ = 0;        // 保留
    uint8_t fwVersion_[3] = {0}; // 无线主机固件版本
    uint8_t hwVersion_[3] = {0}; // 无线主机硬件版本
};

// 获取 全部主控主机/无线主机/有线MIC/POE音箱AES模块信息 请求消息
class AesModuleInfoGetRequestMsg : public CommonMessage
{
public:
    AesModuleInfoGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AES_MDL_INFO_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取 全部主控主机/无线主机/有线MIC/POE音箱AES模块信息 响应消息
class AesModuleInfoGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& pack) override;

    struct AesInfo
    {
        NetworkInfo netInfo_;            // AES模块网络信息
        uint8_t     FwVersion_[4] = {0}; // AES模块固件版本
    };

    uint8_t deviceType_ = 0;  // 设备类型
    uint8_t reserve_    = 0;  // 保留
    std::vector<AesInfo> aesInfoVec_;
};

// 获取 全部会议设备编码 请求消息
class MeetingDeviceCodeGetRequestMsg : public CommonMessage
{
public:
    MeetingDeviceCodeGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_MEETING_DEV_CODE_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取 全部会议设备编码 响应消息
class MeetingDeviceCodeGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& pack) override;

    struct MeetingDeviceCode
    {
        uint8_t  mac_[6] = {0};  // mac地址
        uint16_t code_   = 0;    // 设备编码
    };

    uint8_t deviceType_ = 0;  // 设备类型
    uint8_t reserve_    = 0;  // 保留
    std::vector<MeetingDeviceCode> aesInfoVec_;
};

// 设置单一设备编码修改请求消息
class SingleDeviceCodeSetRequestMsg : public CommonMessage
{
public:
    SingleDeviceCodeSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_SINGLE_DEV_CODE_SET))
        : CommonMessage(functionCode)
    {
    }

    virtual void SerializeBody(Binary::Pack& pack) override;

    struct SingleDeviceCodeInfo
    {
        uint8_t  deviceType_ = 0;    // 设备类型
        uint8_t  reserve_    = 0;    // 保留
        uint8_t  mac_[6]     = {0};  // mac地址
        uint16_t code_       = 0;    // 设备编码
    };
    SingleDeviceCodeInfo singleDeviceCodeInfo_;
};

// 获取无线主机名称请求消息
class WlHostNameGetRequestMsg : public CommonMessage
{
public:
    WlHostNameGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_WL_HOST_NAME_GET))
        : CommonMessage(functionCode)
    {
    }
};

// 获取无线主机名称响应消息
class WlHostNameGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    std::string name_;  // 无线主机名称（不超过64字节）
};

// 获取MIC身份类别请求消息
class MicIdTypeGetRequestMsg : public CommonMessage
{
public:
    MicIdTypeGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_ALL_MIC_ID_TYPE_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取MIC身份类别响应消息
class MicIdTypeGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct IdTypeInfo
    {
        uint16_t deviceCode_ = 0; // 设备编码
        uint16_t idType_     = 0; // 身份类别，0：普通用户，1：VIP，2：主席
    };
    uint8_t deviceType_ = 0;    // 设备类型
    std::vector<IdTypeInfo> idTypeInfoVec_;
};

// 设置单一MIC身份类别请求消息
class SingleMicIdTypeSetRequestMsg : public CommonMessage
{
public:
    SingleMicIdTypeSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_SINGLE_MIC_ID_TYPE_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct SingleMicIdTypeInfo
    {
        uint8_t  deviceType_ = 0;  // 设备类型
        uint8_t  reserve_    = 0;  // 保留
        uint16_t deviceCode_ = 0;  // 设备编码
        uint16_t idType_     = 0;  // 身份类别，0：普通用户，1：VIP，2：主席
    };
    SingleMicIdTypeInfo singleMicIdTypeInfo_;
};

// 获取全部无线MIC电量请求消息
class AllWlMicBteryLvlGetRequestMsg : public CommonMessage
{
public:
    AllWlMicBteryLvlGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_ALL_WL_MIC_BTERY_LVL_GET))
        : CommonMessage(functionCode)
    {
    }
};

// 获取全部无线MIC电量响应消息
class AllWlMicBteryLvlGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    uint16_t deviceCode_ = 201; // 设备编码
    uint16_t bteryLvl    = 100; // 电池电量
};

// 获取音量状态请求消息(全部有线MIC/无线MIC/POE音箱)
class AllMicSpeakerVolGetRequestMsg : public CommonMessage
{
public:
    AllMicSpeakerVolGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_ALL_MIC_SPEAKER_VOL_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取音量状态响应消息(全部有线MIC/无线MIC/POE音箱)
class AllMicSpeakerVolGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct VolumeInfo
    {
        uint16_t deviceCode_ = 0; // 设备编码
        uint8_t  mute_       = 0; // 0：UNMUTE，1：MUTE
        uint8_t  reserve_    = 0;
        uint16_t volume_     = 0; // 音量
    };
    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_    = 0;
    std::vector<VolumeInfo> volumeInfoVec_;
};

// 设置单一设备音量状态请求消息
class SingleDevVolSetRequestMsg : public CommonMessage
{
public:
    SingleDevVolSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_SINGLE_DEV_VOL_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct SingleDevVolumeInfo
    {
        uint8_t  deviceType_ = 0;  // 设备类型
        uint8_t  reserve0_   = 0;  // 保留
        uint16_t deviceCode_ = 0;  // 设备编码
        uint8_t  mute_       = 0;  // 0：UNMUTE，1：MUTE
        uint8_t  reserve1_   = 0;
        uint16_t volume_     = 0;  // 音量
    };
    SingleDevVolumeInfo singleDevVolumeInfo_;
};

// 获取版本信息请求消息(全部有线MIC/无线MIC/POE音箱)
class AllMicSpeakerVerGetRequestMsg : public CommonMessage
{
public:
    AllMicSpeakerVerGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_ALL_MIC_SPEAKER_VER_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取版本信息响应消息(全部有线MIC/无线MIC/POE音箱)
class AllMicSpeakerVerGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct VersionInfo
    {
        uint16_t deviceCode_   = 0;   // 设备编码
        int8_t   fwVersion_[3] = {0}; // 固件版本
        uint8_t  hwVersion_[3] = {0}; // 硬件版本
    };
    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_    = 0;
    std::vector<VersionInfo> versionInfoVec_;
};

// 获取会议设备时钟状态请求消息
class MeetingDevClockStatusGetRequestMsg : public CommonMessage
{
public:
    MeetingDevClockStatusGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_MEETING_DEV_CLK_STA_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取会议设备时钟状态响应消息
class MeetingDevClockStatusGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct ClockInfo
    {
        uint16_t              deviceCode_     = 1; // 设备编码
        int8_t                sync_           = 0; // 是否同步，0：未同步，1：已同步
        uint8_t               mute_           = 0; // 是否静音，0：未静音，1：静音
        uint8_t               clockSrc_       = 0; // 时钟源，0：DANTE，1：其他
        uint8_t               domain_         = 0; // 域，0：Master，1：Slave
        uint8_t               preClock_       = 0; // 是否首选时钟，0：非首选，1：首选
        uint8_t               syncToExternal_ = 0; // 是否启用外部时钟，0：不启用，1：启用
        std::vector<uint32_t> multicastVec_;       // 主网组播，最大为32个地址
    };

    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_    = 0;
    std::vector<ClockInfo> clockInfoVec_;
};

// 设置单一设备时钟状态请求消息
class SingleDevClockStatusSetRequestMsg : public CommonMessage
{
public:
    SingleDevClockStatusSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_SINGLE_DEV_CLK_STA_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct SingleDevClockStatusInfo
    {
        uint8_t  deviceType_     = 0;  // 设备类型
        uint8_t  reserve_        = 0;  // 保留
        uint16_t deviceCode_     = 1;  // 设备编码
        uint8_t  preClock_       = 0;  // 是否首选时钟，0：非首选，1：首选
        uint8_t  syncToExternal_ = 0;  // 是否启用外部时钟，0：不启用，1：启用
    };
    SingleDevClockStatusInfo singleDevClockStatusInfo_;
};

// 获取会议设备网络状态请求消息
class MeetingDevNetStatusGetRequestMsg : public CommonMessage
{
public:
    MeetingDevNetStatusGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_MEETING_DEV_NET_STA_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取会议设备网络状态响应消息
class MeetingDevNetStatusGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct NetInfo
    {
        uint16_t deviceCode_      = 1; // 设备编码
        uint8_t  subscribe_       = 0; // 是否订阅，0：未订阅，1：已订阅
        uint8_t  mainNetStatus_   = 0; // 主DANTE网口连接速率
        uint8_t  sendBandwidth_   = 0; // 发送瞬时带宽
        uint8_t  rcvBandwidth_    = 0; // 接收瞬时带宽
        uint8_t  delaySetResult_  = 0; // 延 时设置结果，0：OK，1：FAIL
        uint8_t  delayStatus_     = 0; // 延 时状态
        uint8_t  packetLossStatus = 0; // 丢包状态
    };

    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_    = 0;
    std::vector<NetInfo> netInfoVec_;
};

// 设置单一设备网络状态请求消息
class SingleDevNetStatusSetRequestMsg : public CommonMessage
{
public:
    SingleDevNetStatusSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_SINGLE_DEV_NET_STA_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct SingleDevNetStatusInfo
    {
        uint8_t  deviceType_ = 0;    // 设备类型
        uint8_t  reserve_    = 0;    // 保留
        uint16_t deviceCode_ = 1;    // 设备编码
        uint16_t delaySet_   = 3000; // AES模块延时设置([1000 - 10000])
    };
    SingleDevNetStatusInfo singleDevNetStatusInfo_;
};

// 获取会议设备事件状态请求消息
class MeetingDevEventStatusGetRequestMsg : public CommonMessage
{
public:
    MeetingDevEventStatusGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_MEETING_DEV_EVENT_STA_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取会议设备事件状态响应消息
class MeetingDevEventStatusGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct EventInfo
    {
        uint16_t deviceCode_   = 1;   // 设备编码
        uint16_t timeStampY_   = 0;   // 年
        uint8_t  timeStampM_   = 0;   // 月
        uint8_t  timeStampD_   = 0;   // 日
        uint8_t  timeStampH_   = 0;   // 时
        uint8_t  timeStampMin_ = 0;   // 分
        uint8_t  timeStampS_   = 0;   // 秒
        uint8_t  reserved0_    = 0;   // 保留
        uint16_t timeStampMs_  = 0;   // 毫秒
        uint8_t  eventType_    = 0;   // 事件类型
        uint8_t  reserved1_    = 0;   // 保留
    };

    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_    = 0;
    std::vector<EventInfo> eventInfoVec_;
};

// 获取细分类别请求消息(全部无线MIC/有线MIC/POE音箱)
class AllWlWdSpeakerTypeGetRequestMsg : public CommonMessage
{
public:
    AllWlWdSpeakerTypeGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_ALL_WL_WD_MIC_SPEAKER_TYPE_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取细分类别响应消息(全部无线MIC/有线MIC/POE音箱)
class AllWlWdSpeakerTypeGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct DetailInfo
    {
        uint16_t deviceCode_ = 1;   // 设备编码
        uint16_t detailType_ = 0;   // 2：无线MIC-手持，3：无线MIC-坐式，4：无线MIC-主席，5：有线MIC，6：有线MIC-主席，7：音柱，8：吸顶音箱
    };

    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_    = 0;
    std::vector<DetailInfo> detailInfoVec_;
};

// 获取全部设备名称请求消息
class AllMicSpeakerDevNameGetRequestMsg : public CommonMessage
{
public:
    AllMicSpeakerDevNameGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_ALL_MIC_SPEAKER_DEV_NAME_GET))
        : CommonMessage(functionCode)
    {
    }

    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取全部设备名称响应消息
class AllMicSpeakerDevNameResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct NameInfo
    {
        uint16_t    deviceCode_; // 设备编码
        std::string name_;       // 设备名称，不超过24字节
    };
    
    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_    = 0;
    std::vector<NameInfo> nameInfoVec_;
};

// 设置单一设备名称请求消息（有线MIC/无线MIC/POE音箱）
class SingleMicSpeakerDevNameSetRequestMsg : public CommonMessage
{
public:
    SingleMicSpeakerDevNameSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_SINGLE_MIC_SPEAKER_DEV_NAME_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct SingleMicSpeakerDevNameInfo
    {
        uint8_t     deviceType_ = 0;   // 设备类型
        uint8_t     reserve_    = 0;   // 保留
        uint16_t    deviceCode_ = 1;   // 设备编码
        uint8_t     name_[24]   = {0}; // 设备名称，不超过24字节
    };
    SingleMicSpeakerDevNameInfo singleMicSpeakerDevNameInfo_;
};

// 获取全部设备在线状态请求消息(全部无线主机/有线MIC/无线MIC/POE音箱)
class AllDeviceOnlineGetRequestMsg : public CommonMessage
{
public:
    AllDeviceOnlineGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_ALL_DEV_ONLINE_GET))
        : CommonMessage(functionCode)
    {
    }

    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取全部设备在线状态响应消息(全部无线主机/有线MIC/无线MIC/POE音箱)
class AllDeviceOnlineResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct OnlineInfo
    {
        uint16_t deviceCode_; // 设备编码
        uint8_t  online_;     // 0：离线，1：在线
        uint8_t  reserve_;
    };
    
    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_    = 0;
    std::vector<OnlineInfo> onlineInfoVec_;
};

// 获取全部设备通道配置请求消息(全部主控主机/无线主机/有线MIC/无线MIC/POE音箱)
class AllDeviceChannelConfigGetRequestMsg : public CommonMessage
{
public:
    AllDeviceChannelConfigGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_ALL_DEV_CHN_CFG_GET))
        : CommonMessage(functionCode)
    {
    }

    virtual void SerializeBody(Binary::Pack& pack) override;

    DeviceTypeBaseInfo deviceTypeInfo_;
};

// 获取全部设备通道配置响应消息(全部主控主机/无线主机/有线MIC/无线MIC/POE音箱)
class AllDeviceChannelConfigResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    struct ChannelInfo
    {
        uint16_t deviceCode_;     // 设备编码
        uint8_t  recvChannelNum_; // 接收通道数
        uint8_t  sendChannelNum_; // 发送通道数
    };
    
    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_    = 0;
    std::vector<ChannelInfo> channelInfoVec_;
};

// 设置单一设备通道配置请求消息
/*
    对于主控主机，最大为32；
    对于无线主机，最大为0；
    对于有线MIC，最大为0；
    对于POE音箱，最大为1
*/
class SingleDevChannelConfigSetRequestMsg : public CommonMessage
{
public:
    SingleDevChannelConfigSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_SINGLE_DEV_CHN_CFG_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct SingleDevChannelConfigInfo
    {
        uint8_t  deviceType_     = 0;   // 设备类型
        uint8_t  reserve_        = 0;   // 保留
        uint16_t deviceCode_     = 1;   // 设备编码
        uint8_t  recvChannelNum_ = 0;   // 接收通道数 [0-32]
        uint8_t  sendChannelNum_ = 0;   // 发送通道数 [0-32]
    };
    SingleDevChannelConfigInfo singleDevChannelConfigInfo_;
};

// 获取单一设备名称请求消息
class SingleDeviceNameGetRequestMsg : public CommonMessage
{
public:
    SingleDeviceNameGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_SINGLE_DEVICE_NAME_GET))
        : CommonMessage(functionCode)
    {
    }
};

// 获取单一设备名称响应消息
class SingleDeviceNameGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    std::string name_;  // 设备名称，不超过24字节
};

/********************************************音频设置消息********************************************************/
// 最大音频输入、输出通道个数
#define MAX_AUDIO_CHANNEL_IN_COUNT    38
#define MAX_AUDIO_CHANNEL_OUT_COUNT   30

// 参量均衡 5段、10段、15段通道个数 
#define PARAMETRIC_EQ_5_BANDS_CHANNEL_COUNT     32
#define PARAMETRIC_EQ_10_BANDS_CHANNEL_COUNT    MAX_AUDIO_CHANNEL_OUT_COUNT
#define PARAMETRIC_EQ_15_BANDS_CHANNEL_COUNT    (MAX_AUDIO_CHANNEL_IN_COUNT - PARAMETRIC_EQ_5_BANDS_CHANNEL_COUNT)

// 增益通道索引起始值
#define GAIN_APPLY_TO_IN_AND_OUT_INDEX_START               1
// 参量均衡通道索引起始值
#define PEQ_APPLY_TO_IN_AND_OUT_INDEX_START                (GAIN_APPLY_TO_IN_AND_OUT_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 高通滤波通道索引起始值
#define HPF_APPLY_TO_IN_AND_OUT_INDEX_START                (PEQ_APPLY_TO_IN_AND_OUT_INDEX_START + PARAMETRIC_EQ_5_BANDS_CHANNEL_COUNT * 5 + PARAMETRIC_EQ_10_BANDS_CHANNEL_COUNT * 10 + PARAMETRIC_EQ_15_BANDS_CHANNEL_COUNT * 15)
// 低通滤波通道索引起始值
#define LPF_APPLY_TO_IN_AND_OUT_INDEX_START                (HPF_APPLY_TO_IN_AND_OUT_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 噪声门通道索引起始值
#define NG_APPLY_TO_IN_INDEX_START                         (LPF_APPLY_TO_IN_AND_OUT_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 噪声门旁通通道索引起始值
#define NG_BPS_APPLY_TO_IN_INDEX_START                     (NG_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 反馈抑制等级通道索引起始值
#define FE_LEVEL_APPLY_TO_IN_INDEX_START                   (NG_BPS_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 反馈抑制旁通通道索引起始值
#define FE_BPS_APPLY_TO_IN_INDEX_START                     (FE_LEVEL_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 自动增益通道索引起始值
#define AGC_APPLY_TO_IN_INDEX_START                        (FE_BPS_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 自动增益旁通通道索引起始值
#define AGC_BPS_APPLY_TO_IN_INDEX_START                    (AGC_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 输入模式通道索引起始值
#define INPUT_MODE_APPLY_TO_IN_INDEX_START                 (AGC_BPS_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 模拟输入控制通道索引起始值 
#define ANALOG_INPUT_APPLY_TO_IN_INDEX_START               (INPUT_MODE_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 延时通道索引起始值
#define DELAY_APPLY_TO_IN_AND_OUT_INDEX_START              (ANALOG_INPUT_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 压缩器通道索引起始值
#define COMP_APPLY_TO_OUT_INDEX_START                      (DELAY_APPLY_TO_IN_AND_OUT_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 压缩器旁通通道索引起始值
#define COMP_BPS_APPLY_TO_OUT_INDEX_START                  (COMP_APPLY_TO_OUT_INDEX_START + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 限幅器通道索引起始值
#define LIMIT_APPLY_TO_OUT_INDEX_START                     (COMP_BPS_APPLY_TO_OUT_INDEX_START + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 限幅器旁通通道索引起始值
#define LIMIT_BPS_APPLY_TO_OUT_INDEX_START                 (LIMIT_APPLY_TO_OUT_INDEX_START + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 混音掩码通道索引起始值
#define MIXER_MASK_APPLY_TO_OUT_INDEX_START                (LIMIT_BPS_APPLY_TO_OUT_INDEX_START + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 自动混音输出通道索引起始值
#define AUTO_MIX_OUT_APPLY_TO_OUT_INDEX_START              (MIXER_MASK_APPLY_TO_OUT_INDEX_START + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 回声消除输出通道索引起始值
#define AEC_OUT_APPLY_TO_OUT_INDEX_START                   (AUTO_MIX_OUT_APPLY_TO_OUT_INDEX_START + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 噪声消除输出通道索引起始值
#define ANS_OUT_APPLY_TO_OUT_INDEX_START                   (AEC_OUT_APPLY_TO_OUT_INDEX_START + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 自动混音输入设置通道索引起始值
#define AUTO_MIX_IN_SET_APPLY_TO_IN_INDEX_START            (ANS_OUT_APPLY_TO_OUT_INDEX_START + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 自动混音输出设置通道索引起始值
#define AUTO_MIX_OUT_SET_INDEX_START                       (AUTO_MIX_IN_SET_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 回声消除通道设置通道索引起始值
#define AEC_CHANNEL_SET_APPLY_TO_IN_INDEX_START            (AUTO_MIX_OUT_SET_APPLY_TO_IN_INDEX_START + 1)
#define AEC_CHANNEL_SET_APPLY_TO_AUTO_MIX_OUT_INDEX_START  (AEC_CHANNEL_SET_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 回声消除等级设置通道索引起始值
#define AEC_LEVEL_SET_INDEX_START                          (AEC_CHANNEL_SET_APPLY_TO_AUTO_MIX_OUT_INDEX_START + 1)
// 噪声消除通道设置通道索引起始值
#define ANS_CHANNEL_SET_APPLY_TO_IN_INDEX_START            (AEC_LEVEL_SET_INDEX_START + 1)
#define ANS_CHANNEL_SET_APPLY_TO_AUTO_MIX_OUT_INDEX_START  (ANS_CHANNEL_SET_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
#define ANS_CHANNEL_SET_APPLY_TO_AEC_OUT_INDEX_START       (ANS_CHANNEL_SET_APPLY_TO_AUTO_MIX_OUT_INDEX_START + 1)
// 噪声消除等级设置通道索引起始值
#define ANS_LEVEL_SET_INDEX_START                          (ANS_CHANNEL_SET_APPLY_TO_AEC_OUT_INDEX_START + 1)
// 正弦波通道索引起始值
#define SINE_WAVE_INDEX_START                              (ANS_LEVEL_SET_INDEX_START + 1)
// 粉红噪声通道索引起始值
#define PINK_NOISE_INDEX_START                             (SINE_WAVE_INDEX_START + 1)
// 白噪声通道索引起始值
#define WHITE_NOISE_INDEX_START                            (PINK_NOISE_INDEX_START + 1)
// 通道名称设置通道索引起始值
#define CHANNEL_NAME_SET_APPLY_TO_IN_AND_OUT_INDEX_START   (WHITE_NOISE_INDEX_START + 1)
// 语音跟踪功能设置通道索引起始值
#define CAM_FUN_SET_INDEX_START                            (CHANNEL_NAME_SET_APPLY_TO_IN_AND_OUT_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT + MAX_AUDIO_CHANNEL_OUT_COUNT)
// 语音跟踪通道(Mic)设置通道索引起始值
#define CAM_CHANNEL_SET_APPLY_TO_IN_INDEX_START            (CAM_FUN_SET_INDEX_START + 1)
#define CAM_CHANNEL_SET_APPLY_TO_VIRTUAL_INDEX_START       (CAM_CHANNEL_SET_APPLY_TO_IN_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT)
// 语音跟踪通讯串口(波特率)设置通道索引起始值
#define CAM_COM_BAUD_SET_APPLY_TO_RS232_INDEX_START        (CAM_CHANNEL_SET_APPLY_TO_VIRTUAL_INDEX_START + 4)
#define CAM_COM_BAUD_SET_APPLY_TO_RS485_INDEX_START        (CAM_COM_BAUD_SET_APPLY_TO_RS232_INDEX_START + 1)
// 存档保护设置通道索引起始值
#define PRESET_LOCK_SET_INDEX_START                        (CAM_COM_BAUD_SET_APPLY_TO_RS485_INDEX_START + 1)
// EQ通道全旁通通道索引起始值
#define CHANNEL_EQ_BYPASS_APPLY_TO_IN_AND_OUT_INDEX_START  (PRESET_LOCK_SET_INDEX_START + 1)
// 通道音量限制通道索引起始值
#define GAIN_LIMIT_APPLY_TO_IN_AND_OUT_INDEX_START         (CHANNEL_EQ_BYPASS_APPLY_TO_IN_AND_OUT_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT + MAX_AUDIO_CHANNEL_OUT_COUNT)
// AI噪声消除参数设置通道索引起始值
#define AI_ANS_SET_INDEX_START                             (GAIN_LIMIT_APPLY_TO_IN_AND_OUT_INDEX_START + MAX_AUDIO_CHANNEL_IN_COUNT + MAX_AUDIO_CHANNEL_OUT_COUNT)
// AI反馈抑制参数设置通道索引起始值
#define AI_AFC_SET_INDEX_START                             (AI_ANS_SET_INDEX_START + 1)
// 总音量通道索引起始值
#define GAIN_GLOBAL_OUT_INDEX_START                        (AI_AFC_SET_INDEX_START + 1)
// 电平读取通道索引起始值
#define UV_READ_APPLY_TO_IN_AND_OUT_INDEX_START            (GAIN_GLOBAL_OUT_INDEX_START + 1)
// 压限器增益读取通道索引起始值
#define CONTROL_GAIN_READ_APPLY_TO_OUT_INDEX_START         (UV_READ_APPLY_TO_IN_AND_OUT_INDEX_START + 1)


// 滤波器信息
struct AudioFilterInfo
{
    uint32_t presetCode_ = 0;   // 存档号
    uint32_t index_      = 1;   // 模块索引
    float    frequency_  = 10;  // 频率 HZ
    uint8_t  filterType_ = 0;   // 滤波器类型
    uint8_t  bypass_     = 1;   // 旁通：1开，0关
    uint8_t  reserve_[2] = {0}; // 保留
};
class AudioChannel
{
public:
    virtual ~AudioChannel() = default;

    virtual int32_t GetAudioChannelInIndex(int32_t channelId = 0)
    {
        if (channelId < MAX_AUDIO_CHANNEL_IN_COUNT)
        {
            return channelIn_[channelId];
        }
        return -1;
    }

    virtual int32_t GetAudioChannelOutIndex(int32_t channelId = 0)
    {
        if (channelId < MAX_AUDIO_CHANNEL_OUT_COUNT)
        {
            return channelOut_[channelId];
        }
        return -1;
    }

    std::array<uint32_t, MAX_AUDIO_CHANNEL_IN_COUNT> channelIn_;
    std::array<uint32_t, MAX_AUDIO_CHANNEL_OUT_COUNT> channelOut_;
};

// 设置增益请求消息
class AudioGainSetRequestMsg : public CommonMessage
                             , public AudioChannel
{
public:
    AudioGainSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = GAIN_APPLY_TO_IN_AND_OUT_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;
    struct AudioGainInfo
    {
        uint32_t presetCode_ = 0;  // 存档号
        uint32_t index_      = 1;  // 模块索引
        float    gain_       = 0;  // 增益
        uint8_t  mute_       = 0;  // 静音：1开，0关
        uint8_t  phase_      = 1;  // 相位：1正，0负
    };
    AudioGainInfo gainInfo_;
};

// 设置参量均衡请求消息


// 设置高通滤波请求消息
class AudioHighPassFilterSetRequestMsg : public CommonMessage
                                       , public AudioChannel
{
public:
    AudioHighPassFilterSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = HPF_APPLY_TO_IN_AND_OUT_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;
    
    AudioFilterInfo highPassFilterInfo_;
};

// 设置低通滤波请求消息
class AudioLowPassFilterSetRequestMsg : public CommonMessage
                                      , public AudioChannel
{
public:
    AudioLowPassFilterSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = LPF_APPLY_TO_IN_AND_OUT_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;
    
    AudioFilterInfo lowPassFilterInfo_;
};

// 设置噪声门请求消息
class AudioNoiseGateSetRequestMsg : public CommonMessage
                                  , public AudioChannel
{
public:
    AudioNoiseGateSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = NG_APPLY_TO_IN_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct AudioNoiseGateInfo
    {
        float    threshold_ = -120; // 阈值
        float    ratio_     = 100;  // 压缩比（协议保留，UI不显示）
        uint32_t holdMs_    = 0;    // 保持时间（单位ms，协议保留，UI不显示）
        uint32_t attackMs_  = 45;   // 启动时间（单位ms）
        uint32_t releaseMs_ = 724;  // 释放时间（单位ms）
        uint32_t knee_      = 0;    // 平滑度，拐点（协议保留，UI不显示）
    };
    
    AudioNoiseGateInfo audioNoiseGateInfo_;
};

// 设置噪声门旁通请求消息
class AudioNoiseGateBypassSetRequestMsg : public CommonMessage
                                  , public AudioChannel
{
public:
    AudioNoiseGateBypassSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = NG_BPS_APPLY_TO_IN_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t bypass_ = 1;  // 旁通：1开，0关
};

// 设置反馈抑制（Feedback Exterminator）等级请求消息
class AudioFELevelSetRequestMsg : public CommonMessage
                                , public AudioChannel
{
public:
    AudioFELevelSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = FE_LEVEL_APPLY_TO_IN_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t level_ = 2;  // 等级：1(3Hz)，2(6Hz)
};

// 设置反馈抑制（Feedback Exterminator）旁通请求消息
class AudioFEBypassSetRequestMsg : public CommonMessage
                                 , public AudioChannel
{
public:
    AudioFEBypassSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = FE_BPS_APPLY_TO_IN_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t bypass_ = 1;  // 旁通：1开，0关
};

// 设置自动增益请求消息
class AudioAGCSetRequestMsg : public CommonMessage
                            , public AudioChannel
{
public:
    AudioAGCSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = AGC_APPLY_TO_IN_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct AGCInfo
    {
        float    attackThreshold_ = 21;  // 启控阈值
        float    targetThreshold_ = 21;  // 目标阈值
        uint32_t ratio_           = 0;   // 比率
        uint32_t attackMs_        = 45;  // 启控时间（单位ms）
        uint32_t releaseMs_       = 724; // 释放时间（单位ms）
    };
    
    AGCInfo info_;
};

// 设置自动增益旁通请求消息
class AudioAGCBypassSetRequestMsg : public CommonMessage
                                  , public AudioChannel
{
public:
    AudioAGCBypassSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = AGC_BPS_APPLY_TO_IN_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t bypass_ = 1;  // 旁通：1开，0关
};

// 设置输入模式请求消息
class AudioInputModeSetRequestMsg : public CommonMessage
                                  , public AudioChannel
{
public:
    AudioInputModeSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = INPUT_MODE_APPLY_TO_IN_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t mode_;
};

// 设置模拟输入控制请求消息
class AudioAnalogInputSetRequestMsg : public CommonMessage
                                    , public AudioChannel
{
public:
    AudioAnalogInputSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = ANALOG_INPUT_APPLY_TO_IN_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct InputInfo
    {
        uint16_t inputSrc_         = 1;  // 输入源 1：line，0：mic
        uint16_t sensitivityLevel_ = 0;  // 灵敏度级别 [0，40]dB
        uint32_t switchOn48V_      = 0;  // 0 关, 1开
    };
    
    InputInfo inputInfo_;
};

// 设置延时请求消息
class AudioDelaySetRequestMsg : public CommonMessage
                              , public AudioChannel
{
public:
    AudioDelaySetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = DELAY_APPLY_TO_IN_AND_OUT_INDEX_START;
        for (auto& channelIn : channelIn_)
        {
            channelIn = i++;
        }
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct DelayInfo
    {
        float   timeMs_     = 0;   // 延时 ms
        uint8_t bypass_     = 0;   // 旁通：1开，0关
        uint8_t reserve_[3] = {0}; // 保留
    };
    
    DelayInfo delayInfo_;
};

// 设置压缩器请求消息
class AudioCompressorSetRequestMsg : public CommonMessage
                                   , public AudioChannel
{
public:
    AudioCompressorSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = COMP_APPLY_TO_OUT_INDEX_START;
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct CompressorInfo
    {
        float    threshold_ = 21;   // 阈值
        float    ratio_     = 1;    // 压缩比（协议保留，UI不显示）
        uint32_t holdMs_    = 0;    // 保持时间（单位ms，协议保留，UI不显示）
        uint32_t attackMs_  = 45;   // 启动时间（单位ms）
        uint32_t releaseMs_ = 724;  // 释放时间（单位ms）
        uint32_t knee_      = 0;    // 平滑度，拐点
    };
    
    CompressorInfo compressorInfo_;
};

// 设置压缩器请求消息
class AudioCompressorBypssSetRequestMsg : public CommonMessage
                                        , public AudioChannel
{
public:
    AudioCompressorBypssSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = COMP_BPS_APPLY_TO_OUT_INDEX_START;
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t bypass_; // 旁通：1开，0关
};

// 设置限幅器请求消息
class AudioLimiterSetRequestMsg : public CommonMessage
                                , public AudioChannel
{
public:
    AudioLimiterSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = LIMIT_APPLY_TO_OUT_INDEX_START;
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct LimiterrInfo
    {
        float    threshold_ = 21;   // 阈值
        float    ratio_     = 1;    // 压缩比（协议保留，UI不显示）
        uint32_t holdMs_    = 0;    // 保持时间（单位ms，协议保留，UI不显示）
        uint32_t releaseMs_ = 724;  // 释放时间（单位ms）
        uint32_t knee_      = 0;    // 平滑度，拐点（协议保留，UI不显示）
    };
    
    LimiterrInfo limiterInfo_;
};

// 设置压缩器旁通请求消息
class AudioLimiterBypassSetRequestMsg : public CommonMessage
                                      , public AudioChannel
{
public:
    AudioLimiterBypassSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = LIMIT_BPS_APPLY_TO_OUT_INDEX_START;
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint32_t bypass_; // 旁通：1开，0关
};

// 设置混音掩码请求消息
/*
    从低到高，每个bit对应一个输入通道是否静音，如bit0对应输入通道In1和输出的静音状态，0表示取消静音（接通），1表示静音（断开）。
    注意：输入只有38个通道，实际64bit只用到前38bit 
*/
class AudioMixerMaskSetRequestMsg : public CommonMessage
                                  , public AudioChannel
{
public:
    AudioMixerMaskSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = MIXER_MASK_APPLY_TO_OUT_INDEX_START;
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }

        std::fill(mark_, mark_ + 8, 0xFF);
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint8_t mark_[8]; // 0表示取消静音（接通），1表示静音（断开）
};

// 设置混音器自动混音输出通道请求消息
class AudioMixerAutoMixOutSetRequestMsg : public CommonMessage
                                        , public AudioChannel
{
public:
    AudioMixerAutoMixOutSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = AUTO_MIX_OUT_APPLY_TO_OUT_INDEX_START;
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct MixOutInfo
    {
        float   gain_       = 0;   // 增益
        uint8_t mute_       = 1;   // 静音：1开，0关
        uint8_t phase_      = 1;   // 相位：1正，0负
        uint8_t reserve_[2] = {0}; // 保留

    };
    MixOutInfo mixOutInfo_;
};

// 设置混音器AEC输出通道请求消息
class AudioMixerAecOutSetRequestMsg : public CommonMessage
                                    , public AudioChannel
{
public:
    AudioMixerAecOutSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = AEC_OUT_APPLY_TO_OUT_INDEX_START;
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct MixAecOutInfo
    {
        float   gain_       = 0;   // 增益
        uint8_t mute_       = 1;   // 静音：1开，0关
        uint8_t phase_      = 1;   // 相位：1正，0负
        uint8_t reserve_[2] = {0}; // 保留

    };
    MixAecOutInfo mixerAecOutInfo_;
};

// 设置混音器ANS输出通道请求消息
class AudioMixerAnsOutSetRequestMsg : public CommonMessage
                                    , public AudioChannel
{
public:
    AudioMixerAnsOutSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = ANS_OUT_APPLY_TO_OUT_INDEX_START;
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct MixerAnsOutInfo
    {
        float   gain_       = 0;   // 增益
        uint8_t mute_       = 1;   // 静音：1开，0关
        uint8_t phase_      = 1;   // 相位：1正，0负
        uint8_t reserve_[2] = {0}; // 保留

    };
    MixerAnsOutInfo mixerAnsOutInfo_;
};

// 设置自动混音输入通道请求消息
class AudioAutoMixInSetRequestMsg : public CommonMessage
                                  , public AudioChannel
{
public:
    AudioAutoMixInSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
        InitAudioChannelIndex();
    }
    void InitAudioChannelIndex()
    {
        uint32_t i = AUTO_MIX_IN_SET_APPLY_TO_IN_INDEX_START;
        for (auto& channelOut : channelOut_)
        {
            channelOut = i++;
        }
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct AutoMixInfo
    {
        uint8_t  auto_        = 0;   // 增益
        uint8_t  mute_        = 1;   // 静音：1开，0关
        uint16_t priority_    = 1;   // 相位：1正，0负
        float    channelGain_ = {0}; // 保留

    };
    AutoMixInfo autoMixInfo_;
};

// 设置自动混音输出通道请求消息
class AudioAutoMixOutSetRequestMsg : public CommonMessage
                                   , public AudioChannel
{
public:
    AudioAutoMixOutSetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_AUDIO_CONFIG_SET))
        : CommonMessage(functionCode)
    {
    }
    virtual int32_t GetAudioChannelOutIndex(int32_t channelId = 0) override
    {
        return AUTO_MIX_OUT_SET_INDEX_START;
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    struct AutoMixOutInfo
    {
        float    slope           = 1; // 斜率
        float    outGain_        = 0; // 输出增益
        uint32_t responseTimeMS_ = 5; // 响应时间
    };
    AutoMixOutInfo autoMixOutInfo_;
};
