#include <archive.h>
#include <archive_entry.h>
#include <openssl/evp.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "utils/FileUtils.h"

namespace FileUtils {

void CreateDirectory(const std::string& dirPath) {
    try {
        boost::filesystem::path folderPath = boost::filesystem::path(dirPath).parent_path();
        if (!folderPath.empty() && !boost::filesystem::exists(folderPath)) {
            boost::filesystem::create_directories(folderPath);
        }
    } catch (const boost::filesystem::filesystem_error& error) {
        std::cerr << "Error creating directory: " << error.what() << std::endl;
    }
}

std::string CalculateMd5(const std::string& data) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLen;

    EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_md5(), nullptr);
    EVP_DigestUpdate(context, data.c_str(), data.size());
    EVP_DigestFinal_ex(context, digest, &digestLen);
    EVP_MD_CTX_free(context);

    std::ostringstream md5Stream;
    for (unsigned int i = 0; i < digestLen; ++i) {
        md5Stream << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }

    return md5Stream.str();
}

std::string GetFullFileName(const std::unordered_map<std::string, std::string>& paramMap) {
    for (const auto& param : paramMap) {
        if (param.first == "filename") {
            return param.second;
        }
    }
    return "";
}

std::string SaveFile(const std::string& fileContent, const std::string& filePath) {
    try {
        CreateDirectory(filePath);
        std::ofstream outFile(filePath, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Failed to open file for writing: " + filePath);
        }
        outFile.write(fileContent.c_str(), fileContent.size());
        outFile.close();
        return filePath;
    } catch (const std::exception& error) {
        throw std::runtime_error("Error saving file: " + std::string(error.what()));
    }
}

bool VerifyMD5(const std::string& fileContent, const std::string& md5) {
    std::string calculatedMd5 = CalculateMd5(fileContent);
    return calculatedMd5 == md5;
}

bool CheckFileFormat(const std::string& fileName, const std::string& format) {
    return boost::algorithm::ends_with(fileName, format);
}

void ExtractTarGzFile(const std::string& fileName, const std::string& destDir) {
    struct archive* reader = archive_read_new();
    struct archive* writer = archive_write_disk_new();
    struct archive_entry* entry;
    int result;

    // 配置读取器
    archive_read_support_filter_gzip(reader);
    archive_read_support_format_tar(reader);

    // 配置写入器
    archive_write_disk_set_options(
        writer, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);
    archive_write_disk_set_standard_lookup(writer);

    // 打开源文件
    result = archive_read_open_filename(reader, fileName.c_str(), 10240);
    if (result != ARCHIVE_OK) {
        throw std::runtime_error("Failed to open archive: " + std::string(archive_error_string(reader)));
    }

    try {
        // 创建目标目录
        boost::filesystem::create_directories(destDir);

        // 解压文件
        while (archive_read_next_header(reader, &entry) == ARCHIVE_OK) {
            const char* currentFile = archive_entry_pathname(entry);
            std::string fullOutputPath = destDir + "/" + currentFile;
            archive_entry_set_pathname(entry, fullOutputPath.c_str());

            result = archive_write_header(writer, entry);
            if (result != ARCHIVE_OK) {
                throw std::runtime_error("Failed to write header: " + std::string(archive_error_string(writer)));
            }

            const void* buff;
            size_t size;
            la_int64_t offset;

            while (archive_read_data_block(reader, &buff, &size, &offset) == ARCHIVE_OK) {
                result = archive_write_data_block(writer, buff, size, offset);
                if (result != ARCHIVE_OK) {
                    throw std::runtime_error("Failed to write data: " + std::string(archive_error_string(writer)));
                }
            }

            result = archive_write_finish_entry(writer);
            if (result != ARCHIVE_OK) {
                throw std::runtime_error("Failed to finish entry: " + std::string(archive_error_string(writer)));
            }
        }

        // 清理资源
        archive_read_close(reader);
        archive_read_free(reader);
        archive_write_close(writer);
        archive_write_free(writer);
    } catch (...) {
        // 确保在发生异常时释放资源
        archive_read_close(reader);
        archive_read_free(reader);
        archive_write_close(writer);
        archive_write_free(writer);
        throw;
    }
}

}  // namespace FileUtils