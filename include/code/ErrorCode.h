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
    SUCCESS = 200000,
    PARAMS_ERROR = 400100,
    UNKNOWN_ERROR = 400101,
    MD5_MISMATCH = 400110,
    UPGRADE_ERROR = 400111,
    DEVICEID_INVAILD_ERROR = 400112,
    DEVICEID_RESET_ERROR = 400112,
    FILE_TOO_LARGE = 413112,
    AUTH_ERROR = 401100,
    AUTH_EXPIRED = 401101,
    PASSWORD_ERROR = 400102,
    OLD_PASSWORD_ERROR = 400103,
    DEVICEMANAGER_ERROR = 500100,
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
