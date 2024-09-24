#include "Version.h"
#include "controllers/VersionController.h"

void VersionController::InitRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/version").methods("GET"_method)([&] {
        crow::json::wvalue info(
            {{"version", VersionInfo::version}, {"hash", VersionInfo::hash}, {"count", VersionInfo::count}});
        return info;
    });
}
