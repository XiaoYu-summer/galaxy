#include <iostream>

#include "utils/FileUtils.h"
#include "utils/LogUtils.h"

FileLogHandler::FileLogHandler() {
    try {
        auto max_size = 1048576 * 20;
        auto max_files = 10;
        std::cout << "Current path: " << FileUtils::app_current_path << std::endl;
#ifdef NDEBUG
        // 拼接日志文件路径
        boost::filesystem::path log_path = FileUtils::app_current_path.parent_path() / "logs" / "galaxy-server.log";
#else
        // 拼接日志文件路径
        boost::filesystem::path log_path = FileUtils::app_current_path / "logs" / "galaxy-server.log";
#endif
        std::cout << "Log path: " << log_path << std::endl;
        m_logger = spdlog::rotating_logger_mt("galaxy_logger", log_path.string(), max_size, max_files);
        m_logger->set_pattern("%Y-%m-%d %H:%M:%S[%l] %v");
        m_logger->info("Log init success!\n current path: " + FileUtils::app_current_path.string() +
                       "\n log path: " + log_path.string());
        m_logger->flush();
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
}

void FileLogHandler::log(std::string message, crow::LogLevel level) {
    switch (level) {
        case crow::LogLevel::Debug:
            m_logger->debug(message);
            break;
        case crow::LogLevel::Info:
            m_logger->info(message);
            break;
        case crow::LogLevel::Warning:
            m_logger->warn(message);
            break;
        case crow::LogLevel::Error:
            m_logger->error(message);
            break;
        case crow::LogLevel::Critical:
            m_logger->critical(message);
            break;
    }
    m_logger->flush();
}