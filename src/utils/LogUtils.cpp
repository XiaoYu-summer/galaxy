#include <boost/filesystem.hpp>
#include <iostream>

#include "libaoip/include/spdlog_adapter.h"
#include "utils/FileUtils.h"
#include "utils/LogUtils.h"

FileLogHandler::FileLogHandler() {
    try {
        auto max_size = 1048576 * 20;
        auto max_files = 10;
        boost::filesystem::path app_current_path = boost::filesystem::current_path();
        std::cout << "Current path: " << app_current_path << std::endl;
#ifdef NDEBUG
        // 拼接日志文件路径
        boost::filesystem::path log_path = app_current_path.parent_path() / "logs" / "galaxy-server.log";
#else
        // 拼接日志文件路径
        boost::filesystem::path log_path = app_current_path / "logs" / "galaxy-server.log";
#endif
        std::cout << "Log path: " << log_path << std::endl;
        m_logger = spdlog::rotating_logger_mt("galaxy_logger", log_path.string(), max_size, max_files);
        m_logger->set_pattern("%Y-%m-%d %H:%M:%S[%l] %v");
        m_logger->info("Log init success!\n current path: " + app_current_path.string() +
                       "\n log path: " + log_path.string());
        m_logger->flush();

        // 设置 libaoip 使用主项目的日志器
        auto aoip_logger = std::make_shared<aoip::SpdLogAdapter>(m_logger);
        aoip::LogManager::instance().setLogger(aoip_logger);

    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
}

void FileLogHandler::log(std::string message, crow::LogLevel level) {
#ifdef NDEBUG
    std::cout << message << std::endl;
#endif
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