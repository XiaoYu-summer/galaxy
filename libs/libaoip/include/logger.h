#pragma once
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

#include "ilogger.h"

namespace aoip {

class DefaultLogger : public Logger {
   public:
    static DefaultLogger& GetInstance() {
        static DefaultLogger logger;
        return logger;
    }

    void SetLogFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_.is_open()) file_.close();
        file_.open(filename, std::ios::app);
    }

    void SetLogLevel(LogLevel level) { level_ = level; }

    void Debug(const std::string& message) override { Log(LogLevel::DEBUG, message); }

    void Info(const std::string& message) override { Log(LogLevel::INFO, message); }

    void Warn(const std::string& message) override { Log(LogLevel::WARNING, message); }

    void Error(const std::string& message) override { Log(LogLevel::ERROR, message); }

   private:
    DefaultLogger() = default;

    void Log(LogLevel level, const std::string& message) {
        if (level < level_) return;

        std::lock_guard<std::mutex> lock(mutex_);

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3)
           << ms << " ";

        switch (level) {
            case LogLevel::DEBUG:
                ss << "[DEBUG] ";
                break;
            case LogLevel::INFO:
                ss << "[INFO] ";
                break;
            case LogLevel::WARNING:
                ss << "[WARN] ";
                break;
            case LogLevel::ERROR:
                ss << "[ERROR] ";
                break;
        }

        ss << message << std::endl;
        std::cout << ss.str();
        if (file_.is_open()) {
            file_ << ss.str();
            file_.flush();
        }
    }

    std::mutex mutex_;
    std::ofstream file_;
    LogLevel level_{LogLevel::INFO};
};

class LogManager {
   public:
    static LogManager& GetInstance() {
        static LogManager manager;
        return manager;
    }

    void SetLogger(std::shared_ptr<Logger> logger) { logger_ = logger; }

    Logger& GetLogger() {
        if (logger_) {
            return *logger_;
        }
        return DefaultLogger::GetInstance();
    }

   private:
    LogManager() = default;
    std::shared_ptr<Logger> logger_;
};

}  // namespace aoip

#define AOIP_LOG_DEBUG(message) aoip::LogManager::GetInstance().GetLogger().Debug(message)
#define AOIP_LOG_INFO(message) aoip::LogManager::GetInstance().GetLogger().Info(message)
#define AOIP_LOG_WARN(message) aoip::LogManager::GetInstance().GetLogger().Warn(message)
#define AOIP_LOG_ERROR(message) aoip::LogManager::GetInstance().GetLogger().Error(message)