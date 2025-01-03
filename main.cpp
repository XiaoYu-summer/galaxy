#pragma execution_character_set("utf-8")
#define CROW_STATIC_DIRECTORY "assets/"
#define CROW_STATIC_ENDPOINT "/assets/<path>"

#include "Routes.h"
#include "config/AppConfig.h"
#include "middleware/PassportMiddleware.h"
#include "types/App.h"
#include "utils/LogUtils.h"
#include "utils/PassportUtils.h"

int main() {
    // 加载APP配置
    if (!AppConfig::Instance().LoadConfig("config/app_config.json")) {
        CROW_LOG_ERROR << "Failed to load app config";
        return 1;
    }

    CrowApp app{Session{crow::CookieParser::Cookie("_sso_token_").max_age(/*one day*/ 24 * 60 * 60).path("/"), 4,
                        crow::InMemoryStore{}},
                PassportMiddleware{app}};
    // 设置日志级别
    crow::logger::setLogLevel(crow::LogLevel::Info);
    FileLogHandler logger;
    crow::logger::setHandler(&logger);
    // 设置路由
    SetupRoutes(app);
    PassportUtils::InitAccountPasswordFile();
    // 启动服务器
    app.port(80).multithreaded().run();

    return 0;
}
