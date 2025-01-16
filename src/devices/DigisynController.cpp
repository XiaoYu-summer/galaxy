#include "devices/DigisynController.h"

DigisynController::DigisynController(const DeviceNetworkInfo& info)
    : DeviceController(info)
{

}

std::string DigisynController::GetFunctionCode(const std::vector<uint8_t>& response) const
{
    return "";
}

std::string DigisynController::GetDeviceName(const std::string& deviceId) const
{
    return "";
}

DeviceAddress DigisynController::GetDeviceAddress(const std::string& deviceId) const
{
    return {};
}

DeviceVersion DigisynController::GetDeviceVersion(const std::string& deviceId) const
{
    return {};
}

bool DigisynController::GetDeviceOnlineStatus(const std::string& deviceId) const
{
    return false;
}
