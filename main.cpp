#pragma execution_character_set("utf-8")
#define CROW_STATIC_DIRECTORY "assets/"
#define CROW_STATIC_ENDPOINT "/assets/<path>"
#include "Poco/AutoPtr.h"
#include "Poco/Logger.h"
#include <Poco/Path.h>
#include "Poco/File.h"
#include "Poco/FileChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/Environment.h"
#include "Poco/Util/Application.h"
#ifdef _WIN32
#include "Poco/WindowsConsoleChannel.h"
#else
#include "Poco/ConsoleChannel.h"
#endif
#include "Poco/AsyncChannel.h"

#include "Routes.h"
#include "config/AppConfig.h"
#include "mdns/MDNSService.h"
#include "middleware/PassportMiddleware.h"
#include "types/App.h"
#include "utils/LogUtils.h"
#include "utils/PassportUtils.h"
#include "controllers/DevicesController.h"
#include "common/LoggerWrapper.h"

void InitLogger()
{
    // logging formats
    Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter("[%Y-%m-%d %H:%M:%S:%i tid=%I %q %s] %t (%U:%u)"));

    // splitter channel
    Poco::AutoPtr<Poco::SplitterChannel> splitterChannel(new Poco::SplitterChannel());

    // file channels
    Poco::AutoPtr<Poco::FileChannel> fileChannel(new Poco::FileChannel());

    Poco::Timestamp::fromEpochTime(time(0));

    std::string suffix = Poco::Environment::get("LOG_FILE_SUFFIX", std::string("log"));
    std::string filepath = "./logs/galaxy." + (suffix.empty() ? "log" : suffix);
    Poco::Path pocoLogFilePath = Poco::Path::expand(filepath);
    try
    {
        Poco::File(pocoLogFilePath.parent()).createDirectories();
    }
    catch (Poco::Exception &)
    {
        poco_error(Poco::Util::Application::instance().logger(), "Create logs directory failed");
        return;
    }

    fileChannel->setProperty("rotation", "100M");
    fileChannel->setProperty("archive", "timestamp");
    fileChannel->setProperty("compress", "true");
    fileChannel->setProperty("purgeAge", "4 weeks");
    fileChannel->setProperty("flush", "false");
    fileChannel->setProperty("path", filepath);

    //////////  This is only for DEBUG/DEV Environment.               ////////////////////////////
    //////////  Update the purge strategy to purge by log file count. ////////////////////////////
    const auto purgeCount = Poco::Environment::get("MAX_LOG_PURGE_COUNT", "10");
    if (!purgeCount.empty())
    {
        fileChannel->setProperty("purgeCount", purgeCount);
    }
    //////////////////////////////////////////////////////////////////////////////////////////////

    // console channel
#ifdef _DEBUG
#ifdef _WIN32
    Poco::AutoPtr<Poco::WindowsColorConsoleChannel> logConsole = new Poco::WindowsColorConsoleChannel();
    splitterChannel->addChannel(logConsole);
#else
    Poco::AutoPtr<Poco::ColorConsoleChannel> colorConsoleChannel(new Poco::ColorConsoleChannel());
    colorConsoleChannel->setProperty("traceColor", "gray");
    colorConsoleChannel->setProperty("debugColor", "white");
    colorConsoleChannel->setProperty("informationColor", "green");
    colorConsoleChannel->setProperty("noticeColor", "blue");
    colorConsoleChannel->setProperty("warningColor", "yellow");
    colorConsoleChannel->setProperty("errorColor", "lightRed");
    colorConsoleChannel->setProperty("criticalColor", "magenta");
    colorConsoleChannel->setProperty("fatalColor", "red");
    splitterChannel->addChannel(colorConsoleChannel);
#endif
    splitterChannel->addChannel(fileChannel);
    Poco::AutoPtr<Poco::AsyncChannel> asyncChannel(new Poco::AsyncChannel(splitterChannel));
#else
    Poco::AutoPtr<Poco::AsyncChannel> asyncChannel(new Poco::AsyncChannel(fileChannel));
#endif

    Poco::AutoPtr<Poco::FormattingChannel> formattingChannel(new Poco::FormattingChannel(patternFormatter, asyncChannel));

    std::vector<std::string> logNames;
    Poco::Logger::root().names(logNames);
    for (auto it = logNames.begin(); it != logNames.end(); ++it)
    {
        Poco::Logger::root().setChannel(*it, formattingChannel);
    }
    Poco::Logger::root().create("LoadManager", formattingChannel);
#ifdef _DEBUG
    Poco::Logger::root().setLevel(Poco::Message::PRIO_DEBUG);
#else
    Poco::Logger::root().setLevel(Poco::Message::PRIO_INFORMATION);
#endif
}

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
    if (!AppConfig::Instance().LoadConfig("config/app_config.json"))
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
    std::vector<std::pair<std::string, std::string>> txt =
    {
        {"version", "1.0"}, {"type", "galaxy-server"}, {"api", "wzw rest"}
    };

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
