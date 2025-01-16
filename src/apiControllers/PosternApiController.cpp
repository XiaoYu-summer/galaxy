#include <boost/process.hpp>

#include "apiControllers/PosternApiController.h"
#include "utils/ResUtils.h"

void PosternApiController::InitRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/postern/api/service/restart")
        .methods("POST"_method)([&](const crow::request& request, crow::response& response) {
            auto requestBody = crow::json::load(request.body);
            if (!requestBody || !requestBody.has("pwd") || requestBody["pwd"].s() != "CVTEMH") {
                return FailResponse(response, ErrorCode::PARAMS_ERROR, "Invalid parameters");
            }

#ifdef NDEBUG
            std::string restartCommand = "sleep 0.01 && /etc/init.d/S99galaxy restart";
            boost::process::child restartProcess(restartCommand, boost::process::std_out > stdout,
                                                 boost::process::std_err > stderr);
            restartProcess.detach();
#else
            std::cout << "Build type: Debug, System Not Restart" << std::endl;
#endif
            return SuccessResponse(response, "Service restarting...");
        });
}
