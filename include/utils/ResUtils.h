#pragma once
#include <crow.h>
#include <Poco/File.h>
#include <Poco/StreamCopier.h>
#include <Poco/FileStream.h>
#include "code/ErrorCode.h"

inline void SuccessResponse(crow::response& response, const std::string& message,
                            const crow::json::wvalue& data = crow::json::wvalue()) {
    crow::json::wvalue responseBody;
    responseBody["code"] = static_cast<int>(ErrorCode::SUCCESS);
    responseBody["message"] = message;

    if (data.t() != crow::json::type::Null) {
        responseBody["data"] = crow::json::load(data.dump());
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

    if (data.t() != crow::json::type::Null) {
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

inline void ResponseWithFile(crow::response& response, const char* newFileName,
                            const Poco::File& downloadedFile) {
    if (!downloadedFile.exists()) {
        return FailResponse(response, ErrorCode::FILE_NOT_FOUND, "\"" + downloadedFile.path() + "\" not found");
    }

    Poco::FileInputStream fis(downloadedFile.path());
    std::ostringstream logContent;
    Poco::StreamCopier::copyStream(fis, logContent);
    fis.close();

    response.code = 200;
    response.set_header("Content-Type", "text/plain");
    char strBuffer[64];
    std::snprintf(strBuffer, sizeof(strBuffer), "attachment; filename=\"%s\"",newFileName);
    response.set_header("Content-Disposition", strBuffer);
    response.body = logContent.str();
    response.end();
}

inline auto HandleFileRetrievalReq = [](crow::response &response, const std::string& filepath, const auto &&handler) {
    Poco::File pocoFile(filepath);
    if (!pocoFile.exists()) {
        return FailResponse(response, ErrorCode::FILE_NOT_FOUND,"\"" + filepath + "\" not found");
    }
    if (!pocoFile.isFile()) {
        return FailResponse(response, ErrorCode::UNKNOWN_ERROR, "\"" + filepath + "\" is not a regular file");
    }
    handler(response, pocoFile);
};
