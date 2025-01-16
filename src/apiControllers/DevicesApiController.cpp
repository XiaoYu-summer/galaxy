#include "apiControllers/DevicesApiController.h"


DevicesApiController::DevicesApiController(std::shared_ptr<DeviceManager> deviceManager)
    : deviceManager_(deviceManager)
{
}

void DevicesApiController::InitRoutes(CrowApp& app)
{

}

