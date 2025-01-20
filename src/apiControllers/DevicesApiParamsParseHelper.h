#pragma once
#include "utils/JsonParamsParseHelper.h"
#include "utils/QueryParamsParseHelper.h"
#include "apiControllers/DevicesApiController.h"

constexpr auto DEVICEID_STR = "deviceId";
constexpr auto DEVICEID_ARRAY_STR = "deviceIds";

#define DEVICEID_NOT_FOUND_MSG(deviceId) "'" + deviceId + "' not exists"

inline ErrorCode ParseDeviceGetParams(
    const crow::query_string& url_params, 
    std::shared_ptr<Device>& device_out,
    std::string& error_message) {
    std::string deviceId;
    if (const auto error_code = ParseQueryParams(url_params, DEVICEID_STR, deviceId, error_message);ErrorCode::SUCCESS != error_code) {
        return error_code;
    }
    const auto device = DevicesApiController::GetDevice(deviceId);
        if (!device) {
        error_message = DEVICEID_NOT_FOUND_MSG(deviceId);
        return ErrorCode::DEVICEID_NOT_FOUND; 
    }
    device_out = device;
    return ErrorCode::SUCCESS;
}

inline ErrorCode ParseDevicePostParams(
    const crow::json::rvalue& request_body, 
    std::shared_ptr<Device>& device_out,
    std::string& error_message) {
    std::string deviceId;
    if (const auto error_code = ParseJsonParams(request_body, DEVICEID_STR, deviceId, error_message);ErrorCode::SUCCESS != error_code) {
        return error_code;
    }
    const auto device = DevicesApiController::GetDevice(deviceId);
    if (!device) {
        error_message = DEVICEID_NOT_FOUND_MSG(deviceId);
        return ErrorCode::DEVICEID_NOT_FOUND; 
    }
    device_out = device;
    return ErrorCode::SUCCESS;
}

inline ErrorCode ParseDevicesPostParams(
    const crow::json::rvalue& request_body, 
    std::unordered_map<std::string, std::shared_ptr<Device>>& devices_out,
    std::string& error_message) {
    std::vector<std::string> deviceIds;
    if (const auto error_code = ParseJsonParams(request_body, DEVICEID_ARRAY_STR, deviceIds, error_message);ErrorCode::SUCCESS != error_code) {
        return error_code;
    }
    devices_out.clear();
    for (const auto& deviceId : deviceIds) {
        const auto device = DevicesApiController::GetDevice(deviceId);
        if (!device) {
            error_message = DEVICEID_NOT_FOUND_MSG(deviceId);
            return ErrorCode::DEVICEID_NOT_FOUND;
        }
        devices_out.emplace(deviceId, device);
    }

    if (devices_out.empty()) {
        error_message = "'deviceIds' is empty";
        return ErrorCode::DEVICEID_ARRAY_EMPTY;
    }

    return ErrorCode::SUCCESS;
}

inline auto HandleDeviceGetReq = [](const crow::request &request, crow::response &response, const auto &&handler) {
    std::shared_ptr<Device> device;
    std::string error_message;
    const auto& requestUrlParams = request.url_params;

    if (const auto error_code = ParseDeviceGetParams(requestUrlParams, device, error_message);ErrorCode::SUCCESS != error_code) {
        return FailResponse(response, error_code, error_message);
    }
    handler(device, response);
};

auto HandleDevicePostReq = [](const crow::request &request, crow::response &response, const auto &&handler) {
    std::shared_ptr<Device> device;
    std::string error_message;
    const auto requestBody = crow::json::load(request.body);
    if (!requestBody) {
        return FailResponse(response, ErrorCode::JSON_BODY_ERROR, "Invalid JSON");
    }

    if (const auto error_code = ParseDevicePostParams(requestBody, device, error_message);ErrorCode::SUCCESS != error_code) {
        return FailResponse(response, error_code, error_message);
    }
    handler(device, response);
};

template <typename T, typename Handler>
inline void HandleDevicePostReqWithParams(const crow::request &request, const std::string& key, crow::response &response, const Handler &&handler) {
    T value;
    std::shared_ptr<Device> device;
    std::string error_message;
    const auto requestBody = crow::json::load(request.body);
    if (!requestBody) {
        return FailResponse(response, ErrorCode::JSON_BODY_ERROR, "Invalid JSON");
    }

    if (const auto error_code = ParseJsonParams(requestBody, key, value, error_message);ErrorCode::SUCCESS != error_code) {
        return FailResponse(response, error_code, error_message);
    }

    if (const auto error_code = ParseDevicePostParams(requestBody, device, error_message);ErrorCode::SUCCESS != error_code) {
        return FailResponse(response, error_code, error_message);
    }

    handler(device, value, response);
};

template <typename T, typename Handler>
inline void HandleDevicesPostReqWithParams(const crow::request &request, const std::string& key, crow::response &response, const Handler &&handler) {
    T value;
    std::unordered_map<std::string, std::shared_ptr<Device>> devices;
    std::string error_message;
    const auto requestBody = crow::json::load(request.body);
    if (!requestBody) {
        return FailResponse(response, ErrorCode::JSON_BODY_ERROR, "Invalid JSON");
    }

    if (const auto error_code = ParseJsonParams(requestBody, key, value, error_message);ErrorCode::SUCCESS != error_code) {
        return FailResponse(response, error_code, error_message);
    }

    if (const auto error_code = ParseDevicesPostParams(requestBody, devices, error_message);ErrorCode::SUCCESS != error_code) {
        return FailResponse(response, error_code, error_message);
    }

    handler(devices, value, response);
};