#include "Routes.h"
#include "controllers/PosternController.h"
#include "controllers/UpgradeController.h"
#include "controllers/VersionController.h"

void SetupRoutes(CrowApp& app) {
    VersionController::InitRoutes(app);
    UpgradeController::InitRoutes(app);
    PosternController::InitRoutes(app);
};