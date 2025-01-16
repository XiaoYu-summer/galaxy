#pragma once
#include <string>
#include <memory>
#include "devices/Device.h"
#include "devices/DeviceController.h"

class DeviceAgent : public Device
{
public:
    DeviceAgent(const DeviceNetworkInfo& info, DeviceController* controller = nullptr);
    virtual ~DeviceAgent() = default;
    virtual DeviceInfo GetDeviceInfo(const std::string& deviceId) const override;
    virtual bool ResetDevice() override;
private:
    std::shared_ptr<DeviceController> controller_;
};
