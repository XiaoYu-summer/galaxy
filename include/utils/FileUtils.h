#pragma once
#include <crow.h>
#include <string>

namespace FileUtils {
    std::string Save(const std::string& file, const std::string& path);
    std::string GetPairFileNameFull(const std::unordered_map<std::string, std::string>& map);
    bool CompareMD5(const std::string& file, const std::string& md5);
}