#pragma once
#include <crow.h>
#include <spdlog/spdlog.h>

#include <string>

#include "spdlog/sinks/rotating_file_sink.h"

class FileLogHandler : public crow::ILogHandler {
   public:
    FileLogHandler();
    void log(std::string message, crow::LogLevel level) override;

   private:
    std::shared_ptr<spdlog::logger> logger_;
};