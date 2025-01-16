#pragma once
#include <string>
#include "devices/DeviceController.h"
#include "AsyncProtocol.h"


class DigisynController : public DeviceController
                       , public aoip::UdpCallback
                       , public std::enable_shared_from_this<DigisynController>
{
public:
    DigisynController(const DeviceNetworkInfo& info);
    virtual ~DigisynController() = default;

    virtual std::string GetFunctionCode(const std::vector<uint8_t>& response) const override;
    virtual std::string GetDeviceName(const std::string& deviceId) const override;
    virtual DeviceAddress GetDeviceAddress(const std::string& deviceId) const override;
    virtual DeviceVersion GetDeviceVersion(const std::string& deviceId) const override;
    virtual bool GetDeviceOnlineStatus(const std::string& deviceId) const override;
};
