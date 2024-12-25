#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include "services/upgrade/ServiceUpgradeService.h"
#include "utils/FileUtils.h"

namespace ServiceUpgradeService {

void UpgradeService(const std::string& filePath, const std::string& fileName) {
    try {
        /**
         * 校验文件格式
         * 后缀为 .tar.gz
         * 再解压到临时目录 判断文件是否包含 galaxy文件 和 galaxy_xxx_xxx_xxx目录
         */
        if (!FileUtils::CheckFileFormat(fileName)) {
            throw std::runtime_error("File format error");
        }
// 如果是调试就不实际解压文件
#ifdef NDEBUG
        boost::filesystem::path appCurrentPath = boost::filesystem::current_path();
        FileUtils::ExtractTarGzFile(filePath, appCurrentPath.parent_path().string());

        std::string restartCmd = "sleep 3 && /etc/init.d/S99galaxy restart";
        boost::process::child restartProcess(restartCmd, boost::process::std_out > stdout,
                                             boost::process::std_err > stderr);
        restartProcess.detach();  // 使子进程在后台运行
#else
        std::cout << "Build type: Debug, Not Extract File" << std::endl;
#endif
    } catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error During Service Upgrade: " << e.what();
        //  将错误继续往外抛
        throw e;
    }
}

}  // namespace ServiceUpgradeService