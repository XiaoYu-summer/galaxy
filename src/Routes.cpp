#include "Routes.h"
#include "controllers/PosternController.h"
#include "controllers/SystemController.h"
#include "controllers/UpgradeController.h"

void SetupRoutes(CrowApp& app) {
    SystemController::InitRoutes(app);
    UpgradeController::InitRoutes(app);
    PosternController::InitRoutes(app);
};