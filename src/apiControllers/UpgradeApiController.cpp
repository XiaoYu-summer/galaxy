#include <boost/filesystem.hpp>
#include <initializer_list>
#include <unordered_set>

#include "code/ErrorCode.h"
#include "apiControllers/UpgradeApiController.h"
#include "services/upgrade/ServiceUpgradeService.h"
#include "services/upgrade/SystemUpgradeService.h"
#include "utils/FileUtils.h"
#include "utils/ResUtils.h"

bool ValidateUpgradeParameters(crow::multipart::message& formData) {
    auto fileData = formData.get_part_by_name("file");
    auto upgradeType = formData.get_part_by_name("type");
    auto md5Hash = formData.get_part_by_name("md5");
    auto contentDisposition = fileData.get_header_object("content-disposition");

    const std::unordered_set<std::string> validTypes = {"view", "service", "firmware", "system"};

    return contentDisposition.params.size() == 0 || upgradeType.body.empty() || md5Hash.body.empty() ||
           validTypes.find(upgradeType.body) == validTypes.end();
}

void UpgradeApiController::InitRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/resource/api/v1/update")
        .methods("POST"_method)([&app](const crow::request& request, crow::response& response) {
            auto contentLength = request.get_header_value("Content-Length");
            if (contentLength.empty() || std::stoul(contentLength) > 100 * 1024 * 1024) {
                return FailResponse(response, ErrorCode::FILE_TOO_LARGE, "File too large");
            }

            crow::multipart::message formData(request);
            if (ValidateUpgradeParameters(formData)) {
                return FailResponse(response, ErrorCode::PARAMS_ERROR, "Invalid parameters");
            }

            auto fileData = formData.get_part_by_name("file");
            auto upgradeType = formData.get_part_by_name("type");
            auto md5Hash = formData.get_part_by_name("md5");
            auto contentDisposition = fileData.get_header_object("content-disposition");

            bool isMD5Match = FileUtils::VerifyMD5(fileData.body, md5Hash.body);
            if (isMD5Match) {
                std::string fileName = FileUtils::GetFullFileName(contentDisposition.params);
                boost::filesystem::path appCurrentPath = boost::filesystem::current_path();
                boost::filesystem::path saveFilePath = appCurrentPath / "upgrades" / upgradeType.body / fileName;
                FileUtils::SaveFile(fileData.body, saveFilePath.string());

                try {
                    if (upgradeType.body == "service") {
                        ServiceUpgradeService::UpgradeService(saveFilePath.string(), fileName);
                    } else if (upgradeType.body == "system") {
                        SystemUpgradeService::UpgradeSystem(saveFilePath.string(), fileName);
                    } else if (upgradeType.body == "firmware") {
                        // FirmwareUpgradeService::UpgradeFirmware(saveFilePath.string());
                    }
                    return SuccessResponse(response, "Upgrade initiated successfully");
                } catch (const std::exception& error) {
                    return FailResponse(response, ErrorCode::UPGRADE_ERROR, error.what());
                }
            } else {
                return FailResponse(response, ErrorCode::MD5_MISMATCH, "MD5 verification failed");
            }
        });

    CROW_ROUTE(app, "/resource/api/v1/rollback")
        .methods("POST"_method)([&app](const crow::request& request, crow::response& response) {
            auto requestBody = crow::json::load(request.body);
            if (!requestBody) {
                return FailResponse(response, ErrorCode::PARAMS_ERROR, "Invalid request body");
            }

            std::string upgradeType = requestBody["type"].s();
            try {
                // if (upgradeType == "service") {
                //     ServiceUpgradeService::RollbackService();
                // } else if (upgradeType == "system") {
                //     SystemUpgradeService::RollbackSystem();
                // } else if (upgradeType == "firmware") {
                //     FirmwareUpgradeService::RollbackFirmware();
                // } else {
                //     return FailResponse(response, ErrorCode::PARAMS_ERROR, "Invalid upgrade type");
                // }
                return SuccessResponse(response, "Rollback initiated successfully");
            } catch (const std::exception& error) {
                return FailResponse(response, ErrorCode::UPGRADE_ERROR, error.what());
            }
        });
}
