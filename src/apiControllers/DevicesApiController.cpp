#include "devices/DeviceManager.h"
#include "devices/Device.h"
#include "apiControllers/DevicesApiController.h"
#include "utils/ResUtils.h"

std::shared_ptr<DeviceManager> DevicesApiController::deviceManager_ = std::make_shared<DeviceManager>();

DevicesApiController::DevicesApiController()
{
}

void DevicesApiController::InitRoutes(CrowApp& crowApp)
{
    CROW_ROUTE(crowApp, "/device/api/v1/reset")
        .methods("POST"_method)([] (const crow::request& request, crow::response& response){ 
            const char * deviceId = request.url_params.get("deviceId");
            if (!deviceId)
            {
                return FailResponse(response, ErrorCode::PARAMS_ERROR, "'deviceId' is required");
            }

            const auto device = deviceManager_->get(deviceId);
            if (!device)
            {
                return FailResponse(response, ErrorCode::DEVICEID_INVAILD_ERROR, "'deviceId' is invalid");
            }

            if (device->ResetDevice())
            {
                return SuccessResponse(response, "Device is reset successflly");
            }
            else
            {
                return FailResponse(response, ErrorCode::DEVICEID_RESET_ERROR, "Device reset failed");
            }
        });
}
