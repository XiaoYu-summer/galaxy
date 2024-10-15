#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>

#include "Constant.h"
#include "controllers/PassportController.h"
#include "utils/ResUtils.h"

void PassportController::InitRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/passport/v1/token").methods("POST"_method)([&app](const crow::request& req, crow::response& res) {
        auto& session = app.get_context<Session>(req);
        // 生成 UUID
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        std::string token = boost::uuids::to_string(uuid);
        // 设置 token
        session.set(TOKEN_KEY, token);
        // 设置过期时间
        auto expireTime = std::chrono::system_clock::now() + std::chrono::hours(1);
        session.set(TOKEN_EXPIRE_KEY, std::chrono::system_clock::to_time_t(expireTime));
        crow::json::wvalue r({{"accessToken", token}});
        return SuccessResponse(res, "success", r);
    });
}