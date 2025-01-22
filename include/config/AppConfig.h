#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "models/AppInfo.h"
#include "nlohmann/json.hpp"

extern const char APP_CONFIG_NAME[];
extern const std::string APP_CONFIG_ABSOLUTE_PATH;

class AppConfig {
   public:
    static AppConfig& Instance() {
        static AppConfig instance;
        return instance;
    }

    bool LoadConfig(const std::string& configPath);
    std::shared_ptr<AppInfo> GetAppInfo(const std::string& appId);
    std::string GetPublicKey(const std::string& appId);
    std::string GetPrivateKey(const std::string& appId);

   private:
    AppConfig() = default;
    std::unordered_map<std::string, std::shared_ptr<AppInfo>> appInfoMap_;
};
