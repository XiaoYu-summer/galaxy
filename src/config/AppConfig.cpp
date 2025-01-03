#include <fstream>

#include "config/AppConfig.h"

bool AppConfig::LoadConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json config = nlohmann::json::parse(file);
        for (const auto& appConfig : config["apps"]) {
            auto appInfo = std::make_shared<AppInfo>();
            appInfo->appId = appConfig["appId"].get<std::string>();
            appInfo->publicKey = appConfig["publicKey"].get<std::string>();
            appInfo->privateKey = appConfig.value("privateKey", "");  // 可选
            appInfo->isEnabled = appConfig.value("isEnabled", true);

            appInfoMap_[appInfo->appId] = appInfo;
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::shared_ptr<AppInfo> AppConfig::GetAppInfo(const std::string& appId) {
    auto it = appInfoMap_.find(appId);
    if (it != appInfoMap_.end() && it->second->isEnabled) {
        return it->second;
    }
    return nullptr;
}

std::string AppConfig::GetPublicKey(const std::string& appId) {
    auto appInfo = GetAppInfo(appId);
    return appInfo ? appInfo->publicKey : "";
}

std::string AppConfig::GetPrivateKey(const std::string& appId) {
    auto appInfo = GetAppInfo(appId);
    return appInfo ? appInfo->privateKey : "";
}
