#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include "services/upgrade/SystemUpgradeService.h"
#include "utils/FileUtils.h"

namespace SystemUpgradeService {

void UpgradeSystem(const std::string& filePath, const std::string& fileName) {
    try {
        /**
         * 校验文件格式
         */
        if (!FileUtils::CheckFileFormat(fileName, ".img.tar.gz")) {
            throw std::runtime_error("File format error");
        }
#if !defined(_DEBUG) || defined(NDEBUG)
        FileUtils::ExtractTarGzFile(filePath, "/userdata");
        // 将解压的文件重命名
        boost::filesystem::path sourcePath = "/userdata/" + fileName;
        boost::filesystem::path destPath = "/userdata/update.img";
        boost::filesystem::rename(sourcePath, destPath);
        // 重启应用
        std::string restartCmd =
            "updateEngine --image_url=/userdata/update.img --misc=update --savepath=/userdata/update.img --reboot &";
        boost::process::child restartProcess(restartCmd, boost::process::std_out > stdout,
                                             boost::process::std_err > stderr);
        restartProcess.detach();  // 使子进程在后台运行
#else
        std::cout << "Build type: Debug, System Not Upgrade" << std::endl;
#endif
    } catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error During System Upgrade: " << e.what();
        //  将错误继续往外抛
        throw e;
    }
}

}  // namespace SystemUpgradeService