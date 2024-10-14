#pragma once
#include <crow.h>

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
        // 如果token为空，返回错误信息
        if (token.empty() || token.empty() || token != authToken) {
            FailResponse(res, ErrorCode::PARAMS_ERROR, "Authorization");
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};