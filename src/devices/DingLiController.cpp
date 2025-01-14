#include "devices/DingLiController.h"

DingLiController::DingLiController(const DeviceNetworkInfo& info)
    : DeviceController(info)
{

}

std::string DingLiController::GetFunctionCode(const std::vector<uint8_t>& response) const
{
    return "";
}

std::string DingLiController::GetDeviceName(const std::string& deviceId) const
{
    return "";
}

DeviceAddress DingLiController::GetDeviceAddress(const std::string& deviceId) const
{
    return {};
}

DeviceVersion DingLiController::GetDeviceVersion(const std::string& deviceId) const
{
    return {};
}

bool DingLiController::GetDeviceOnlineStatus(const std::string& deviceId) const
{
    return false;
}
