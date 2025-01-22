#include "Version.h"
#include "apiControllers/SystemApiController.h"
#include "utils/FileUtils.h"
#include "utils/JsonParamsParseHelper.h"
#include "utils/LogUtils.h"
#include "utils/ResUtils.h"

void SystemApiController::InitRoutes(CrowApp& crowApp) {
    CROW_ROUTE(crowApp, "/version").methods("GET"_method)([&] {
        crow::json::wvalue versionInfo(
            {{"version", VersionInfo::VERSION}, {"hash", VersionInfo::GIT_HASH}, {"count", VersionInfo::COMMIT_COUNT}});
        return versionInfo;
    });

    CROW_ROUTE(crowApp, "/health").methods("GET"_method)([] { return "OK"; });

    CROW_ROUTE(crowApp, "/log-download")
        .methods("GET"_method)([](const crow::request& request, crow::response& response) {
            HandleFileRetrievalReq(response, LOG_FILE_ABSOLUTE_PATH,
                [](crow::response& response, const Poco::File& logFile) {
                    return ResponseWithFile(response, "log.txt", logFile);
                });
        });

    CROW_ROUTE(crowApp, "/system/api/v1/channel/in")
        .methods("GET"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Get PAT71 in-channel config success success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/channel/in")
        .methods("PUT"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Set PAT71 in-channel config success success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/channel/out")
        .methods("GET"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Get PAT71 out-channel config success success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/channel/out")
        .methods("PUT"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Set PAT71 out-channel config success success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/channel/matrix")
        .methods("GET"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Get PAT71 out-channel config success success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/channel/matrix")
        .methods("PUT"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Set PAT71 out-channel config success success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/serial")
        .methods("GET"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Get network serial success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/serial")
        .methods("PUT"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Set network serial success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/network")
        .methods("GET"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Get network config success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/network")
        .methods("PUT"_method)([](const crow::request& request, crow::response& response) {
            crow::json::wvalue responseData({{}});
            return SuccessResponse(response, "Set network config success", responseData);
        });

    CROW_ROUTE(crowApp, "/system/api/v1/speaker-nums")
        .methods("GET"_method)([](const crow::request& request, crow::response& response) {
            return SuccessResponse(response, "Get the number of speaker successfully", {{"speakerNums", 2}});
        });

    CROW_ROUTE(crowApp, "/system/api/v1/speaker-nums")
        .methods("PUT"_method)([](const crow::request& request, crow::response& response) {
            HandleJsonReqWithParams<uint16_t>(request, response,
                [](const uint16_t speakerNums, crow::response& response) {
                    return SuccessResponse(response, "Set the number of speaker successfully");
                }, "speakerNums");
        });

    CROW_ROUTE(crowApp, "/system/api/v1/config-file/export")
        .methods("GET"_method)([](const crow::request& request, crow::response& response) {
            HandleFileRetrievalReq(response, APP_CONFIG_ABSOLUTE_PATH,
                [](crow::response& response, const Poco::File& appConfigFile) {
                    return ResponseWithFile(response, APP_CONFIG_NAME, appConfigFile);
                });
        });

    CROW_ROUTE(crowApp, "/system/api/v1/config-file/import")
        .methods("PUT"_method)([](const crow::request& request, crow::response& response) {
            auto contentLength = request.get_header_value("Content-Length");
            if (contentLength.empty() || std::stoul(contentLength) > 100 * 1024 * 1024) {
                return FailResponse(response, ErrorCode::FILE_TOO_LARGE, "File too large");
            }

            if (request.get_header_value("Content-Type").find("multipart/form-data") == std::string::npos) {
                return FailResponse(response, ErrorCode::PARAMS_ERROR, "Invalid parameters");
            }

            crow::multipart::message formData(request);
            auto fileData = formData.get_part_by_name("configFile");
            auto contentDisposition = fileData.get_header_object("content-disposition");
            std::string fileName = FileUtils::GetFullFileName(contentDisposition.params);
            if (fileName.empty()) {
                return FailResponse(response, ErrorCode::PARAMS_ERROR, "Invalid parameters");
            }

            const std::string fileContent = fileData.body;
            if (fileContent.empty()) {
                return FailResponse(response, ErrorCode::PARAMS_ERROR, "Invalid parameters");
            }

            return SuccessResponse(response, "Import configfile Success");
        });
}
