#include "devices/DeviceController.h"
#include "devices/Device.h"
#include "devices/DeviceParams.h"

Device::Device(const DeviceNetworkInfo& info)
    : controller_(DeviceController::CreateDeviceController(info))
{}

Device::Device(const std::shared_ptr<Device>&device)
    : controller_(device->controller_)
{
}

std::shared_ptr<Device> Device::CreateDevice(const DeviceNetworkInfo& info)
{
    std::shared_ptr<Device> device;
    device.reset(new Device(info));
    return device;
}

bool Device::GetInfo(DeviceInfo& info) const
{
    return false;
}

std::string Device::GetId() const
{
    DeviceInfo info_;
    if (!GetInfo(info_)){
        return "";
    }
    return info_.id;
}

bool Device::CheckSpeaker()
{
    return false;
}

bool Device::SetLock(bool lock)
{
    return false;
}

bool Device::SetVolume(const uint16_t volume)
{
    return false;   
}

bool Device::Flashing()
{
    return false;
}

bool Device::Disconnect()
{
    return false;
}

bool Device::IsConnecting()
{
    return false;
}

bool Device::SetMute(bool mute)
{
    return false;
}