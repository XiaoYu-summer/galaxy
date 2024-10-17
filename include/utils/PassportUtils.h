#include <crow/logging.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

namespace PassportUtils {

/**
 * @brief 获取账号密码文件路径
 */
inline boost::filesystem::path GetAccountPasswordFile() {
    boost::filesystem::path app_current_path = boost::filesystem::current_path();
#ifdef NDEBUG
    boost::filesystem::path file = app_current_path.parent_path() / "passport" / "passport.json";
#else
    boost::filesystem::path file = app_current_path / "passport" / "passport.json";
#endif
    return file;
}

/**
 * @brief 写入账号密码到文件
 */
inline void WriteAccountPassword(const std::string& account, const std::string& password,
                                 const std::string& file = GetAccountPasswordFile().string()) {
    boost::filesystem::path file_path(file);
    boost::filesystem::path dir = file_path.parent_path();
    if (!boost::filesystem::exists(dir)) {
        if (!boost::filesystem::create_directories(dir)) {
            CROW_LOG_ERROR << "Failed to create directories: " << dir;
            return;
        }
    }
    std::ofstream ofs(file);
    if (!ofs) {
        CROW_LOG_ERROR << "Write Account Password Failed to open file: " << file;
        return;
    }
    // 写入数据
    ofs << R"({"account": ")" << account << R"(", "password": ")" << password << R"("})";
    ofs.close();
}

/**
 * @brief 读取 GetAccountPasswordFile 所返回的JSON文件内容
 * 并返回 account 与 password
 */
inline std::pair<std::string, std::string> ReadAccountPassword(
    const std::string& file = GetAccountPasswordFile().string()) {
    std::ifstream ifs(file);
    if (!ifs) {
        CROW_LOG_ERROR << "Read Account Password Failed to open file: " << file;
        return {"", ""};
    }
    std::string fileContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();
    crow::json::rvalue accountPassword = crow::json::load(fileContent);
    return {accountPassword["account"].s(), accountPassword["password"].s()};
}

/**
 * @brief 初始化账号密码文件
 */
inline void InitAccountPasswordFile() {
    auto file = GetAccountPasswordFile();
    // 判断 file 是否存在，不存在则执行写入操作
    CROW_LOG_INFO << "Account password file: " << boost::filesystem::exists(file);
    if (!boost::filesystem::exists(file)) {
        WriteAccountPassword("admin", "21232f297a57a5a743894a0e4a801fc3", file.string());
    }
}

}  // namespace PassportUtils