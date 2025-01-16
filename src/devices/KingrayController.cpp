#include "devices/KingrayController.h"
#include "devices/KingrayControlMessage.h"

KingrayController::KingrayController(const DeviceNetworkInfo& info)
    : DeviceController(info)
      
{
    InitTransport();
}

void KingrayController::InitTransport()
{
    aoip::ProtocolConfig config;
    config.masterIp_ = networkInfo_.unicastIp;
    config.masterPort_ = networkInfo_.unicastPort;
    config.broadcast_ = false;
    transport_.reset(new aoip::AsyncProtocol(config));
    if (transport_)
    {
        transport_->SetUdpCallback(std::dynamic_pointer_cast<aoip::UdpCallback>(shared_from_this()));
        transport_->Start();
    }
}

std::string KingrayController::GetFunctionCode(const std::vector<uint8_t>& response) const
{
    Binary::Unpack unpack(response.data(), response.size());
    CommonMessage message;
    message.Deserialize(unpack);

    return GetFunctionCodeStr(message.messageHeader_.functionCode_);
}

std::string KingrayController::GetDeviceName(const std::string& deviceId) const
{
    Binary::Pack pack;
    DeviceNameGetRequestMsg request;
    request.messageHeader_.functionCode_ = (uint16_t)FunctionCode::PL_FUN_DEVICE_NAME_GET;
    const auto serializeResult = request.Serialize(pack);
    if (transport_ && serializeResult)
    {
        std::future<std::vector<uint8_t>> future = transport_->SendRequest(GetFunctionCodeStr(request.messageHeader_.functionCode_), pack.data(), pack.size());
        std::vector<uint8_t> response = future.get();
        Binary::Unpack unpack(response.data(), response.size());

        DeviceNameGetResponseMsg responseMsg;
        responseMsg.messageHeader_.functionCode_ = (uint16_t)FunctionCode::PL_FUN_DEVICE_NAME_GET;
        responseMsg.DeserializeBody(unpack);
        return responseMsg.name_;
        
    }
     return "";
}

DeviceAddress KingrayController::GetDeviceAddress(const std::string& deviceId) const
{
    return {};
}

DeviceVersion KingrayController::GetDeviceVersion(const std::string& deviceId) const
{
    return {};
}

bool KingrayController::GetDeviceOnlineStatus(const std::string& deviceId) const
{
    return false;
}
