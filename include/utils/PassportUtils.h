#include <crow/logging.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

namespace PassportUtils {

/**
 * @brief 获取账号密码文件路径
 */
inline boost::filesystem::path GetAccountPasswordFilePath() {
    boost::filesystem::path appCurrentPath = boost::filesystem::current_path();
#ifdef NDEBUG
    boost::filesystem::path filePath = appCurrentPath.parent_path() / "passport" / "passport.json";
#else
    boost::filesystem::path filePath = appCurrentPath / "passport" / "passport.json";
#endif
    return filePath;
}

/**
 * @brief 写入账号密码到文件
 */
inline void WriteAccountPasswordToFile(const std::string& account, const std::string& password,
                                       const std::string& filePath = GetAccountPasswordFilePath().string()) {
    boost::filesystem::path filePathObj(filePath);
    boost::filesystem::path dirPath = filePathObj.parent_path();
    if (!boost::filesystem::exists(dirPath)) {
        if (!boost::filesystem::create_directories(dirPath)) {
            CROW_LOG_ERROR << "Failed to create directories: " << dirPath;
            return;
        }
    }
    std::ofstream outFile(filePath);
    if (!outFile) {
        CROW_LOG_ERROR << "Failed to open file for writing: " << filePath;
        return;
    }
    outFile << R"({"account": ")" << account << R"(", "password": ")" << password << R"("})";
    outFile.close();
}

/**
 * @brief 读取账号密码文件内容并返回 account 与 password
 */
inline std::tuple<std::string, std::string> ReadAccountPasswordFromFile() {
    std::string accountFilePath = GetAccountPasswordFilePath().string();
    std::ifstream inFile(accountFilePath);
    if (!inFile) {
        CROW_LOG_ERROR << "Failed to open file for reading: " << accountFilePath;
        return std::make_tuple("", "");
    }
    std::string fileContent((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    crow::json::rvalue accountPassword = crow::json::load(fileContent);
    if (!accountPassword) {
        CROW_LOG_ERROR << "Failed to parse JSON from file: " << accountFilePath;
        return std::make_tuple("", "");
    }

    return std::make_tuple(accountPassword["account"].s(), accountPassword["password"].s());
}

/**
 * @brief 初始化账号密码文件
 */
inline void InitAccountPasswordFile() {
    std::string accountFilePath = GetAccountPasswordFilePath().string();
    if (!boost::filesystem::exists(accountFilePath)) {
        WriteAccountPasswordToFile("admin", "admin");
    }
}

}  // namespace PassportUtils