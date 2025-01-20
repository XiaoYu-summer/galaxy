#include "devices/DeviceManager.h"
#include "devices/Device.h"
#include "apiControllers/DevicesApiController.h"
#include "utils/ResUtils.h"
#include "DevicesApiParamsParseHelper.h"

DeviceManager DevicesApiController::deviceManager_;

DevicesApiController::DevicesApiController()
{
}

static crow::json::wvalue DeviceInfoToJson(const DeviceInfo& info) {
    crow::json::wvalue json;
    return json;
}

void DevicesApiController::InitRoutes(CrowApp& crowApp)
{
    CROW_ROUTE(crowApp, "/device/api/v1/info")
        .methods("GET"_method)([] (const crow::request& request, crow::response& response){ 
            HandleDeviceGetReq(request, response, [](const std::shared_ptr<Device>& device, crow::response& response) {
                DeviceInfo info;
                if (device->GetInfo(info))
                {
                    crow::json::wvalue responseData({{"data", DeviceInfoToJson(info)}});
                    return SuccessResponse(response, "Device's info is got successfully", responseData);
                }
                else
                {
                    return FailResponse(response, ErrorCode::DEVICE_GETINFO_ERROR, "Device's info is got failed");
                }
            });
        });

    CROW_ROUTE(crowApp, "/device/api/v1/speaker-check")
        .methods("GET"_method)([] (const crow::request& request, crow::response& response){
            HandleDeviceGetReq(request, response, [](const std::shared_ptr<Device>& device, crow::response& response) {
                if (device->CheckSpeaker())
                {
                    return SuccessResponse(response, "Device's speaker is checked successfully");
                }
                else
                {
                    return FailResponse(response, ErrorCode::DEVICE_CHECKSPEAKER_ERROR, "Device's speaker is checked failed");
                }
            }); 
        });

    CROW_ROUTE(crowApp, "/device/api/v1/flashing")
        .methods("POST"_method)([] (const crow::request& request, crow::response& response){ 
            HandleDevicePostReq(request, response, [](const std::shared_ptr<Device>& device, crow::response& response) {
                if (device->Flashing())
                {
                    return SuccessResponse(response, "Device is going to flashing");
                }
                else
                {
                    return FailResponse(response, ErrorCode::DEVICE_RESET_ERROR, "Device falshing failed");
                }
            });
        });

    CROW_ROUTE(crowApp, "/device/api/v1/reset")
        .methods("POST"_method)([] (const crow::request& request, crow::response& response){ 
            HandleDevicePostReq(request, response, [](const std::shared_ptr<Device>& device, crow::response& response) {
                if (device->Reset())
                {
                    return SuccessResponse(response, "Device is reset successflly");
                }
                else
                {
                    return FailResponse(response, ErrorCode::DEVICE_RESET_ERROR, "Device reset failed");
                }
            });
        });

    CROW_ROUTE(crowApp, "/device/api/v1/disconnect")
        .methods("POST"_method)([] (const crow::request& request, crow::response& response){ 
            HandleDevicePostReq(request, response, [](const std::shared_ptr<Device>& device, crow::response& response) {
                if (device->Disconnect())
                {
                    return SuccessResponse(response, "Device has been disconnected successfully");
                }
                else
                {
                    return FailResponse(response, ErrorCode::DEVICE_DISCONNECT_ERROR, "Device disconnect failed");
                }
            });
        });

    CROW_ROUTE(crowApp, "/device/api/v1/lock")
        .methods("POST"_method)([] (const crow::request& request, crow::response& response){ 
            HandleDevicePostReqWithParams<bool>(request, "lock", response, [](const std::shared_ptr<Device>& device, const auto lock, crow::response& response) {
                if (device->SetLock(lock))
                {
                    if (lock)
                    {
                        return SuccessResponse(response, "Device's has been locked successfully");
                    }
                    else
                    {
                        return SuccessResponse(response, "Device's has been unlocked successfully");
                    }
                }
                else
                {
                    if (lock)
                    {
                        return FailResponse(response, ErrorCode::DEVICE_LOCK_ERROR, "Device lock failed");
                    }
                    else
                    {
                        return FailResponse(response, ErrorCode::DEVICE_UNLOCK_ERROR, "Device unlock failed");
                    }
                }
            });
        });

    CROW_ROUTE(crowApp, "/device/api/v1/volume")
        .methods("POST"_method)([] (const crow::request& request, crow::response& response){
            HandleDevicePostReqWithParams<uint16_t>(request, "volume", response, [](const std::shared_ptr<Device>& device, const auto volume, crow::response& response) {
                if (device->SetVolume(volume))
                {
                    return SuccessResponse(response, "Device's volume changed successfully");
                }
                else
                {
                    return FailResponse(response, ErrorCode::DEVICE_SETVOLUME_ERROR, "Device volume change failed");
                }
            }); 
        });

    CROW_ROUTE(crowApp, "/devices/api/v1/list/connected")
        .methods("GET"_method)([] (const crow::request& request, crow::response& response){
            crow::json::wvalue::list devicesInfo;
            for (const auto& device : deviceManager_.GetConnectingDevices())
            {
                DeviceInfo info;
                if (device->GetInfo(info))
                {
                    devicesInfo.push_back(DeviceInfoToJson(info));
                }
            }
            crow::json::wvalue responseData({{"data", devicesInfo}});
            return SuccessResponse(response, "Get connected devices successfully", responseData);
        });

    CROW_ROUTE(crowApp, "/devices/api/v1/list/active-microphone")
        .methods("GET"_method)([] (const crow::request& request, crow::response& response){
            crow::json::wvalue::list devicesInfo;
            for (const auto& device : deviceManager_.GetActiveMicrophoneDevices())
            {
                DeviceInfo info;
                if (device->GetInfo(info))
                {
                    devicesInfo.push_back(DeviceInfoToJson(info));
                }
            }
            crow::json::wvalue responseData({{"data", devicesInfo}});
            return SuccessResponse(response, "Get Microphone-active devices is successfully", responseData);
        });

    CROW_ROUTE(crowApp, "/devices/api/v1/mute")
        .methods("POST"_method)([] (const crow::request& request, crow::response& response){ 
            HandleDevicesPostReqWithParams<bool>(request, "mute", response, [](const std::unordered_map<std::string, std::shared_ptr<Device>>& devices, const auto mute, crow::response& response) {
                crow::json::wvalue::list failDevices;
                bool success = false;

                for (const auto& item : devices)
                {
                    const auto& deviceId = item.first;
                    const auto& device = item.second;
                    if (!device->SetMute(mute)) {
                        failDevices.push_back(deviceId);
                    }
                    else {
                        success = true;
                    }
                }

                if (!success)
                {
                    if (mute)
                    {
                        return FailResponse(response, ErrorCode::DEVICE_MUTE_ERROR, "Devices' mute failed", failDevices);
                    }
                    else
                    {
                        return FailResponse(response, ErrorCode::DEVICE_UNMUTE_ERROR, "Devices' unmute failed", failDevices);
                    }
                }
                else if (!failDevices.empty())
                {
                    crow::json::wvalue responseDate({{"failDevices", failDevices}});
                    if (mute)
                    {
                        return SuccessResponse(response, "Devices' mute changed with wrong", responseDate);
                    }
                    else
                    {
                        return SuccessResponse(response, "Devices' unmute changed with wrong", responseDate);
                    }
                }
                else
                {
                    if (mute)
                    {
                        return SuccessResponse(response, "Devices' mute changed successfully");
                    }
                    else
                    {
                        return SuccessResponse(response, "Devices' unmute changed successfully");
                    }
                }
            });
        });
}

std::shared_ptr<Device> DevicesApiController::GetDevice(const std::string& deviceId)
{
    return deviceManager_.Get(deviceId);
}
