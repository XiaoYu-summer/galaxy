#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>

#include "Constant.h"
#include "controllers/PassportController.h"
#include "utils/PassportUtils.h"
#include "utils/ResUtils.h"

void PassportController::InitRoutes(CrowApp& app) {
    CROW_ROUTE(app, "/passport/api/v1/token")
        .methods("POST"_method)([&app](const crow::request& req, crow::response& res) {
            // 校验账号密码是否正确
            auto body = crow::json::load(req.body);
            if (!body) {
                return FailResponse(res, ErrorCode::PASSWORD_ERROR, "Invalid request body");
            }
            std::string account = body["account"].s();
            std::string password = body["password"].s();
            // 获取GetAccountPasswordFile路径的JSON文件，读取account 与 password
            std::string accountFile = PassportUtils::GetAccountPasswordFile().string();
            std::ifstream ifs(accountFile);
            if (!ifs) {
                PassportUtils::InitAccountPasswordFile();
                return FailResponse(res, ErrorCode::AUTH_ERROR, "Server Passport not found");
            }
            // 读取文件内容到字符串
            std::string fileContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            ifs.close();
            crow::json::rvalue accountPassword = crow::json::load(fileContent);
            // 简单校验
            if (account != accountPassword["account"].s() || password != accountPassword["password"].s()) {
                return FailResponse(res, ErrorCode::PASSWORD_ERROR, "Authorization failed");
            }
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
    // 修改密码
    CROW_ROUTE(app, "/passport/api/v1/password")
        .methods("PUT"_method)([&app](const crow::request& req, crow::response& res) {
            // 校验账号密码是否正确
            auto body = crow::json::load(req.body);
            if (!body) {
                return FailResponse(res, ErrorCode::PASSWORD_ERROR, "Invalid request body");
            }
            std::string password = body["password"].s();
            std::string newPassword = body["newPassword"].s();
            // 读取账号密码
            std::string storedAccount, storedPassword;
            std::tie(storedAccount, storedPassword) = PassportUtils::ReadAccountPassword();
            if (password != storedPassword) {
                return FailResponse(res, ErrorCode::OLD_PASSWORD_ERROR, "old password error");
            }
            // 写入新密码
            PassportUtils::WriteAccountPassword(storedAccount, newPassword);
            return SuccessResponse(res, "success");
        });
    // 登出
    CROW_ROUTE(app, "/passport/api/v1/token")
        .methods("DELETE"_method)([&app](const crow::request& req, crow::response& res) {
            auto& session = app.get_context<Session>(req);
            // 销毁会话
            session.remove("token");
            session.remove("expireTime");
            return SuccessResponse(res, "Logged out successfully");
        });
}