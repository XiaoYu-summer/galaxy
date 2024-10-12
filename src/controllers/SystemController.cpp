#include "Version.h"
#include "controllers/SystemController.h"

void SystemController::InitRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/version").methods("GET"_method)([&] {
        crow::json::wvalue info(
            {{"version", VersionInfo::version}, {"hash", VersionInfo::hash}, {"count", VersionInfo::count}});
        return info;
    });
    CROW_ROUTE(app, "/health").methods("GET"_method)([&] { return "health"; });
}
