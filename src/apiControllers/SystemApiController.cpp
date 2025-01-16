#include "Version.h"
#include "apiControllers/SystemApiController.h"

void SystemApiController::InitRoutes(CrowApp& crowApp) {
    CROW_ROUTE(crowApp, "/version").methods("GET"_method)([&] {
        crow::json::wvalue versionInfo(
            {{"version", VersionInfo::VERSION}, {"hash", VersionInfo::GIT_HASH}, {"count", VersionInfo::COMMIT_COUNT}});
        return versionInfo;
    });

    CROW_ROUTE(crowApp, "/health").methods("GET"_method)([&] { return "OK"; });
}
