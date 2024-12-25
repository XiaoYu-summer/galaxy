#pragma once
#include <crow.h>

#include <chrono>

#include "Constant.h"
#include "code/ErrorCode.h"
#include "crow/middlewares/session.h"
#include "middleware/ReqLoggerMiddleware.h"
#include "utils/ResUtils.h"

using Session = crow::SessionMiddleware<crow::InMemoryStore>;

static const std::unordered_set<std::string> SKIP_PATHS = {"/",       "/passport", "/version",
                                                           "/assets", "/health",   "/postern"};

struct PassportMiddleware {
    struct context {};

    crow::App<crow::CookieParser, Session, ReqLoggerMiddleware, PassportMiddleware>& app_;

    void before_handle(crow::request& request, crow::response& response, context& context) {
        std::string requestPath = request.url;

        for (const auto& skipPath : SKIP_PATHS) {
            if (requestPath.compare(0, skipPath.size(), skipPath) == 0) {
                return;
            }
        }

        std::string authToken = request.get_header_value("Authorization");
        auto& session = app_.get_context<Session>(request);

        auto sessionToken = session.get(TOKEN_KEY, "");
        auto tokenExpireTime = session.get(TOKEN_EXPIRE_KEY, 0);
        auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        if (sessionToken.empty() || authToken.empty() || sessionToken != authToken) {
            return FailResponse(response, ErrorCode::AUTH_ERROR, "Authorization failed");
        }

        if (currentTime > tokenExpireTime) {
            FailResponse(response, ErrorCode::AUTH_EXPIRED, "Authorization expired");
        } else {
            auto newExpireTime = std::chrono::system_clock::now() + std::chrono::hours(1);
            session.set(TOKEN_EXPIRE_KEY, std::chrono::system_clock::to_time_t(newExpireTime));
        }
    }

    void after_handle(crow::request& request, crow::response& response, context& context) {}
};