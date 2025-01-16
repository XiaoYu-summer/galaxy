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

std::shared_ptr<Device> DeviceManager::get(const std::string& deviceId)
{
    std::shared_ptr<Device> device = nullptr;
    auto it = devices_.find(deviceId);
    if (it != devices_.end())
    {
        device = it->second;
    }
    return device;
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
