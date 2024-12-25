#include <boost/filesystem.hpp>
#include <iostream>

#include "libaoip/include/spdlog_adapter.h"
#include "utils/FileUtils.h"
#include "utils/LogUtils.h"

FileLogHandler::FileLogHandler() {
    try {
        auto maxSize = 1048576 * 20;
        auto maxFiles = 10;
        boost::filesystem::path appCurrentPath = boost::filesystem::current_path();
        std::cout << "Current path: " << appCurrentPath << std::endl;
#ifdef NDEBUG
        boost::filesystem::path logPath = appCurrentPath.parent_path() / "logs" / "galaxy-server.log";
#else
        boost::filesystem::path logPath = appCurrentPath / "logs" / "galaxy-server.log";
#endif
        std::cout << "Log path: " << logPath << std::endl;
        logger_ = spdlog::rotating_logger_mt("galaxy_logger", logPath.string(), maxSize, maxFiles);
        logger_->set_pattern("%Y-%m-%d %H:%M:%S[%l] %v");
        logger_->info("Log init success!\n current path: " + appCurrentPath.string() +
                      "\n log path: " + logPath.string());
        logger_->flush();

        auto aoipLogger = std::make_shared<aoip::SpdLogAdapter>(logger_);
        aoip::LogManager::GetInstance().SetLogger(aoipLogger);

    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
}

void FileLogHandler::log(std::string message, crow::LogLevel level) {
#ifdef NDEBUG
    std::cout << message << std::endl;
#endif
    switch (level) {
        case crow::LogLevel::Debug:
            logger_->debug(message);
            break;
        case crow::LogLevel::Info:
            logger_->info(message);
            break;
        case crow::LogLevel::Warning:
            logger_->warn(message);
            break;
        case crow::LogLevel::Error:
            logger_->error(message);
            break;
        case crow::LogLevel::Critical:
            logger_->critical(message);
            break;
    }
    logger_->flush();
}