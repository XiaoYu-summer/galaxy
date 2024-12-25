#pragma once
#include <string>

namespace aoip {

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

class Logger {
   public:
    virtual ~Logger() = default;
    virtual void Debug(const std::string& message) = 0;
    virtual void Info(const std::string& message) = 0;
    virtual void Warn(const std::string& message) = 0;
    virtual void Error(const std::string& message) = 0;
};

}  // namespace aoip