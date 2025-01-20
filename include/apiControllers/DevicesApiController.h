#pragma once
#include <memory>
#include "types/App.h"

class Device;
class DeviceManager;
class DevicesApiController
{
public:
    DevicesApiController();
    ~DevicesApiController() = default;
    static void InitRoutes(CrowApp& app);
    /**
     * Retrieves a device by its ID.
     * @param deviceId The device's identifier.
     * @return A shared pointer to the Device object.
     */
    static std::shared_ptr<Device> GetDevice(const std::string& deviceId);
private:
    static DeviceManager deviceManager_;
};
