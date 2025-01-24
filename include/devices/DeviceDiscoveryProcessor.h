#pragma once
#include <memory>
#include <Poco/Util/Timer.h>
#include <Poco/Util/TimerTask.h>
#include <Poco/AutoPtr.h>
#include "devices/DeviceParams.h"
#include "SerialProtocol.h"
#include "common/LoggerWrapper.h"

class DeviceDiscoveryObserver
{
public:
    virtual ~DeviceDiscoveryObserver() = default;
    virtual void OnUpdateDeviceStatus(const DeviceNetworkInfo& info, bool onLine) = 0;
};



class DeviceDiscoveryProcessor : public aoip::ResponseCallback
                               , public std::enable_shared_from_this<DeviceDiscoveryProcessor>
{
public:
    DeviceDiscoveryProcessor(const DeviceVendor deviceVendor);
    ~DeviceDiscoveryProcessor();
    virtual void OnRecvResponse(const std::vector<uint8_t>& data) override;

    void InitProcessor(std::shared_ptr<DeviceDiscoveryObserver> ob);
    void DeviceDiscoveryRequest();
private:
    

    void CreateSerialConnection();

    DeviceVendor deviceVendor_;
    std::weak_ptr<DeviceDiscoveryObserver> discoverOb_;

    std::unique_ptr<aoip::SerialTask> serialConnection_;
    std::shared_ptr<Poco::Util::Timer> _requestTimer;
    std::shared_ptr<Poco::Util::TimerTask> _timerTask;

    Poco::Logger& logger_;
};

class DeviceDiscoveryTask : public Poco::Util::TimerTask
{
public:
    DeviceDiscoveryTask(DeviceDiscoveryProcessor& processor) : processor_(processor)
    {
        LOG_I("construct device discovery task...");
    }

    void run() override
    {
        processor_.DeviceDiscoveryRequest();
    }

private:
    DeviceDiscoveryProcessor& processor_;
};