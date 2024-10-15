#pragma once
#include <crow.h>

#include "code/ErrorCode.h"

inline void SuccessResponse(crow::response& res, const std::string& message = "Success",
                            const crow::json::wvalue& data = crow::json::wvalue()) {
    crow::json::wvalue response_body;
    response_body["code"] = ErrorCode::SUCCESS;
    response_body["message"] = message;
    // 合并 response_body 与 data
    if (!data.dump().empty()) {
        crow::json::rvalue data_rvalue = crow::json::load(data.dump());
        for (const auto& item : data_rvalue) {
            response_body[item.key()] = item;
        }
    }
    res.code = 200;
    res.set_header("Content-Type", "application/json");
    res.write(response_body.dump());
    res.end();
}

inline void FailResponse(crow::response& res, ErrorCode code, const std::string& message,
                         const crow::json::wvalue& data = crow::json::wvalue()) {
    crow::json::wvalue response_body;
    response_body["code"] = code;
    response_body["message"] = message;

    if (!data.dump().empty()) {
        response_body["data"] = crow::json::load(data.dump());
    }

    int http_status_code = 500;  // 默认状态码为500
    int code_value = static_cast<int>(code);
    if (code_value >= 100000 && code_value <= 500999) {
        http_status_code = code_value / 1000;  // 取前三位作为HTTP状态码
    }
    if (http_status_code < 100 || http_status_code > 599) {
        http_status_code = 400;
    }
    res.code = http_status_code;
    res.set_header("Content-Type", "application/json");
    res.write(response_body.dump());
    res.end();
}