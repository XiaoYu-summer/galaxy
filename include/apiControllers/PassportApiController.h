#pragma once
#include "types/App.h"

class PassportApiController {
   public:
    static void InitRoutes(CrowApp& app);

   private:
    static void InitAdminRoutes(CrowApp& app);
    static void InitClientRoutes(CrowApp& app);
};
