#include "Routes.h"
#include "apiControllers/PassportApiController.h"
#include "apiControllers/PosternApiController.h"
#include "apiControllers/SystemApiController.h"
#include "apiControllers/UpgradeApiController.h"
#include "apiControllers/DevicesApiController.h"

void SetupRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/")
    ([] {
        crow::mustache::set_base(".");
        crow::mustache::context ctx;
        return crow::mustache::load("assets/index.html").render();
    });
    SystemApiController::InitRoutes(app);
    UpgradeApiController::InitRoutes(app);
    PosternApiController::InitRoutes(app);
    PassportApiController::InitRoutes(app);
    DevicesApiController::InitRoutes(app);
};