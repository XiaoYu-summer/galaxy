#pragma once

/**
 * @brief 错误码
 * 所有错误码均为6位数字，前三位为HTTP状态码，后三位为业务错误码
 */
enum ErrorCode {
    SUCCESS = 200000,
    PARAMS_ERROR = 400100,
    UNKNOWN_ERROR = 400101,
    MD5_MISMATCH = 400110,
    UPGRADE_ERROR = 400111,
    FILE_TOO_LARGE = 400112,
};