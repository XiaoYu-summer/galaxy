#include "utils/LogUtils.h"

FileLogHandler::FileLogHandler() {
    try {
        auto max_size = 1048576 * 20;
        auto max_files = 10;
        m_logger = spdlog::rotating_logger_mt("galaxy_logger", "logs/galaxy-server.log", max_size, max_files);
        m_logger->set_pattern("%Y-%m-%d %H:%M:%S[%l] %v");
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