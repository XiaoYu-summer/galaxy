#pragma once
#include <crow.h>

#include <chrono>

#include "Constant.h"
#include "code/ErrorCode.h"
#include "crow/middlewares/session.h"
#include "middleware/ReqLoggerMiddleware.h"
#include "utils/ResUtils.h"

using Session = crow::SessionMiddleware<crow::InMemoryStore>;

static const std::unordered_set<std::string> skipPaths = {"/passport", "/version", "/assets", "/health", "/postern"};
struct PassportMiddleware {
    struct context {};

    crow::App<crow::CookieParser, Session, ReqLoggerMiddleware, PassportMiddleware>& app;  // 引用 CrowApp 对象

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // 校验授权
        // 获取请求链接
        std::string path = req.url;
        std::cout << "path: " << path << std::endl;
        // 如果是/passport开头相关的请求，不需要校验

        for (const auto& skipPath : skipPaths) {
            if (path.compare(0, skipPath.size(), skipPath) == 0) {
                return;
            }
        }
        // 获取请求头中的token
        std::string authToken = req.get_header_value("Authorization");
        auto& session = app.get_context<Session>(req);

        // this is automatically deduced to be a std::string
        // (even with the parameter being a char[])
        // one could also write: session.get<std::string>("user")
        auto token = session.get(TOKEN_KEY, "");
        auto expireTime = session.get(TOKEN_EXPIRE_KEY, 0);
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        // 如果token为空，返回错误信息
        if (token.empty() || authToken.empty() || token != authToken) {
            return FailResponse(res, ErrorCode::AUTH_ERROR, "Authorization failed");
        }
        if (now > expireTime) {
            FailResponse(res, ErrorCode::AUTH_EXPIRED, "Authorization expired");
        } else {
            // 更新过期时间
            auto expireTime = std::chrono::system_clock::now() + std::chrono::hours(1);
            session.set(TOKEN_EXPIRE_KEY, std::chrono::system_clock::to_time_t(expireTime));
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};