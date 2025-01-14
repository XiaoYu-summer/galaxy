#pragma once
#include <string>
#include "devices/DeviceController.h"
#include "AsyncProtocol.h"


class DingLiController : public DeviceController
                       , public aoip::UdpCallback
                       , public std::enable_shared_from_this<DingLiController>
{
public:
    DingLiController(const DeviceNetworkInfo& info);
    virtual ~DingLiController() = default;

    virtual std::string GetFunctionCode(const std::vector<uint8_t>& response) const override;
    virtual std::string GetDeviceName(const std::string& deviceId) const override;
    virtual DeviceAddress GetDeviceAddress(const std::string& deviceId) const override;
    virtual DeviceVersion GetDeviceVersion(const std::string& deviceId) const override;
    virtual bool GetDeviceOnlineStatus(const std::string& deviceId) const override;
};
