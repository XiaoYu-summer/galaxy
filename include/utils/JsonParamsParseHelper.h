#pragma once
#include <string>
#include <vector>
#include <crow/json.h>
#include "code/ErrorCode.h"

inline ErrorCode ParseJsonParams(
    const crow::json::rvalue& request_body, 
    const std::string& key,
    const crow::json::type t, 
    crow::json::rvalue& value_out, 
    std::string& error_message) {
    if (!request_body.has(key)) {
        error_message = "'" + key + "' is required";
        return ErrorCode::PARAMS_ERROR;
    }
    if (request_body[key].t() != t) {
        error_message = "'" + key + "' is invalid";
        return ErrorCode::PARAMS_ERROR;
    }
    value_out = request_body[key];
    return ErrorCode::SUCCESS;
}

template <typename T>
inline ErrorCode ParseJsonParams(
    const crow::json::rvalue& request_body, 
    const std::string& key,
    T& value_out, 
    std::string& error_message) {
    error_message = "This feature is unimplemented";
    return ErrorCode::UNKNOWN_ERROR;
}

template <>
inline ErrorCode ParseJsonParams(
    const crow::json::rvalue& request_body, 
    const std::string& key,
    std::uint16_t& value_out, 
    std::string& error_message) {
    crow::json::rvalue value;
    if (const auto error_code = ParseJsonParams(request_body, key, crow::json::type::Number, value, error_message);ErrorCode::SUCCESS != error_code) {
        return error_code;
    }
    const int parsed_value = int(value);
    if (parsed_value < 0 || parsed_value > UINT16_MAX) {
        error_message = "'" + key + "' out of range";
        return ErrorCode::PARAMS_ERROR;
    }   
    value_out = parsed_value;
    return ErrorCode::SUCCESS;
}

template <>
inline ErrorCode ParseJsonParams(
    const crow::json::rvalue& request_body, 
    const std::string& key,
    std::string& value_out, 
    std::string& error_message) {
    crow::json::rvalue value;
    if (const auto error_code = ParseJsonParams(request_body, key, crow::json::type::String, value, error_message);ErrorCode::SUCCESS != error_code) {
        return error_code;
    }
    value_out = value.s();
    return ErrorCode::SUCCESS;
}

template <>
inline ErrorCode ParseJsonParams(
    const crow::json::rvalue& request_body, 
    const std::string& key,
    std::vector<std::string>& value_out, 
    std::string& error_message) {
    crow::json::rvalue value;
    if (const auto error_code = ParseJsonParams(request_body, key, crow::json::type::List, value, error_message);ErrorCode::SUCCESS != error_code) {
        return error_code;
    }
    for (const auto& v : value) {
        if (v.t() != crow::json::type::String) {
            error_message = "'" + key + "' is invalid";
            return ErrorCode::PARAMS_ERROR;
        }
        value_out.push_back(v.s());
    }
    return ErrorCode::SUCCESS;
}

template <>
inline ErrorCode ParseJsonParams(
    const crow::json::rvalue& request_body, 
    const std::string& key,
    bool& value_out, 
    std::string& error_message) {
    if (!request_body.has(key)) {
        error_message = "'" + key + "' is required";
        return ErrorCode::PARAMS_ERROR;
    }
    if (request_body[key].t() == crow::json::type::True) {
        value_out = true;
        return ErrorCode::SUCCESS;
    }
    if (request_body[key].t() == crow::json::type::False) {
        value_out = false;
        return ErrorCode::SUCCESS;
    }
    error_message = "'" + key + "' is invalid";
    return ErrorCode::PARAMS_ERROR;
}