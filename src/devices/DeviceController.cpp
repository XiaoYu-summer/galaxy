#include "devices/DeviceController.h"
#include "devices/KingrayController.h"
#include "devices/DigisynController.h"

DeviceController* DeviceController::CreateDeviceController(const DeviceNetworkInfo& info)
{
    switch (info.deviceVendor)
    {
        case DeviceVendor::KINGRAY:
            return new KingrayController(info);
        case DeviceVendor::DIGISYN:
            return new DigisynController(info);
        default:
            return nullptr;
    }
}

DeviceController::DeviceController(const DeviceNetworkInfo& info)
    : networkInfo_(info)
{

}
