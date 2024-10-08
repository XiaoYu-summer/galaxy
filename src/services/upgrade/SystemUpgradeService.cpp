#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include "services/upgrade/SystemUpgradeService.h"
#include "utils/FileUtils.h"

namespace SystemUpgradeService {
void Upgrade(const std::string& file, const std::string& file_name) {
    try {
        /**
         * 校验文件格式
         */
        if (!FileUtils::CheckFileFormat(file_name, ".img.tar.gz")) {
            throw std::runtime_error("File format error");
        }
#ifdef NDEBUG
        FileUtils::ExtractTarGz(file, "/userdata");
        // 将解压的文件重命名
        boost::filesystem::path source = "/userdata/" + file_name;
        boost::filesystem::path destination = "/userdata/update.img";
        boost::filesystem::rename(source, destination);
        // 重启应用
        std::string restart =
            "updateEngine --image_url=/userdata/update.img --misc=update --savepath=/userdata/update.img --reboot &";
        boost::process::child c(restart, boost::process::std_out > stdout, boost::process::std_err > stderr);
        c.detach();  // 使子进程在后台运行
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