#include "devices/DeviceManager.h"
#include "devices/Device.h"
#include "devices/DeviceParams.h"
#include "common/LoggerWrapper.h"

DEFINE_FILE_NAME("DeviceManager.cpp")

DeviceManager::DeviceManager()
    : logger_(Poco::Logger::get("DeviceManager"))
{
    Init();
}

void DeviceManager::Init()
{
   // 创建设备发现任务
}

std::shared_ptr<Device> DeviceManager::Get(const std::string& deviceId) const
{
    std::shared_ptr<Device> device = nullptr;
    auto it = devices_.find(deviceId);
    if (it != devices_.end())
    {
        device = it->second;
    }
    return device;
}

std::vector<std::shared_ptr<Device>> DeviceManager::DevicesFilter(const std::function<bool(const std::shared_ptr<Device>&)>& predicate) const
{
    std::vector<std::shared_ptr<Device>> devices;
    for (const auto& item : devices_)
    {
        if (predicate(item.second))
        {
            devices.push_back(item.second);
        }
    }
    return devices;
}

std::vector<std::shared_ptr<Device>> DeviceManager::GetConnectingDevices() const
{
    return DevicesFilter([](const auto& device){ return device->IsConnecting(); });
}

std::vector<std::shared_ptr<Device>> DeviceManager::GetActiveMicrophoneDevices() const
{
    return DevicesFilter([](const auto& device){ return false; });
}

void DeviceManager::AddDevice(const DeviceNetworkInfo& info)
{
    if (!devices_.count(info.deviceId))
    {
        std::shared_ptr<Device> device = Device::CreateDevice(info);
        devices_[info.deviceId] = device;
    }
    else
    {
        LOG_WARNING_THIS("device is exist! deviceId=" << info.deviceId << ", deviceType=" << (int)info.deviceType);
    }
}

void DeviceManager::DelDevice(const std::string& deviceId)
{

}

void DeviceManager::OnUpdateDeviceStatus(const DeviceNetworkInfo& info, bool onLine)
{

}
