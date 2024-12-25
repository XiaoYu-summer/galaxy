#pragma once
#include <crow.h>

#include "code/ErrorCode.h"

inline void SuccessResponse(crow::response& response, const std::string& message = "Success",
                            const crow::json::wvalue& data = crow::json::wvalue()) {
    crow::json::wvalue responseBody;
    responseBody["code"] = static_cast<int>(ErrorCode::SUCCESS);
    responseBody["message"] = message;

    if (!data.dump().empty()) {
        crow::json::rvalue dataValue = crow::json::load(data.dump());
        for (const auto& item : dataValue) {
            responseBody[item.key()] = item;
        }
    }

    response.code = 200;
    response.set_header("Content-Type", "application/json");
    response.write(responseBody.dump());
    response.end();
}

inline void FailResponse(crow::response& response, ErrorCode errorCode, const std::string& message,
                         const crow::json::wvalue& data = crow::json::wvalue()) {
    crow::json::wvalue responseBody;
    responseBody["code"] = static_cast<int>(errorCode);
    responseBody["message"] = message;

    if (!data.dump().empty()) {
        responseBody["data"] = crow::json::load(data.dump());
    }

    int httpStatusCode = 500;  // Default to 500 Internal Server Error
    int errorCodeValue = static_cast<int>(errorCode);
    if (errorCodeValue >= 100000 && errorCodeValue <= 500999) {
        httpStatusCode = errorCodeValue / 1000;  // Extract HTTP status code from error code
    }
    if (httpStatusCode < 100 || httpStatusCode > 599) {
        httpStatusCode = 400;  // Default to 400 Bad Request if invalid
    }

    response.code = httpStatusCode;
    response.set_header("Content-Type", "application/json");
    response.write(responseBody.dump());
    response.end();
}