#include "devices/DeviceController.h"
#include "devices/DeviceAgent.h"

DeviceAgent::DeviceAgent(const DeviceNetworkInfo& info, DeviceController* controller)
    : controller_(controller ? controller : DeviceController::CreateDeviceController(info))
{
    
}

bool DeviceAgent::GetInfo(DeviceInfo& info) const
{
    return false;
}

bool DeviceAgent::CheckSpeaker()
{
    return false;
}

bool DeviceAgent::Reset()
{
    return false;
}

bool DeviceAgent::SetLock(bool lock)
{
    return false;
}

bool DeviceAgent::SetVolume(const uint16_t volume)
{
    return false;   
}

bool DeviceAgent::Flashing()
{
    return false;
}

bool DeviceAgent::Disconnect()
{
    return false;
}

bool DeviceAgent::IsConnecting()
{
    return false;
}

bool DeviceAgent::SetMute(bool mute)
{
    return false;
}