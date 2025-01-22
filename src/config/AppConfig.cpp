#include <fstream>
#include "Poco/Path.h"
#include "config/AppConfig.h"

constexpr const char APP_CONFIG_NAME[] = "app_config.json";
const std::string APP_CONFIG_RELATIVE_PATH = std::string("./config/") + std::string(APP_CONFIG_NAME);
const Poco::Path APP_CONFIG_RELATIVE_POCO = Poco::Path(APP_CONFIG_RELATIVE_PATH);
const Poco::Path APP_CONFIG_ABSOLUTE_POCO = APP_CONFIG_RELATIVE_POCO.absolute();
const std::string APP_CONFIG_ABSOLUTE_PATH = APP_CONFIG_ABSOLUTE_POCO.toString();

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
