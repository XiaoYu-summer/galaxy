#pragma once
#include <string>
#include <memory>
#include "devices/Device.h"
#include "devices/DeviceController.h"

class DeviceImpl : public Device
{
public:
    DeviceImpl(const DeviceNetworkInfo& info, DeviceController* controller = nullptr);
    virtual ~DeviceImpl() = default;
    virtual DeviceInfo GetDeviceInfo(const std::string& deviceId) const override;
private:
    std::shared_ptr<DeviceController> controller_;
};
