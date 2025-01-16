#pragma once
#include <crow.h>
#include <string>
#include "Poco/Logger.h"

class FileLogHandler : public crow::ILogHandler {
   public:
    FileLogHandler();
    void log(std::string message, crow::LogLevel level) override;

   private:
    Poco::Logger& logger_;
};