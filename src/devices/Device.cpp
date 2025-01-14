#include "devices/DeviceImpl.h"

std::shared_ptr<Device> Device::CreateDevice(const DeviceNetworkInfo& info, DeviceController* controller)
{
    return std::make_shared<DeviceImpl>(info, controller);
}
