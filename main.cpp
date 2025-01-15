#pragma execution_character_set("utf-8")
#define CROW_STATIC_DIRECTORY "assets/"
#define CROW_STATIC_ENDPOINT "/assets/<path>"

#include "Routes.h"
#include "config/AppConfig.h"
#include "mdns/MDNSService.h"
#include "middleware/PassportMiddleware.h"
#include "types/App.h"
#include "utils/LogUtils.h"
#include "utils/PassportUtils.h"

int main() {
    CROW_LOG_INFO << "Starting Galaxy Server...";

    // 加载APP配置
    CROW_LOG_INFO << "Loading app configuration...";
    if (!AppConfig::Instance().LoadConfig("config/app_config.json")) {
        CROW_LOG_ERROR << "Failed to load app config";
        return 1;
    }
    CROW_LOG_INFO << "App configuration loaded successfully.";

    CROW_LOG_INFO << "Initializing Crow app...";
    CrowApp app{Session{crow::CookieParser::Cookie("_sso_token_").max_age(/*one day*/ 24 * 60 * 60).path("/"), 4,
                        crow::InMemoryStore{}},
                PassportMiddleware{app}};

    // 设置日志级别
    CROW_LOG_INFO << "Setting up logging...";
    crow::logger::setLogLevel(crow::LogLevel::Info);
    FileLogHandler logger;
    crow::logger::setHandler(&logger);

    // 设置路由
    CROW_LOG_INFO << "Setting up routes...";
    SetupRoutes(app);

    CROW_LOG_INFO << "Initializing account password file...";
    PassportUtils::InitAccountPasswordFile();

    // 发布服务信息
    CROW_LOG_INFO << "Publishing mDNS service...";
    mdns::MDNSService mdnsPublisher;
    std::vector<std::pair<std::string, std::string>> txt = {
        {"version", "1.0"}, {"type", "galaxy-server"}, {"api", "wzw rest"}};

    try {
        if (!mdnsPublisher.Publish("PAT71", "http", 80, txt)) {
            CROW_LOG_ERROR << "Failed to publish mDNS service";
        } else {
            CROW_LOG_INFO << "Published mDNS service: Galaxy Server._http._tcp.local";
        }
    } catch (const std::exception& e) {
        CROW_LOG_ERROR << "Exception while publishing mDNS service: " << e.what();
    }
    // 启动服务器
    CROW_LOG_INFO << "Starting Crow server...";
    try {
        app.port(80).multithreaded().run();
    } catch (const std::exception& e) {
        CROW_LOG_ERROR << "Exception while running Crow server: " << e.what();
    }
    return 0;
}
