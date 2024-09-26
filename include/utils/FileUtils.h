#pragma once
#include <crow.h>

#include <string>
namespace FileUtils {

std::string Save(const std::string& file, const std::string& path);
std::string GetPairFileNameFull(const std::unordered_map<std::string, std::string>& map);
bool CompareMD5(const std::string& file, const std::string& md5);

bool CheckFileFormat(const std::string& filename, const std::string& format = ".tar.gz");
// 解压.tar.gz文件
void ExtractTarGz(const std::string& filename, const std::string& dest_dir);
}  // namespace FileUtils