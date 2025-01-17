#pragma once
#include <unordered_map>
#include "Poco/Logger.h"
#include "devices/DeviceDiscoveryProcessor.h"

class Device;
struct DeviceNetworkInfo;

class DeviceManager : public DeviceDiscoveryObserver
                    , public std::enable_shared_from_this<DeviceManager>
{
public:
    DeviceManager();
    ~DeviceManager() = default;
    void AddDevice(const DeviceNetworkInfo& info);
    void DelDevice(const std::string& deviceId);
    std::shared_ptr<Device> Get(const std::string& deviceId) const;

    virtual void OnUpdateDeviceStatus(const DeviceNetworkInfo& info, bool onLine) override;
private:
    void Init();
    std::shared_ptr<DeviceDiscoveryProcessor> kingrayDiscoveryProcessor_;
    std::shared_ptr<DeviceDiscoveryProcessor> digisynDiscoveryProcessor_;

    std::unordered_map<std::string, std::shared_ptr<Device>> devices_; // <DeviceId, devicePtr>

    Poco::Logger& logger_;
};
