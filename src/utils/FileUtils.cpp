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
// 检查文件是否为.tar.gz格式
bool CheckFileFormat(const std::string &filename, const std::string &format) {
    return boost::algorithm::ends_with(filename, format);
}

// 解压.tar.gz文件
void ExtractTarGz(const std::string &filename, const std::string &dest_dir) {
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    // 设置解压标志
    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    // 打开归档文件
    a = archive_read_new();
    archive_read_support_format_tar(a);
    archive_read_support_filter_gzip(a);
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);

    if ((r = archive_read_open_filename(a, filename.c_str(), 10240))) {
        throw std::runtime_error("Could extract file");
    }

    // 逐个解压文件
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        // 设置解压路径
        const char *currentFile = archive_entry_pathname(entry);
        std::string fullOutputPath = dest_dir + "/" + currentFile;
        archive_entry_set_pathname(entry, fullOutputPath.c_str());

        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK) {
            std::cerr << archive_error_string(ext) << std::endl;
        } else {
            const void *buff;
            size_t size;
            la_int64_t offset;

            while (true) {
                r = archive_read_data_block(a, &buff, &size, &offset);
                if (r == ARCHIVE_EOF) {
                    break;
                }
                if (r < ARCHIVE_OK) {
                    std::cerr << archive_error_string(a) << std::endl;
                    break;
                }
                r = archive_write_data_block(ext, buff, size, offset);
                if (r < ARCHIVE_OK) {
                    std::cerr << archive_error_string(ext) << std::endl;
                    break;
                }
            }
        }
        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK) {
            std::cerr << archive_error_string(ext) << std::endl;
        }
    }

    // 关闭归档文件
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
}

}  // namespace FileUtils