#pragma once
#include <memory>
#include "types/App.h"
#include "devices/DeviceManager.h"

class DevicesController
{
public:
    DevicesController(std::shared_ptr<DeviceManager> deviceManager);
    ~DevicesController() = default;
    void InitRoutes(CrowApp& app);
private:

    std::weak_ptr<DeviceManager> deviceManager_;
};
