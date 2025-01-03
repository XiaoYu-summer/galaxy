#pragma once

#include <string>

struct AppInfo {
    std::string appId;
    std::string publicKey;    // RSA public key
    std::string privateKey;   // RSA private key (可选，如果需要服务端签名)
    bool isEnabled;           // APP是否启用
};
