#pragma once

#include <memory>
#include <spdlog/spdlog.h>

#include "logger.h"

namespace aoip {

class SpdLogAdapter : public Logger {
   public:
    explicit SpdLogAdapter(std::shared_ptr<spdlog::logger> logger) : logger_(logger) {}

    void Debug(const std::string& message) override { logger_->debug(message); }

    void Info(const std::string& message) override { logger_->info(message); }

    void Warn(const std::string& message) override { logger_->warn(message); }

    void Error(const std::string& message) override { logger_->error(message); }

   private:
    std::shared_ptr<spdlog::logger> logger_;
};

}  // namespace aoip
