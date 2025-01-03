#pragma once

#include <openssl/evp.h>
#include <string>
#include <vector>

class RsaUtils {
   private:
    static EVP_PKEY* LoadPublicKey(const std::string& publicKey);
    static EVP_PKEY* LoadPrivateKey(const std::string& privateKey);
    static std::vector<unsigned char> Base64Decode(const std::string& base64Text);
    static std::string Base64Encode(const unsigned char* buffer, size_t length);

   public:
    // 新的加密解密方法
    static std::string Encrypt(const std::string& data, const std::string& publicKey);
    static std::string Decrypt(const std::string& encryptedData, const std::string& privateKey);
};
