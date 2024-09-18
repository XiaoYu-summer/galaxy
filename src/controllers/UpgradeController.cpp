#include "controllers/UpgradeController.h"
#include "utils/FileUtils.h"

void UpgradeController::InitRoutes(crow::SimpleApp& app) {
    /**
     * 生成CROW Route 对应的代码
     * 路径 /resource/v1/update
     * 方法 POST
     * body 参数 form-data
     *  - file 文件 file
     *  - type 类型 "view" "service" "fireware"
     *  - md5 文件md5 string
     * 返回值 void
     */
    CROW_ROUTE(app, "/resource/v1/update")
    .methods("POST"_method)
    ([&](const crow::request& req) {
        std::string upname;
        crow::multipart::message form_data(req);
        // 获取文件
        auto file = form_data.get_part_by_name("file");
        // 获取类型
        auto type = form_data.get_part_by_name("type");
        // 获取md5
        auto md5 = form_data.get_part_by_name("md5");
        auto shead = file.get_header_object("content-disposition");
        bool isSame = FileUtils::CompareMD5(file.body, md5.body);
        if (shead.params.size() != 0)
        {
            upname = FileUtils::GetPairFileNameFull(shead.params);
        }
        std::string saveFilePath = upname;
        FileUtils::Save(file.body, saveFilePath);
        return upname;
    });
}
