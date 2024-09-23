#include <initializer_list>
#include <unordered_set>

#include "code/ErrorCode.h"
#include "controllers/UpgradeController.h"
#include "services/upgrade/ServiceUpgradeService.h"
#include "utils/FileUtils.h"
#include "utils/ResUtils.h"

bool ValidateUpgradeParams(crow::multipart::message& form_data) {
    auto file = form_data.get_part_by_name("file");
    auto type = form_data.get_part_by_name("type");
    auto md5 = form_data.get_part_by_name("md5");

    auto shead = file.get_header_object("content-disposition");

    std::unordered_set<std::string> valid_types = {"view", "service", "firmware"};

    return shead.params.size() == 0 || type.body.empty() || md5.body.empty() ||
           valid_types.find(type.body) == valid_types.end();
}

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
        .methods("POST"_method)([&app](const crow::request& req, crow::response& res) {
            std::string file_name;
            crow::multipart::message form_data(req);
            if (ValidateUpgradeParams(form_data)) {
                return FailResponse(res, ErrorCode::PARAMS_ERROR, "params error");
            } else {
                // 获取文件
                auto file = form_data.get_part_by_name("file");
                // 获取类型
                auto type = form_data.get_part_by_name("type");
                // 获取md5
                auto md5 = form_data.get_part_by_name("md5");
                auto shead = file.get_header_object("content-disposition");
                bool isMD5Match = FileUtils::CompareMD5(file.body, md5.body);
                if (isMD5Match) {
                    file_name = FileUtils::GetPairFileNameFull(shead.params);
                    std::string save_file_path = "./upgrades/" + type.body + '/' + file_name;
                    FileUtils::Save(file.body, save_file_path);
                    try {
                        ServiceUpgradeService::Upgrade(save_file_path, file_name);
                        return SuccessResponse(res);
                    } catch (const std::exception& e) {
                        return FailResponse(res, ErrorCode::UPGRADE_ERROR, e.what());
                    }
                } else {
                    return FailResponse(res, ErrorCode::MD5_MISMATCH, "md5 mismatch");
                }
            }
        });
}
