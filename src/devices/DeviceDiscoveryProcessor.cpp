#include <Poco/NumberParser.h>
#include <Poco/Environment.h>
#include "devices/DeviceDiscoveryProcessor.h"
#include "SerialProtocol.h"
#include "devices/KingrayControlMessage.h"

using namespace StringUtils;

const auto SERIAL_PORT = Poco::Environment::get("SERIAL_PORT", "/dev/tty0");
const int32_t BAUD_RATE = Poco::NumberParser::parse(Poco::Environment::get("BAUD_RATE", "115200"));
const int32_t SERIAL_READ_TIMEOUT_S = Poco::NumberParser::parse(Poco::Environment::get("SERIAL_READ_TIMEOUT_S", "5"));
const int32_t DEVICE_DISCOVERY_TIME_INTERVAL_MS = Poco::NumberParser::parse(Poco::Environment::get("DEVICE_DISCOVERY_TIME_INTERVAL_S", "3000"));

DEFINE_FILE_NAME("DeviceDiscoveryProcessor.cpp")

DeviceDiscoveryProcessor::DeviceDiscoveryProcessor(const DeviceVendor deviceVendor)
    : deviceVendor_(deviceVendor)
    , _requestTimer(std::make_shared<Poco::Util::Timer>())
    , logger_(Poco::Logger::get("DeviceDiscoveryProcessor"))
{
    LOG_INFO_THIS("constuct device discory processor vendor=" << (int)deviceVendor_);

    _timerTask = std::make_shared<DeviceDiscoveryTask>(*this);
    _requestTimer->scheduleAtFixedRate(_timerTask.get(), 0, DEVICE_DISCOVERY_TIME_INTERVAL_MS);
}

DeviceDiscoveryProcessor::~DeviceDiscoveryProcessor()
{
    if (_requestTimer)
    {
        _requestTimer->cancel();
    }
}

void DeviceDiscoveryProcessor::OnRecvResponse(const std::vector<uint8_t>& data)
{
    const auto functionCode = GetFunctionCodeByData(data);
    LOG_INFO_THIS("recv response function code=" << functionCode);
    if (FunctionCode::PL_FUN_NETINFO_GET == FunctionCode(functionCode))
    {
        McuNetInfoGetResponseMsg msg;
        msg.DeserializeBody(Binary::Unpack(data.data(), data.size()));
        LOG_DEBUG_THIS("mac=" << MacToString(msg.netInfo_.mac_) << ", ip=" << IpToString(msg.netInfo_.ip_) << ", mask=" << IpToString(msg.netInfo_.mask_) << ", gw=" << IpToString(msg.netInfo_.gw_));
        if (discoverOb_.lock())
        {
            DeviceNetworkInfo networkInfo;
            networkInfo.deviceType   = DeviceType::PAT71;
            networkInfo.deviceVendor = DeviceVendor::KINGRAY;
            networkInfo.deviceId     = 201;
            networkInfo.unicastIp    = IpToString(msg.netInfo_.ip_);
            networkInfo.unicastPort  = 50000;

            discoverOb_.lock()->OnUpdateDeviceStatus(networkInfo, true);
        }
    }
}

void DeviceDiscoveryProcessor::InitProcessor(std::shared_ptr<DeviceDiscoveryObserver> ob)
{
    LOG_INFO_THIS("init processor=" << (int)deviceVendor_);
    discoverOb_ = ob;
}

void DeviceDiscoveryProcessor::DeviceDiscoveryRequest()
{
    if (DeviceVendor::KINGRAY == deviceVendor_)
    {
        // 若创建串口连接失败，定时尝试
        if (!serialConnection_)
        {
            CreateSerialConnection();
        }
        // 创建成功，向串口发送请求
        if (serialConnection_)
        {
            Binary::Pack pack;
            McuNetInfoGetRequestMsg request;
            request.messageHeader_.functionCode_ = (uint16_t)FunctionCode::PL_FUN_NETINFO_GET;
            const auto serializeResult = request.Serialize(pack);
            if (serializeResult)
            {
                LOG_INFO_THIS("send get mcu network info request");
                serialConnection_->Write(pack.data(), pack.size());
            }
        }
    }
}

void DeviceDiscoveryProcessor::CreateSerialConnection()
{
    LOG_INFO_THIS("Create Serial Connection " << (int)deviceVendor_);
    try
    {
        if (!serialConnection_)
        {
            serialConnection_.reset(new aoip::SerialTask(std::dynamic_pointer_cast<ResponseCallback>(shared_from_this()), SERIAL_PORT, BAUD_RATE, SERIAL_READ_TIMEOUT_S));
        }
        if (serialConnection_)
        {
            serialConnection_->Start();
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR_THIS("create serial connection fail! reason=" << e.what());
    }
}
