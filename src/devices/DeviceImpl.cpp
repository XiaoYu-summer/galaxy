#include "devices/DeviceController.h"
#include "devices/DeviceImpl.h"

DeviceImpl::DeviceImpl(const DeviceNetworkInfo& info, DeviceController* controller)
    : controller_(controller ? controller : DeviceController::CreateDeviceController(info))
{
    
}

DeviceInfo DeviceImpl::GetDeviceInfo(const std::string& deviceId) const
{
    DeviceInfo info;
    return info;
}
