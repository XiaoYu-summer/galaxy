#pragma once
#include <crow.h>
#include <nlohmann/json.hpp>
#include "code/ErrorCode.h"

inline void SuccessResponse(crow::response& res, const std::string& message = "Success", const nlohmann::json& data = nlohmann::json::object()) {
    crow::json::wvalue response_body;
    response_body["code"] = ErrorCode::SUCCESS;
    response_body["message"] = message;

    if (!data.empty()) {
        response_body["data"] = crow::json::load(data.dump());
    }
    res.code = 200;
    res.set_header("Content-Type", "application/json");
    res.write(response_body.dump());
    res.end();
}

inline void FailResponse(crow::response& res, ErrorCode code, const std::string& message, const nlohmann::json& data = nlohmann::json::object()) {
    crow::json::wvalue response_body;
    response_body["code"] = code;
    response_body["message"] = message;

    if (!data.empty()) {
        response_body["data"] = crow::json::load(data.dump());
    }

    int http_status_code = 500; // 默认状态码为500
    int code_value = static_cast<int>(code);
    if (code_value >= 100000 && code_value <= 500999) {
        http_status_code = code_value / 1000; // 取前三位作为HTTP状态码
    }
    if (http_status_code < 100 || http_status_code > 599) {
        http_status_code = 400;
    }
    res.code = http_status_code;
    res.set_header("Content-Type", "application/json");
    res.write(response_body.dump());
    res.end();
}