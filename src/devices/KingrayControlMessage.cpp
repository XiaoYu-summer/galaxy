#include "common/LoggerWrapper.h"
#include "devices/KingrayControlMessage.h"

DEFINE_FILE_NAME("KingrayControlMessage.cpp")

CommonMessage::CommonMessage(uint16_t functionCode)
    : messageHeader_({PROTOCOL_HEADER, 0, 0, functionCode})
    , logger_(Poco::Logger::get("KingrayControlMessage"))
{

}

bool CommonMessage::Serialize(Binary::Pack& pack)
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
        LOG_ERROR_THIS("pack serialize fail! " << e.what());
        return false;
    }
    return true;
}

bool CommonMessage::Deserialize(const Binary::Unpack& unpack)
{
    try
    {
        // 消息头
        DeserializeHeader(unpack);
        // 消息体
        DeserializeBody(unpack);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR_THIS("pack deserialize error! " << e.what());
        return false;
    }
    return true;
}

void CommonMessage::SerializeHeader(Binary::Pack& pack)
{
    pack << messageHeader_.frameHeader_ << messageHeader_.productID_ << messageHeader_.deviceID_
         << messageHeader_.functionCode_;
    headerSize_ = pack.size();
}

void CommonMessage::DeserializeHeader(const Binary::Unpack& unpack)
{
    const auto totalSize = unpack.size();
    unpack >> messageHeader_.frameHeader_ >> messageHeader_.productID_ >> messageHeader_.deviceID_ >>
        messageHeader_.functionCode_;
    headerSize_ = totalSize - unpack.size();
}

void McuNetInfoGetResponseMsg::DeserializeBody(const Binary::Unpack& unpack)
{
    uint32_t checksum = 0;
    uint32_t dataLen = 0;
    unpack >> dataLen;
    const auto sum = CalculateChecksum(dataLen, reinterpret_cast<const uint32_t*>(unpack.data()));
    Binary::ReadArray(unpack, netInfo_.mac_, sizeof(netInfo_.mac_));
    Binary::ReadArray(unpack, netInfo_.ip_, sizeof(netInfo_.ip_));
    Binary::ReadArray(unpack, netInfo_.mask_, sizeof(netInfo_.mask_));
    Binary::ReadArray(unpack, netInfo_.gw_, sizeof(netInfo_.gw_));
    unpack >> netInfo_.dhcpMode_ >> netInfo_.reserve_  >> checksum;
    // 验证检验和
    VerifyChecksum(sum, checksum);
}

void McuNetInfoSetRequestMsg::SerializeBody(Binary::Pack& pack)
{
    // 消息体大小
    const uint32_t dataLen = sizeof(netInfo_) / sizeof(uint32_t);
    pack << dataLen;
    const auto bodySize = pack.size();
    // 消息体
    WriteArray(pack, netInfo_.mac_, sizeof(netInfo_.mac_));
    WriteArray(pack, netInfo_.ip_, sizeof(netInfo_.ip_));
    WriteArray(pack, netInfo_.mask_, sizeof(netInfo_.mask_));
    WriteArray(pack, netInfo_.gw_, sizeof(netInfo_.gw_));
    pack << netInfo_.dhcpMode_ << netInfo_.reserve_;
    // 计算校验和
    pack << CalculateChecksum(dataLen, reinterpret_cast<const uint32_t*>(pack.data() + bodySize));
}

void DeviceMarkRequestMsg::SerializeBody(Binary::Pack& pack)
{
    // 消息体大小，这里以 action_、deviceType_ 和 deviceCode_ 的总字节数作为数据长度
    const uint32_t dataLen = sizeof(action_) + sizeof(deviceType_) + sizeof(deviceCode_);
    pack << dataLen;
    const auto bodySize = pack.size();
    // 消息体
    pack << action_ << deviceType_ << deviceCode_;
    // 计算校验和
    pack << CalculateChecksum(dataLen, reinterpret_cast<const uint32_t*>(pack.data() + bodySize));
}

void MicIdTypeGetRequestMsg::SerializeBody(Binary::Pack& pack)
{
    // 消息体大小，这里以 deviceType_ 和 reserve_ 数组的总字节数作为数据长度
    const uint32_t dataLen = sizeof(deviceType_) + sizeof(reserve_);
    pack << dataLen;
    const auto bodySize = pack.size();

    // 消息体
    pack << deviceType_;
    for (int i = 0; i < sizeof(reserve_) / sizeof(reserve_[0]); ++i) {
        pack << reserve_[i];
    }

    // 计算校验和
    pack << CalculateChecksum(dataLen, reinterpret_cast<const uint32_t*>(pack.data() + bodySize));
}

void MicIdTypeGetResponseMsg::DeserializeBody(const Binary::Unpack& unpack)
{
    uint32_t checksum = 0;
    uint32_t dataLen = 0;

    // 读取数据长度
    unpack >> dataLen;

    // 获取数据指针
    const uint32_t* dataPtr = reinterpret_cast<const uint32_t*>(unpack.data());

    // 计算校验和
    uint32_t sum = CalculateChecksum(dataLen, dataPtr);

    // 读取 deviceType_
    unpack >> deviceType_;
    deviceType_*=4;

    const uint16_t idTypeInfoCount = (dataLen - sizeof(uint8_t) - sizeof(deviceType_)) / sizeof(IdTypeInfo);

    idTypeInfoVec_.clear();

    // 读取每个 IdTypeInfo 结构体
    for (uint16_t i = 0; i < idTypeInfoCount; ++i) {
        IdTypeInfo idTypeInfo;
        unpack >> idTypeInfo.deviceCode_;
        unpack >> idTypeInfo.idType_;
        idTypeInfoVec_.push_back(idTypeInfo);
    }

    // 读取校验和
    unpack >> checksum;

    // 验证检验和
    VerifyChecksum(sum, checksum);
}

void DeviceNameGetResponseMsg::DeserializeBody(const Binary::Unpack& unpack)
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
