#pragma once
#include <unordered_map>
#include <vector>
#include "Poco/Logger.h"
#include "Poco/Mutex.h"
#include "devices/DeviceDiscoveryProcessor.h"

class Device;
struct DeviceNetworkInfo;

class DeviceManager : public DeviceDiscoveryObserver
                    , public std::enable_shared_from_this<DeviceManager>
{
public:
    DeviceManager();
    ~DeviceManager() = default;
    void Init();
    void AddDevice(const DeviceNetworkInfo& info);
    void DeleteDevice(const std::string& deviceId);
    std::shared_ptr<Device> Get(const std::string& deviceId) const;

    // 获取连接中的设备
    std::vector<std::shared_ptr<Device>> GetConnectingDevices() const;

    // 获取活跃的麦克风设备
    std::vector<std::shared_ptr<Device>> GetActiveMicrophoneDevices() const;

    virtual void OnUpdateDeviceStatus(const DeviceNetworkInfo& info, bool onLine) override;
private:

    // 过滤符合条件的设备
    std::vector<std::shared_ptr<Device>> DevicesFilter(const std::function<bool(const std::shared_ptr<Device>&)>& predicate) const;

    std::shared_ptr<DeviceDiscoveryProcessor> kingrayDiscoveryProcessor_;
    std::shared_ptr<DeviceDiscoveryProcessor> digisynDiscoveryProcessor_;

    mutable Poco::FastMutex devicesMutex_;
    std::unordered_map<std::string, std::shared_ptr<Device>> devices_; // <DeviceId, devicePtr>

    Poco::Logger& logger_;
};
