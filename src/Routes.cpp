#include "Routes.h"
#include "controllers/HelloController.h"
#include "controllers/UpgradeController.h"

void SetupRoutes(crow::SimpleApp& app) {
    HelloController::InitRoutes(app);
    UpgradeController::InitRoutes(app);
};