#pragma once
#include <string>
#include <crow/query_string.h>
#include "code/ErrorCode.h"

inline ErrorCode ParseQueryParams(
    const crow::query_string& url_params, 
    const std::string& key,
    std::string& value_out,
    std::string& error_message) {
    
    const char* value_str = url_params.get(key);
    if (!value_str) {
        error_message = "'" + key + "' is required";
        return ErrorCode::PARAMS_ERROR;
    }

    if (strlen(value_str) == 0) {
        error_message = "'" + key + "' is invalid";
        return ErrorCode::PARAMS_ERROR;
    }
    value_out = value_str;
    return ErrorCode::SUCCESS;
}