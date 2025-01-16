// #include <boost/filesystem.hpp>
#include "Version.h"
#include "apiControllers/SystemApiController.h"
#include "utils/FileUtils.h"
#include "utils/LogUtils.h"
#include "utils/ResUtils.h"

void SystemApiController::InitRoutes(CrowApp& crowApp) {
    CROW_ROUTE(crowApp, "/version").methods("GET"_method)([&] {
        crow::json::wvalue versionInfo(
            {{"version", VersionInfo::VERSION}, {"hash", VersionInfo::GIT_HASH}, {"count", VersionInfo::COMMIT_COUNT}}
        );
        return versionInfo;
    });

    CROW_ROUTE(crowApp, "/health").methods("GET"_method)([] { return "OK"; });
    
    CROW_ROUTE(crowApp, "/log-download").methods("GET"_method)([] (const crow::request& request, crow::response& response){ 
        HandleFileRetrievalReq(response, LOG_FILE_ABSOLUTE_PATH, [](crow::response &response, const Poco::File &logFile) {
            return ResponseWithFile(response, "log.txt", logFile);
        });
    });
}
