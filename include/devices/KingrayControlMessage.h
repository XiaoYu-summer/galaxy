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
    PL_FUN_DEVICE_NAME_GET                 = 0x020B,    // 设备名称获取
    PL_FUN_UNKNOW
};

const std::map<std::string, FunctionCode> FUNCTION_CODE_MAP =
{
    std::pair<std::string, FunctionCode>("PL_FUN_SLAVE_RESPONSE",                  FunctionCode::PL_FUN_SLAVE_RESPONSE),
    std::pair<std::string, FunctionCode>("PL_FUN_NETINFO_GET",                     FunctionCode::PL_FUN_NETINFO_GET),
    std::pair<std::string, FunctionCode>("PL_FUN_NETINFO_SET",                     FunctionCode::PL_FUN_NETINFO_SET),
    std::pair<std::string, FunctionCode>("PL_FUN_DEVICE_MARK",                     FunctionCode::PL_FUN_DEVICE_MARK),
    std::pair<std::string, FunctionCode>("PL_FUN_DEVICE_NAME_GET",                 FunctionCode::PL_FUN_DEVICE_NAME_GET),
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
    std::pair<std::string, FunctionCode>("PL_FUN_AES_MDL_INFO_GET",               FunctionCode::PL_FUN_AES_MDL_INFO_GET),              
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
    std::pair<std::string, FunctionCode>("PL_FUN_DEVICE_NAME_GET",                 FunctionCode::PL_FUN_DEVICE_NAME_GET)                
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

    uint32_t    savePresetCode_  = 0;  // 保存存档号
    std::string presetName_;           // 存档名称（不超过20字节）
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

    uint8_t  action_     = 0;   // 0：停止， 1：开始
    uint8_t  deviceType_ = 0;   // 设备类型
    uint16_t deviceCode_ = 0;   // 设备编码
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

// 获取群组编码响应消息
class GroupCodeGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& pack) override;

    uint8_t  groupCode_[2] = {0};  // 群组编码
    uint16_t reserve_      = 0;    // 保留
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

    uint8_t  groupCode_[2] = {0};  // 群组编码
    uint16_t reserve_      = 0;    // 保留
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

// 获取会议参数响应消息
class MeetingParamGetResponsetMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& pack) override;

    uint8_t meetingMode_    = 0;  // 会议模式
    uint8_t wlMicSpeechMax_ = 0;  // 无线麦克最大发言数
    uint8_t wdMicSpeechMax_ = 0;  // 有线麦克最大发言数
    uint8_t reserve_        = 0;  // 保留
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

    uint8_t meetingMode_    = 0;  // 会议模式
    uint8_t wlMicSpeechMax_ = 0;  // 无线麦克最大发言数
    uint8_t wdMicSpeechMax_ = 0;  // 有线麦克最大发言数
    uint8_t reserve_        = 0;  // 保留
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

    uint8_t action_     = 0;    // 1: 发起对频，0：停止对频
    uint8_t reserve_[3] = {0};  // 保留
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

// 获取配对模式响应消息
class PairModeGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& pack) override;

    uint8_t  deviceType_ = 0;  // 设备类型
    uint8_t  pairMode_   = 0;  // 0：自动配对，1：手动配对
    uint16_t reserve_    = 0;  // 保留
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

    uint8_t  deviceType_ = 0;  // 设备类型
    uint8_t  pairMode_   = 0;  // 0：自动配对，1：手动配对
    uint16_t reserve_    = 0;  // 保留
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

    uint8_t  deviceType_ = 0;  // 设备类型
    uint8_t  action_     = 0;  // 1：发起配对，0：停止配对
    uint16_t reserve_    = 0;  // 保留
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

    uint8_t  deviceType_ = 0;  // 设备类型
    uint8_t  reserve_    = 0;  // 保留
    uint16_t deviceCode_ = 0;  // 设备编码
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

    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_[3] = {0};  // 保留
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

    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_[3] = {0};  // 保留
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

    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_[3] = {0};  // 保留
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

    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_[3] = {0};  // 保留
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

    uint8_t  deviceType_ = 0;    // 设备类型
    uint8_t  reserve_    = 0;    // 保留
    uint8_t  mac_[6]     = {0};  // mac地址
    uint16_t code_       = 0;    // 设备编码
};

// 无线主机名称获取请求消息
class WlHostNameGetRequestMsg : public CommonMessage
{
public:
    WlHostNameGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_WL_HOST_NAME_GET))
        : CommonMessage(functionCode)
    {
    }
};

// 无线主机名称获取响应消息
class WlHostNameGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    std::string name_;  // 无线主机名称（不超过64字节）
};

// MIC身份类别获取请求消息
class MicIdTypeGetRequestMsg : public CommonMessage
{
public:
    MicIdTypeGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_ALL_MIC_ID_TYPE_GET))
        : CommonMessage(functionCode)
    {
    }
    virtual void SerializeBody(Binary::Pack& pack) override;

    uint8_t deviceType_ = 0;    // 设备类型
    uint8_t reserve_[3] = {0};  // 保留
};

// MIC身份类别获取响应消息
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

// 设备名称获取请求消息
class DeviceNameGetRequestMsg : public CommonMessage
{
public:
    DeviceNameGetRequestMsg(uint16_t functionCode = static_cast<uint16_t>(FunctionCode::PL_FUN_DEVICE_NAME_GET))
        : CommonMessage(functionCode)
    {
    }
};

// 设备名称获取响应消息
class DeviceNameGetResponseMsg : public CommonMessage
{
public:
    virtual void DeserializeBody(const Binary::Unpack& unpack) override;

    std::string name_;
};
