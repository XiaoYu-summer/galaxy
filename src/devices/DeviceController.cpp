#include "devices/DeviceController.h"
#include "devices/JingRuiController.h"
#include "devices/DingLiController.h"
#include "Logger.h"

DeviceController* DeviceController::CreateDeviceController(const DeviceNetworkInfo& info)
{
    switch (info.deviceVendor)
    {
        case DeviceVendor::JING_RUI:
            return new JingRuiController(info);
        case DeviceVendor::DING_LI:
            return new DingLiController(info);
        default:
            return nullptr;
    }
}

DeviceController::DeviceController(const DeviceNetworkInfo& info)
    : networkInfo_(info)
{

}
