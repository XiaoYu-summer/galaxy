#pragma once
#include <crow.h>
#include <string>
#include "Poco/Logger.h"
#include "Poco/Path.h"

extern const std::string LOG_FILE_ABSOLUTE_PATH;

class FileLogHandler : public crow::ILogHandler {
   public:
    FileLogHandler();
    void log(std::string message, crow::LogLevel level) override;

   private:
    Poco::Logger& logger_;
};

void InitLogger();