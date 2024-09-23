#include "services/upgrade/ServiceUpgradeService.h"

namespace ServiceUpgradeService {
void Upgrade(const std::string& file, const std::string& file_name) {
    // 使用tar命令进行解压
    std::string tar = "tar -zxvf " + file + " -C /usr/bin/galaxy-series";
    system(tar.c_str());
    // 将 /usr/bin/galaxy-series 下的最新的文件夹 创建动态链接到 /usr/bin/galaxy-series/galaxy
    std::string link = "rm -rf /usr/bin/galaxy-series/galaxy && ln -s /usr/bin/galaxy-series/" + file_name +
                       " /usr/bin/galaxy-series/galaxy";
    system(link.c_str());
    // 重启应用
    std::string restart = "/etc/init.d/S99galaxy restart";
    system(restart.c_str());
}
}  // namespace ServiceUpgradeService