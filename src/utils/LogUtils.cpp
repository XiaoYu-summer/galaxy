#include <iostream>
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/AutoPtr.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/SplitterChannel.h"
#include "Poco/AsyncChannel.h"
#include "Poco/FileChannel.h"
#ifdef _WIN32
#include "Poco/WindowsConsoleChannel.h"
#else
#include "Poco/ConsoleChannel.h"
#endif
#include "Poco/Logger.h"
#include "Poco/Environment.h"
#include "Poco/Util/Application.h"

#include "utils/LogUtils.h"
#include "common/LoggerWrapper.h"

DEFINE_FILE_NAME("LogUtils.cpp")

constexpr const char DEFAULT_LOG_FILE_SUFFIX[] = "log";
const std::string LOG_FILE_SUFFIX = Poco::Environment::get("LOG_FILE_SUFFIX", DEFAULT_LOG_FILE_SUFFIX);
const std::string LOG_FILE_NAME = "galaxy." + LOG_FILE_SUFFIX;
const std::string LOG_FILE_RELATIVE_PATH = "./logs/" + LOG_FILE_NAME;
const Poco::Path LOG_FILE_RELATIVE_POCO = Poco::Path(LOG_FILE_RELATIVE_PATH);
const Poco::Path LOG_FILE_ABSOLUTE_POCO = LOG_FILE_RELATIVE_POCO.absolute();
const std::string LOG_FILE_ABSOLUTE_PATH = LOG_FILE_ABSOLUTE_POCO.toString();

FileLogHandler::FileLogHandler()
    : logger_(Poco::Logger::get("Crow"))
{

}

void FileLogHandler::log(std::string message, crow::LogLevel level) {
#if defined(_DEBUG) && !defined(NDEBUG)
    std::cout << message << std::endl;
#endif
    switch (level) {
        case crow::LogLevel::Debug:
            LOG_DEBUG(message);
            break;
        case crow::LogLevel::Info:
            LOG_INFO(message);
            break;
        case crow::LogLevel::Warning:
            LOG_WARNING(message);
            break;
        case crow::LogLevel::Error:
            LOG_ERROR(message);
            break;
        case crow::LogLevel::Critical:
            LOG_CRITICAL(message);
            break;
    }
}

static void _InitLogger()
{
    // logging formats
    Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(
        new Poco::PatternFormatter("[%Y-%m-%d %H:%M:%S:%i tid=%I %q %s] %t (%U:%u)"));

    // splitter channel
    Poco::AutoPtr<Poco::SplitterChannel> splitterChannel(new Poco::SplitterChannel());

    // file channels
    Poco::AutoPtr<Poco::FileChannel> fileChannel(new Poco::FileChannel());

    Poco::Timestamp::fromEpochTime(time(0));

    try
    {
        Poco::File(Poco::Path(Poco::Path::expand(LOG_FILE_RELATIVE_PATH)).parent()).createDirectories();
    }
    catch (Poco::Exception&)
    {
        poco_error(Poco::Util::Application::instance().logger(), "Create logs directory failed");
        return;
    }

    fileChannel->setProperty("rotation", "100M");
    fileChannel->setProperty("archive", "timestamp");
    fileChannel->setProperty("compress", "true");
    fileChannel->setProperty("purgeAge", "4 weeks");
    fileChannel->setProperty("flush", "false");
    fileChannel->setProperty("path", LOG_FILE_ABSOLUTE_PATH);

    //////////  This is only for DEBUG/DEV Environment.               ////////////////////////////
    //////////  Update the purge strategy to purge by log file count. ////////////////////////////
    const auto purgeCount = Poco::Environment::get("MAX_LOG_PURGE_COUNT", "10");
    if (!purgeCount.empty())
    {
        fileChannel->setProperty("purgeCount", purgeCount);
    }
    //////////////////////////////////////////////////////////////////////////////////////////////

    // console channel
#if !defined(_DEBUG) || defined(NDEBUG)
    Poco::AutoPtr<Poco::AsyncChannel> asyncChannel(new Poco::AsyncChannel(fileChannel));
#else
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
#endif

    Poco::AutoPtr<Poco::FormattingChannel> formattingChannel(
        new Poco::FormattingChannel(patternFormatter, asyncChannel));

    std::vector<std::string> logNames;
    Poco::Logger::root().names(logNames);
    for (auto it = logNames.begin(); it != logNames.end(); ++it)
    {
        Poco::Logger::root().setChannel(*it, formattingChannel);
    }
    Poco::Logger::root().create("LoadManager", formattingChannel);
#if !defined(_DEBUG) || defined(NDEBUG)
    Poco::Logger::root().setLevel(Poco::Message::PRIO_INFORMATION);
#else
    Poco::Logger::root().setLevel(Poco::Message::PRIO_DEBUG);
#endif
}

void InitLogger()
{
    static std::once_flag flag;
    std::call_once(flag, _InitLogger);
}
