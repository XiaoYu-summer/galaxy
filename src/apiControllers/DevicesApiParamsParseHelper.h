#pragma once
#include "utils/QueryParamsParseHelper.h"
#include "apiControllers/DevicesApiController.h"
#include "utils/ResUtils.h"

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

inline ErrorCode ParseDevicesHelper(
    const std::vector<std::string>& deviceIds,
    std::unordered_map<std::string, std::shared_ptr<Device>>& devices_out,
    std::string& error_message) {
    if (deviceIds.empty()) {
        error_message = "'deviceIds' is empty";
        return ErrorCode::DEVICEID_ARRAY_EMPTY;
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
    HandleJsonReqWithParams<std::string>(request, response,[handler](const std::string& deviceId, crow::response& response){
        std::shared_ptr<Device> device = DevicesApiController::GetDevice(deviceId);
        if (!device) {
            return FailResponse(response, ErrorCode::DEVICEID_NOT_FOUND, DEVICEID_NOT_FOUND_MSG(deviceId));
        }
        handler(device, response);
    }, DEVICEID_STR);
};

template <typename T, typename Handler>
inline void HandleDevicePostReqWithParams(const crow::request &request, const std::string& key, crow::response &response, const Handler &&handler) {
    HandleJsonReqWithParams<std::string, T>(request, response, [handler](const std::string& deviceId, const T& value, crow::response& response){
        std::shared_ptr<Device> device = DevicesApiController::GetDevice(deviceId);
        if (!device) {
            return FailResponse(response, ErrorCode::DEVICEID_NOT_FOUND, DEVICEID_NOT_FOUND_MSG(deviceId));
        }
        handler(device, value, response);
    }, DEVICEID_STR, key);
};

template <typename T, typename Handler>
inline void HandleDevicesPostReqWithParams(const crow::request &request, const std::string& key, crow::response &response, const Handler &&handler) {
    HandleJsonReqWithParams<std::vector<std::string>, T>(request, response, [handler](const std::vector<std::string>& deviceIds, const T& value, crow::response& response){
        std::unordered_map<std::string, std::shared_ptr<Device>> devices;
        std::string error_message;
        if (const auto error_code = ParseDevicesHelper(deviceIds, devices, error_message);error_code != ErrorCode::SUCCESS) {
            return FailResponse(response, error_code, error_message);
        }
        handler(devices, value, response);
    }, DEVICEID_ARRAY_STR, key);
};