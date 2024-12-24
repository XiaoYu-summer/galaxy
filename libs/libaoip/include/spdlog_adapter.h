#pragma once
#include <spdlog/spdlog.h>

#include <memory>

#include "ilogger.h"
#include "logger.h"

namespace aoip {

class SpdLogAdapter : public ILogger {
   public:
    explicit SpdLogAdapter(std::shared_ptr<spdlog::logger> logger) : logger_(logger) {}

    void debug(const std::string& message) override { logger_->debug(message); }

    void info(const std::string& message) override { logger_->info(message); }

    void warn(const std::string& message) override { logger_->warn(message); }

    void error(const std::string& message) override { logger_->error(message); }

   private:
    std::shared_ptr<spdlog::logger> logger_;
};

}  // namespace aoip
