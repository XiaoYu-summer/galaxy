#pragma once
#include <memory>
#include <string>
#include "devices/DeviceParams.h"

class DeviceController
{
public:
    static DeviceController* CreateDeviceController(const DeviceNetworkInfo& info);

    DeviceController(const DeviceNetworkInfo& info);
    virtual ~DeviceController() = default;

    virtual std::string GetDeviceName(const std::string& deviceId) const = 0;
    virtual DeviceAddress GetDeviceAddress(const std::string& deviceId) const = 0;
    virtual DeviceVersion GetDeviceVersion(const std::string& deviceId) const = 0;
    virtual bool GetDeviceOnlineStatus(const std::string& deviceId) const = 0;

protected:
    DeviceNetworkInfo networkInfo_;
};
