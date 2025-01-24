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
