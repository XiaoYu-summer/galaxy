#include "utils/RsaUtils.h"

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

EVP_PKEY* RsaUtils::LoadPublicKey(const std::string& publicKey) {
    BIO* bio = BIO_new_mem_buf(publicKey.c_str(), -1);
    if (!bio) {
        return nullptr;
    }

    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    return pkey;
}

EVP_PKEY* RsaUtils::LoadPrivateKey(const std::string& privateKey) {
    BIO* bio = BIO_new_mem_buf(privateKey.c_str(), -1);
    if (!bio) {
        return nullptr;
    }

    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    return pkey;
}

std::vector<unsigned char> RsaUtils::Base64Decode(const std::string& base64Text) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    
    BIO* bmem = BIO_new_mem_buf(base64Text.c_str(), base64Text.length());
    bmem = BIO_push(b64, bmem);
    
    std::vector<unsigned char> buffer(base64Text.length());
    int decodedLength = BIO_read(bmem, buffer.data(), buffer.size());
    BIO_free_all(bmem);
    
    if (decodedLength > 0) {
        buffer.resize(decodedLength);
        return buffer;
    }
    return std::vector<unsigned char>();
}

std::string RsaUtils::Base64Encode(const unsigned char* buffer, size_t length) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    
    BIO* bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    
    BIO_write(b64, buffer, length);
    BIO_flush(b64);
    
    BUF_MEM* bptr;
    BIO_get_mem_ptr(b64, &bptr);
    
    std::string base64Text(bptr->data, bptr->length);
    BIO_free_all(b64);
    return base64Text;
}

std::string RsaUtils::Encrypt(const std::string& data, const std::string& publicKey) {
    EVP_PKEY* pkey = LoadPublicKey(publicKey);
    if (!pkey) {
        return "";
    }

    // 创建加密上下文
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        EVP_PKEY_free(pkey);
        return "";
    }

    // 初始化加密操作
    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return "";
    }

    // 设置填充模式
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return "";
    }

    // 获取输出缓冲区大小
    size_t outlen;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen, 
        reinterpret_cast<const unsigned char*>(data.c_str()), data.length()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return "";
    }

    // 执行加密
    std::vector<unsigned char> encrypted(outlen);
    if (EVP_PKEY_encrypt(ctx, encrypted.data(), &outlen,
        reinterpret_cast<const unsigned char*>(data.c_str()), data.length()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return "";
    }

    // Base64编码
    std::string base64Result = Base64Encode(encrypted.data(), outlen);

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    return base64Result;
}

std::string RsaUtils::Decrypt(const std::string& encryptedData, const std::string& privateKey) {
    EVP_PKEY* pkey = LoadPrivateKey(privateKey);
    if (!pkey) {
        return "";
    }

    // 创建解密上下文
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        EVP_PKEY_free(pkey);
        return "";
    }

    // 初始化解密操作
    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return "";
    }

    // 设置填充模式
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return "";
    }

    // Base64解码
    std::vector<unsigned char> decoded = Base64Decode(encryptedData);
    if (decoded.empty()) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return "";
    }

    // 获取输出缓冲区大小
    size_t outlen;
    if (EVP_PKEY_decrypt(ctx, nullptr, &outlen, decoded.data(), decoded.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return "";
    }

    // 执行解密
    std::vector<unsigned char> decrypted(outlen);
    if (EVP_PKEY_decrypt(ctx, decrypted.data(), &outlen, decoded.data(), decoded.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return "";
    }

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    return std::string(reinterpret_cast<char*>(decrypted.data()), outlen);
}
