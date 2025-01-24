#include "devices/DeviceManager.h"
#include "devices/Device.h"
#include "devices/DeviceParams.h"
#include "common/LoggerWrapper.h"

DEFINE_FILE_NAME("DeviceManager.cpp")

DeviceManager::DeviceManager()
    : logger_(Poco::Logger::get("DeviceManager"))
{
    LOG_I("construct device manager...");
}

void DeviceManager::Init()
{
    LOG_I("Init...");

   // 创建设备发现任务
   if (!kingrayDiscoveryProcessor_)
   {
        kingrayDiscoveryProcessor_.reset(new DeviceDiscoveryProcessor(DeviceVendor::KINGRAY));
   }
   if (kingrayDiscoveryProcessor_)
   {
        kingrayDiscoveryProcessor_->InitProcessor(std::dynamic_pointer_cast<DeviceDiscoveryObserver>(shared_from_this()));
   }
   if (!digisynDiscoveryProcessor_)
   {
        digisynDiscoveryProcessor_.reset(new DeviceDiscoveryProcessor(DeviceVendor::DIGISYN));
   }
   if (digisynDiscoveryProcessor_)
   {
        digisynDiscoveryProcessor_->InitProcessor(std::dynamic_pointer_cast<DeviceDiscoveryObserver>(shared_from_this()));
   }
}

std::shared_ptr<Device> DeviceManager::Get(const std::string& deviceId) const
{
    std::shared_ptr<Device> device = nullptr;
    {
        Poco::ScopedLock<Poco::FastMutex> lock(devicesMutex_);
        auto it = devices_.find(deviceId);
        if (it != devices_.end())
        {
            device = it->second;
        }
    }
    return device;
}

std::vector<std::shared_ptr<Device>> DeviceManager::DevicesFilter(const std::function<bool(const std::shared_ptr<Device>&)>& predicate) const
{
    std::vector<std::shared_ptr<Device>> devices;
    {
        Poco::ScopedLock<Poco::FastMutex> lock(devicesMutex_);
        for (const auto& item : devices_)
        {
            if (predicate(item.second))
            {
                devices.push_back(item.second);
            }
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
    LOG_INFO_THIS("add device deviceId=" << info.deviceId);
    Poco::ScopedLock<Poco::FastMutex> lock(devicesMutex_);
    if (!devices_.count(info.deviceId))
    {
        std::shared_ptr<Device> device = Device::CreateDevice(info);
        devices_[info.deviceId] = device;
    }
    else
    {
        LOG_DEBUG_THIS("device is exist! deviceId=" << info.deviceId << ", deviceType=" << (int)info.deviceType);
    }
}

void DeviceManager::DeleteDevice(const std::string& deviceId)
{
    LOG_INFO_THIS("delete device deviceId=" << deviceId);
    Poco::ScopedLock<Poco::FastMutex> lock(devicesMutex_);
    if (0 != devices_.count(deviceId))
    {
        devices_.erase(deviceId);
    }
}

void DeviceManager::OnUpdateDeviceStatus(const DeviceNetworkInfo& info, bool onLine)
{
    if (onLine)
    {
        AddDevice(info);
    }
    else
    {
        DeleteDevice(info.deviceId);
    }
}
