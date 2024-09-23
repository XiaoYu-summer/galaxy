#include <boost/process.hpp>

#include "services/upgrade/ServiceUpgradeService.h"

namespace bp = boost::process;

namespace ServiceUpgradeService {
void Upgrade(const std::string& file, const std::string& file_name) {
    try {
        // 使用tar命令进行解压
        std::string tar = "tar -zxvf " + file + " -C /usr/bin/galaxy-series";
        bp::system(tar);

        // 将 /usr/bin/galaxy-series 下的最新的文件夹 创建动态链接到 /usr/bin/galaxy-series/galaxy
        std::string link = "rm -rf /usr/bin/galaxy-series/galaxy && ln -s /usr/bin/galaxy-series/" + file_name +
                           " /usr/bin/galaxy-series/galaxy";
        bp::system(link);

        // 重启应用
        std::string restart = "/etc/init.d/S99galaxy restart";
        bp::system(restart);
    } catch (const std::exception& e) {
        CROW_LOG_ERROR << "Error during upgrade: " << e.what();
    }
}
}  // namespace ServiceUpgradeService