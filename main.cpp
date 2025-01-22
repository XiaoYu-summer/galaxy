#pragma execution_character_set("utf-8")

#include "Routes.h"
#include "config/AppConfig.h"
#include "mdns/MDNSService.h"
#include "types/App.h"
#include "utils/LogUtils.h"
#include "utils/PassportUtils.h"

int main()
{
    InitLogger();
    // 设置日志级别
    LOG_I("Setting up logging...");
    crow::logger::setLogLevel(crow::LogLevel::Info);
    FileLogHandler logger;
    crow::logger::setHandler(&logger);

    LOG_I("Starting Galaxy Server...");

    // 加载APP配置
    LOG_I("Loading app configuration...");
    if (!AppConfig::Instance().LoadConfig(APP_CONFIG_ABSOLUTE_PATH))
    {
        LOG_E("Failed to load app config");
        return 1;
    }
    LOG_I("App configuration loaded successfully.");

    LOG_I("Initializing Crow app...");
    CrowApp app{Session{crow::CookieParser::Cookie("_sso_token_").max_age(/*one day*/ 24 * 60 * 60).path("/"), 4,
                        crow::InMemoryStore{}},
                PassportMiddleware{app}};

    // 设置路由
    LOG_I("Setting up routes...");
    SetupRoutes(app);

    LOG_I("Initializing account password file...");
    PassportUtils::InitAccountPasswordFile();

    // 发布服务信息
    LOG_I("Publishing mDNS service...");
    mdns::MDNSService mdnsPublisher;
    std::vector<std::pair<std::string, std::string>> txt;
    txt.emplace_back("version", "1.0");
    txt.emplace_back("author", "example");

    try
    {
        if (!mdnsPublisher.Publish("PAT71", "http", 80, txt))
        {
            LOG_E("Failed to publish mDNS service");
        }
        else
        {
            LOG_I("Published mDNS service: Galaxy Server._http._tcp.local");
        }
    }
    catch (const std::exception& e)
    {
        LOG_E("Exception while publishing mDNS service: " << e.what());
    }

    // 启动服务器
    LOG_I("Starting Crow server...");
    try
    {
        app.port(80).multithreaded().run();
    }
    catch (const std::exception& e)
    {
        LOG_E("Exception while running Crow server: " << e.what());
    }
    return 0;
}
