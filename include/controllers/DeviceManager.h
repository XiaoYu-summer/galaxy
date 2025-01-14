#pragma once
#include <unordered_map>
#include "types/App.h"

class Device;
struct DeviceNetworkInfo;

class DeviceManager
{
public:
    static DeviceManager& GetInstance()
    {
		static DeviceManager instance;
		return instance;
	}
    DeviceManager();
    ~DeviceManager() = default;
    void InitRoutes(CrowApp& app);
    void AddDevice(const DeviceNetworkInfo& info);
    void DelDevice(const std::string& deviceId);
private:
    void Init();

    std::unordered_map<std::string, std::shared_ptr<Device>> devices_; // <DeviceId, devicePtr>
};
