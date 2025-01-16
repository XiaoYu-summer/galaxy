#pragma once
#include <memory>
#include "types/App.h"

class DeviceManager;
class DevicesApiController
{
public:
    DevicesApiController();
    ~DevicesApiController() = default;
    static void InitRoutes(CrowApp& app);
private:
    static std::shared_ptr<DeviceManager> deviceManager_;
};
