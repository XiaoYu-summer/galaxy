#include "apiControllers/CameraApiController.h"
#include "utils/ResUtils.h"

void CameraApiController::InitRoutes(CrowApp& crowApp) {
    CROW_ROUTE(crowApp, "/camera/api/v1/linkage")
        .methods("PUT"_method)([](const crow::request& request, crow::response& response) {
            HandleJsonReqWithParams<bool, std::string>(request, response, [](const bool enable, const std::string& micId, crow::response& response) {
                if (enable) {
                    return SuccessResponse(response, "Enable linkage Successfully");
                } else {
                    return SuccessResponse(response, "Disable linkage Successfully");
                }
            },
                "enable", "micId");
        });

    CROW_ROUTE(crowApp, "/camera/api/v1/preset")
        .methods("GET"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{"micId", "micId"}, {"thumbnail", "thumbnail"}, {"eptzPositionId", "eptzPositionId"}});
            return SuccessResponse(response, "Get preset Successfully", responseData);
        });

    CROW_ROUTE(crowApp, "/camera/api/v1/preset")
        .methods("POST"_method)([](const crow::request& request, crow::response& response) {
            HandleJsonReqWithParams<std::string, std::string, std::string>(request, response, [](const std::string& micId, const std::string& thumbnail, std::string& eptzPositionId, crow::response& response) {
                return SuccessResponse(response, "Set preset Successfully");
            },
                "micId", "thumbnail", "eptzPositionId");
        });

    CROW_ROUTE(crowApp, "/camera/api/v1/preset")
        .methods("DELETE"_method)([](const crow::request& request, crow::response& response) {
            HandleJsonReqWithParams<std::string>(request, response, [](std::string& eptzPositionId, crow::response& response) {
                return SuccessResponse(response, "Delete preset Successfully");
            },
                "eptzPositionId");
        });
}