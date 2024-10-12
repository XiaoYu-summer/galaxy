#include <boost/process.hpp>

#include "controllers/PosternController.h"
#include "utils/ResUtils.h"

void PosternController::InitRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/postern/service/restart")
        .methods("POST"_method)([&](const crow::request& req, crow::response& res) {
            auto body = crow::json::load(req.body);
            if (!body || !body.has("pwd") || body["pwd"].s() != "CVTEMH") {
                return FailResponse(res, ErrorCode::PARAMS_ERROR, "params error");
            }

#ifdef NDEBUG
            // 重启应用
            std::string restart = "sleep 0.01 && /etc/init.d/S99galaxy restart";
            boost::process::child c(restart, boost::process::std_out > stdout, boost::process::std_err > stderr);
            c.detach();
#else
            std::cout << "Build type: Debug, System Not Restart" << std::endl;
#endif
            return SuccessResponse(res, "Restarting...");
        });
}
