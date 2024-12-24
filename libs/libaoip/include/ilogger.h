#pragma once
#include <string>

namespace aoip {

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

class ILogger {
   public:
    virtual ~ILogger() = default;
    virtual void debug(const std::string& message) = 0;
    virtual void info(const std::string& message) = 0;
    virtual void warn(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
};
}  // namespace aoip