#include <iostream>
#include "utils/LogUtils.h"
#include "common/LoggerWrapper.h"

DEFINE_FILE_NAME("LogUtils.cpp")

FileLogHandler::FileLogHandler()
    : logger_(Poco::Logger::get("Crow"))
{

}

void FileLogHandler::log(std::string message, crow::LogLevel level) {
#ifdef NDEBUG
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