#pragma once
#include <memory>
#include "devices/DeviceParams.h"

class DeviceDiscoveryObserver
{
public:
    virtual ~DeviceDiscoveryObserver() = default;
    virtual void OnUpdateDeviceStatus(const DeviceNetworkInfo& info, bool onLine) = 0;
};

class DeviceDiscoveryProcessor
{
public:
    DeviceDiscoveryProcessor(const DeviceVendor deviceVendor);
    ~DeviceDiscoveryProcessor() = default;
    void InitProcessor(std::shared_ptr<DeviceDiscoveryObserver> ob);
private:
    DeviceVendor deviceVendor_;
    std::weak_ptr<DeviceDiscoveryObserver> discoverOb_;
};
