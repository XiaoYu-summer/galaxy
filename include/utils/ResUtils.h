#pragma once
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>
#include <crow.h>

#include "code/ErrorCode.h"
#include "utils/JsonParamsParseHelper.h"

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
    std::snprintf(strBuffer, sizeof(strBuffer), "attachment; filename=\"%s\"", newFileName);
    response.set_header("Content-Disposition", strBuffer);
    response.body = logContent.str();
    response.end();
}

inline auto HandleFileRetrievalReq = [](crow::response& response, const std::string& filepath, const auto&& handler) {
    Poco::File pocoFile(filepath);
    if (!pocoFile.exists()) {
        return FailResponse(response, ErrorCode::FILE_NOT_FOUND, "\"" + filepath + "\" not found");
    }
    if (!pocoFile.isFile()) {
        return FailResponse(response, ErrorCode::UNKNOWN_ERROR, "\"" + filepath + "\" is not a regular file");
    }
    handler(response, pocoFile);
};

template <typename Tuple, std::size_t Index = 0>
ErrorCode ParseJsonParamsRecursive(const crow::json::rvalue& requestBody, Tuple& values, std::string& error_message, const std::vector<std::string>& keys) {
    if constexpr (Index < std::tuple_size_v<Tuple>) {
        if (Index >= keys.size()) {
            return ErrorCode::SUCCESS;
        }
        if (const auto error_code = ParseJsonParams(requestBody, keys[Index], std::get<Index>(values), error_message); error_code != ErrorCode::SUCCESS) {
            return error_code;
        }
        return ParseJsonParamsRecursive<Tuple, Index + 1>(requestBody, values, error_message, keys);
    } else {
        return ErrorCode::SUCCESS;
    }
}

template <typename Handler, typename Tuple, std::size_t... Is>
void CallHandler(Handler&& handler, Tuple& values, crow::response& response, std::index_sequence<Is...>) {
    handler(std::get<Is>(values)..., response);
}

template <typename... Types, typename Handler, typename... Keys>
inline void HandleJsonReqWithParams(const crow::request& request, crow::response& response, const Handler&& handler, const Keys&... keys) {
    using Tuple = std::tuple<Types...>;
    Tuple values;
    std::string error_message;
    const auto requestBody = crow::json::load(request.body);
    if (!requestBody) {
        return FailResponse(response, ErrorCode::JSON_BODY_ERROR, "Invalid JSON");
    }

    std::vector<std::string> keys_vector = {keys...};
    if (const auto error_code = ParseJsonParamsRecursive(requestBody, values, error_message, keys_vector); ErrorCode::SUCCESS != error_code) {
        return FailResponse(response, error_code, error_message);
    }

    CallHandler(handler, values, response, std::index_sequence_for<Types...>{});
}