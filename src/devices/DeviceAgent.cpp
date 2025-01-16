#include "devices/DeviceController.h"
#include "devices/DeviceAgent.h"

DeviceAgent::DeviceAgent(const DeviceNetworkInfo& info, DeviceController* controller)
    : controller_(controller ? controller : DeviceController::CreateDeviceController(info))
{
    
}

DeviceInfo DeviceAgent::GetDeviceInfo(const std::string& deviceId) const
{
    DeviceInfo info;
    return info;
}

bool DeviceAgent::ResetDevice()
{
    return false;
}