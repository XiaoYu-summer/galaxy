#pragma once
#include <string>
#include <memory>
#include "devices/DeviceParams.h"


class DeviceController;

class Device
{
public:
    static std::shared_ptr<Device> CreateDevice(const DeviceNetworkInfo& info, DeviceController* controller = nullptr);

    Device() = default;
    virtual ~Device() = default;
    // 获取设备信息
    virtual DeviceInfo GetDeviceInfo(const std::string& deviceId) const = 0;
    // 重置设备
    virtual bool ResetDevice() = 0;
};
