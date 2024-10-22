#include "Routes.h"
#include "controllers/PassportController.h"
#include "controllers/PosternController.h"
#include "controllers/SystemController.h"
#include "controllers/UpgradeController.h"

void SetupRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/")
    ([] {
        crow::mustache::set_base(".");
        crow::mustache::context ctx;
        return crow::mustache::load("assets/index.html").render();
    });
    SystemController::InitRoutes(app);
    UpgradeController::InitRoutes(app);
    PosternController::InitRoutes(app);
    PassportController::InitRoutes(app);
};