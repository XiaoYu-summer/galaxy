#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include "services/upgrade/ServiceUpgradeService.h"
#include "utils/FileUtils.h"

namespace ServiceUpgradeService {
void Upgrade(const std::string& file, const std::string& file_name) {
    try {
        /**
         * 校验文件格式
         * 后缀为 .tar.gz
         * 再解压到临时目录 判断文件是否包含 galaxy文件 和 galaxy_xxx_xxx_xxx目录
         */
        if (!FileUtils::CheckFileFormat(file_name)) {
            throw std::runtime_error("File format error");
        }
// 如果是调试就不实际解压文件
#ifdef NDEBUG
        boost::filesystem::path app_current_path = boost::filesystem::current_path();
        FileUtils::ExtractTarGz(file, app_current_path.parent_path().string());
        // 重启应用
        std::string restart = "/etc/init.d/S99galaxy restart";
        boost::process::system(restart);
#else
        std::cout << "Build type: Debug, Not Extract File" << std::endl;
#endif
    } catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error during upgrade: " << e.what();
        //  将错误继续往外抛
        throw e;
    }
}
}  // namespace ServiceUpgradeService