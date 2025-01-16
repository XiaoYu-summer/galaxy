#pragma once
#include <memory>
#include "types/App.h"
#include "devices/DeviceManager.h"

class DevicesApiController
{
public:
    DevicesApiController(std::shared_ptr<DeviceManager> deviceManager);
    ~DevicesApiController() = default;
    void InitRoutes(CrowApp& app);
private:

    std::weak_ptr<DeviceManager> deviceManager_;
};
