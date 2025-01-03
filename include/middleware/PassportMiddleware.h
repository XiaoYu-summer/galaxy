#pragma once
#include <crow.h>

#include <chrono>

#include "Constant.h"
#include "code/ErrorCode.h"
#include "config/AppConfig.h"
#include "crow/middlewares/session.h"
#include "middleware/ReqLoggerMiddleware.h"
#include "nlohmann/json.hpp"
#include "utils/ResUtils.h"
#include "utils/RsaUtils.h"

using Session = crow::SessionMiddleware<crow::InMemoryStore>;
using json = nlohmann::json;

// 5分钟的时间戳有效期
static const int64_t TIMESTAMP_VALID_DURATION = 5 * 60;
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

        // 检查是否是APP对接（通过RSA认证）
        std::string appId = request.get_header_value("X-App-Id");
        std::string signature = request.get_header_value("X-Signature");

        if (!appId.empty() && !signature.empty()) {
            // APP对接模式：使用RSA解密验证
            std::string privateKey = GetPrivateKey(appId);  // 获取服务端私钥
            if (privateKey.empty()) {
                return FailResponse(response, ErrorCode::AUTH_ERROR, "Invalid app id");
            }

            // 使用私钥解密X-Signature中的数据
            std::string decryptedData = RsaUtils::Decrypt(signature, privateKey);
            if (decryptedData.empty()) {
                return FailResponse(response, ErrorCode::AUTH_ERROR, "Decrypt signature failed");
            }

            try {
                // 解析解密后的JSON数据
                json signData = json::parse(decryptedData);

                // 验证必要字段
                if (!signData.contains("appId") || !signData.contains("timestamp") || !signData.contains("nonce") ||
                    !signData.contains("path")) {
                    return FailResponse(response, ErrorCode::AUTH_ERROR, "Invalid signature format");
                }

                // 验证appId
                if (signData["appId"] != appId) {
                    return FailResponse(response, ErrorCode::AUTH_ERROR, "AppId mismatch");
                }

                // 验证路径
                if (signData["path"] != requestPath) {
                    return FailResponse(response, ErrorCode::AUTH_ERROR, "Path mismatch");
                }

                // // 验证时间戳
                // int64_t timestamp = signData["timestamp"].get<int64_t>();
                // int64_t currentTime = std::chrono::duration_cast<std::chrono::seconds>(
                //                           std::chrono::system_clock::now().time_since_epoch())
                //                           .count();

                // if (std::abs(currentTime - timestamp) > TIMESTAMP_VALID_DURATION) {
                //     return FailResponse(response, ErrorCode::AUTH_ERROR, "Timestamp expired");
                // }

                // TODO: 可以添加nonce防重放检查

                return;  // 验证通过
            } catch (const json::exception& e) {
                return FailResponse(response, ErrorCode::AUTH_ERROR, "Invalid signature data");
            }
        }

        // 普通用户模式：使用token验证
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

   private:
    std::string GetPrivateKey(const std::string& appId) { return AppConfig::Instance().GetPrivateKey(appId); }
};