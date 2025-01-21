#pragma once

#ifdef __GNUC__
#define __FUNC__ __PRETTY_FUNCTION__ 
#else
#define __FUNC__ __FUNCTION__ 
#endif

/**
 * @brief 错误码
 * 所有错误码均为6位数字，前三位为HTTP状态码，后三位为业务错误码
 */
enum class ErrorCode {
    SUCCESS                     = 200000, ///< 请求成功
    DELETE_SUCCESS              = 204000, ///< 删除成功
    PARAMS_ERROR                = 400001, ///< 参数错误
    JSON_BODY_ERROR             = 400002, ///< JSON解析错误
    MD5_MISMATCH                = 400101, ///< MD5不匹配
    UPDATE_ERROR                = 400102, ///< 更新错误
    DEVICEID_ARRAY_EMPTY        = 400201, ///< 设备数组为空
    DEVICE_LOCK_ERROR           = 400202, ///< 设备锁定错误
    DEVICE_UNLOCK_ERROR         = 400203, ///< 设备解锁锁错误
    DEVICE_SETVOLUME_ERROR      = 400204, ///< 设备设置音量错误
    DEVICE_DISCONNECT_ERROR     = 400205, ///< 设备断开连接错误
    DEVICE_FLASHING_ERROR       = 400206, ///< 设备寻址错误
    DEVICE_CHECKSPEAKER_ERROR   = 400207, ///< 设备扬声器检查错误
    DEVICE_GETINFO_ERROR        = 400208, ///< 设备信息获取错误
    DEVICE_MUTE_ERROR           = 400209, ///< 设备静音错误
    DEVICE_UNMUTE_ERROR         = 400210, ///< 设备取消静音错误
    DEVICE_RESET_ERROR          = 400211, ///< 设备重置错误
    AUTH_ERROR                  = 401100, ///< 认证错误
    AUTH_EXPIRED                = 401101, ///< 认证过期
    PASSWORD_ERROR              = 401102, ///< 密码错误
    OLD_PASSWORD_ERROR          = 401103, ///< 旧密码错误
    DEVICEID_NOT_FOUND          = 404001, ///< 设备未找到错误
    FILE_NOT_FOUND              = 404002, ///< 文件未找到错误
    FILE_TOO_LARGE              = 413112, ///< 文件太大
    UNKNOWN_ERROR               = 500001, ///< 服务器内部错误
    DEVICE_MANAGER_ERROR        = 500100, ///< 设备管理错误
};

struct RuntimeException : public std::runtime_error
{
    RuntimeException(const std::string & w) :std::runtime_error(w)
    {
    }
    virtual ~RuntimeException() {}
};

#define RUNTIME_EXCEPTION(msg) \
{ \
    std::ostringstream oss; \
    oss << msg << ", function="<< __FUNC__ << " (" << __FILE__ << ":" << __LINE__ << ")"; \
    throw RuntimeException(oss.str()); \
}
