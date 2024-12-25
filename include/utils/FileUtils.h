#pragma once
#include <crow.h>

#include <string>
namespace FileUtils {

std::string SaveFile(const std::string& fileContent, const std::string& filePath);
std::string GetFullFileName(const std::unordered_map<std::string, std::string>& paramMap);
bool VerifyMD5(const std::string& fileContent, const std::string& md5);

bool CheckFileFormat(const std::string& fileName, const std::string& format = ".tar.gz");
void ExtractTarGzFile(const std::string& fileName, const std::string& destDir);

}  // namespace FileUtils