#pragma once
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <memory>
#include "ilogger.h"

namespace aoip {

class DefaultLogger : public ILogger {
public:
    static DefaultLogger& instance() {
        static DefaultLogger logger;
        return logger;
    }

    void setLogFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_.is_open()) file_.close();
        file_.open(filename, std::ios::app);
    }

    void setLogLevel(LogLevel level) { level_ = level; }

    void debug(const std::string& message) override {
        log(LogLevel::DEBUG, message);
    }

    void info(const std::string& message) override {
        log(LogLevel::INFO, message);
    }

    void warn(const std::string& message) override {
        log(LogLevel::WARNING, message);
    }

    void error(const std::string& message) override {
        log(LogLevel::ERROR, message);
    }

private:
    DefaultLogger() = default;

    void log(LogLevel level, const std::string& message) {
        if (level < level_) return;

        std::lock_guard<std::mutex> lock(mutex_);

        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << " [" << getLevelString(level) << "] "
           << message << std::endl;

        if (file_.is_open()) {
            file_ << ss.str();
            file_.flush();
        }

        std::cout << ss.str();
    }

    const char* getLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARNING:
                return "WARN";
            case LogLevel::ERROR:
                return "ERROR";
            default:
                return "UNKNOWN";
        }
    }

    std::mutex mutex_;
    std::ofstream file_;
    LogLevel level_{LogLevel::INFO};
};

// 全局日志管理器
class LogManager {
public:
    static LogManager& instance() {
        static LogManager manager;
        return manager;
    }

    void setLogger(std::shared_ptr<ILogger> logger) {
        logger_ = logger;
    }

    ILogger& getLogger() {
        if (logger_) {
            return *logger_;
        }
        return DefaultLogger::instance();
    }

private:
    LogManager() = default;
    std::shared_ptr<ILogger> logger_;
};

} // namespace aoip

// 便捷的日志宏
#define AOIP_LOG_DEBUG(message) aoip::LogManager::instance().getLogger().debug(message)
#define AOIP_LOG_INFO(message) aoip::LogManager::instance().getLogger().info(message)
#define AOIP_LOG_WARN(message) aoip::LogManager::instance().getLogger().warn(message)
#define AOIP_LOG_ERROR(message) aoip::LogManager::instance().getLogger().error(message)