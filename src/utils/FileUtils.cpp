#include <openssl/evp.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "utils/FileUtils.h"

namespace FileUtils {
void createDirectory(const std::string &path) {
    try {
        boost::filesystem::path folder = boost::filesystem::path(path).parent_path();
        if (!folder.empty() && !boost::filesystem::exists(folder)) {
            boost::filesystem::create_directories(folder);
        }
    } catch (const boost::filesystem::filesystem_error &e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
    }
}
std::string CalculateMD5(const std::string &data) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
    EVP_DigestUpdate(ctx, data.c_str(), data.size());
    EVP_DigestFinal_ex(ctx, digest, &digest_len);
    EVP_MD_CTX_free(ctx);

    std::ostringstream md5Stream;
    for (unsigned int i = 0; i < digest_len; ++i) {
        md5Stream << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }

    return md5Stream.str();
}

std::string GetPairFileNameFull(const std::unordered_map<std::string, std::string> &map) {
    for (auto pair : map) {
        if (pair.first == "filename") {
            return pair.second;
        }
    }
    return "";
}

std::string Save(const std::string &file, const std::string &path) {
    createDirectory(path);
    std::ofstream saveFile;
    saveFile.open(path, std::ofstream::binary);
    saveFile << file;
    saveFile.close();
    return path;
}

bool CompareMD5(const std::string &file, const std::string &md5) {
    std::string calculated_md5 = CalculateMD5(file);
    return calculated_md5 == md5;
}
}  // namespace FileUtils