#include "controllers/DevicesController.h"


DevicesController::DevicesController(std::shared_ptr<DeviceManager> deviceManager)
    : deviceManager_(deviceManager)
{
}

void DevicesController::InitRoutes(CrowApp& app)
{

}

