#pragma once
#include <Poco/Logger.h>

#include <sstream>
#include <string>

#ifdef __GNUC__
#define __FUNC__ __PRETTY_FUNCTION__
#else
#define __FUNC__ __FUNCTION__
#endif

#define DEFINE_FILE_NAME(x)              \
    namespace                            \
    {                                    \
    static const char* __FILENAME__ = x; \
    }

// fatal
#define LOG_FATAL(msg)                                    \
    {                                                     \
        std::ostringstream oss;                           \
        oss << msg << ", function=" << __FUNC__;          \
        logger_.fatal(oss.str(), __FILENAME__, __LINE__); \
    }

#define LOG_FATAL_THIS(msg)                                           \
    {                                                                 \
        std::ostringstream oss;                                       \
        oss << msg << ", this=" << this << ", function=" << __FUNC__; \
        logger_.fatal(oss.str(), __FILENAME__, __LINE__);             \
    }

// critical
#define LOG_CRITICAL(msg)                                    \
    {                                                        \
        std::ostringstream oss;                              \
        oss << msg << ", function=" << __FUNC__;             \
        logger_.critical(oss.str(), __FILENAME__, __LINE__); \
    }

#define LOG_CRITICAL_THIS(msg)                                        \
    {                                                                 \
        std::ostringstream oss;                                       \
        oss << msg << ", this=" << this << ", function=" << __FUNC__; \
        logger_.critical(oss.str(), __FILENAME__, __LINE__);          \
    }

// error
#define LOG_ERROR(msg)                                    \
    {                                                     \
        std::ostringstream oss;                           \
        oss << msg << ", function=" << __FUNC__;          \
        logger_.error(oss.str(), __FILENAME__, __LINE__); \
    }

#define LOG_ERROR_THIS(msg)                                           \
    {                                                                 \
        std::ostringstream oss;                                       \
        oss << msg << ", this=" << this << ", function=" << __FUNC__; \
        logger_.error(oss.str(), __FILENAME__, __LINE__);             \
    }

// warning
#define LOG_WARNING(msg)                                    \
    {                                                       \
        std::ostringstream oss;                             \
        oss << msg << ", function=" << __FUNC__;            \
        logger_.warning(oss.str(), __FILENAME__, __LINE__); \
    }

#define LOG_WARNING_THIS(msg)                                         \
    {                                                                 \
        std::ostringstream oss;                                       \
        oss << msg << ", this=" << this << ", function=" << __FUNC__; \
        logger_.warning(oss.str(), __FILENAME__, __LINE__);           \
    }

// notice
#define LOG_NOTICE(msg)                                    \
    {                                                      \
        std::ostringstream oss;                            \
        oss << msg << ", function=" << __FUNC__;           \
        logger_.notice(oss.str(), __FILENAME__, __LINE__); \
    }

#define LOG_NOTICE_THIS(msg)                                          \
    {                                                                 \
        std::ostringstream oss;                                       \
        oss << msg << ", this=" << this << ", function=" << __FUNC__; \
        logger_.notice(oss.str(), __FILENAME__, __LINE__);            \
    }

// information
#define LOG_INFO(msg)                                           \
    {                                                           \
        std::ostringstream oss;                                 \
        oss << msg << ", function=" << __FUNC__;                \
        logger_.information(oss.str(), __FILENAME__, __LINE__); \
    }

#define LOG_INFO_THIS(msg)                                            \
    {                                                                 \
        std::ostringstream oss;                                       \
        oss << msg << ", this=" << this << ", function=" << __FUNC__; \
        logger_.information(oss.str(), __FILENAME__, __LINE__);       \
    }

// debug
#define LOG_DEBUG(msg)                                    \
    {                                                     \
        std::ostringstream oss;                           \
        oss << msg << ", function=" << __FUNC__;          \
        logger_.debug(oss.str(), __FILENAME__, __LINE__); \
    }

#define LOG_DEBUG_THIS(msg)                                           \
    {                                                                 \
        std::ostringstream oss;                                       \
        oss << msg << ", this=" << this << ", function=" << __FUNC__; \
        logger_.debug(oss.str(), __FILENAME__, __LINE__);             \
    }

// trace
#define LOG_TRACE(msg)                                    \
    {                                                     \
        std::ostringstream oss;                           \
        oss << msg << ", function=" << __FUNC__;          \
        logger_.trace(oss.str(), __FILENAME__, __LINE__); \
    }

#define LOG_TRACE_THIS(msg)                                           \
    {                                                                 \
        std::ostringstream oss;                                       \
        oss << msg << ", this=" << this << ", function=" << __FUNC__; \
        logger_.trace(oss.str(), __FILENAME__, __LINE__);             \
    }

#define LOGGER(tag) Poco::Logger::get(tag)

inline const char* _fileNameFromPath(const char* file)
{
    const char* end1 = ::strrchr(file, '/');
    const char* end2 = ::strrchr(file, '\\');
    if (!end1 && !end2)
    {
        return file;
    }
    else
    {
        return (end1 > end2) ? end1 + 1 : end2 + 1;
    }
}

#define LOG_INFO_TAG(tag, x)                                                       \
    try                                                                            \
    {                                                                              \
        std::ostringstream str;                                                    \
        str << x << ", function=" << __FUNCTION__;                                 \
        LOGGER(tag).information(str.str(), _fileNameFromPath(__FILE__), __LINE__); \
    }                                                                              \
    catch (Poco::Exception & e)                                                    \
    {                                                                              \
    }

#define LOG_WARN_TAG(tag, x)                                                   \
    try                                                                        \
    {                                                                          \
        std::ostringstream str;                                                \
        str << x << ", function=" << __FUNCTION__;                             \
        LOGGER(tag).warning(str.str(), _fileNameFromPath(__FILE__), __LINE__); \
    }                                                                          \
    catch (Poco::Exception & e)                                                \
    {                                                                          \
    }

#define LOG_ERROR_TAG(tag, x)                                                \
    try                                                                      \
    {                                                                        \
        std::ostringstream str;                                              \
        str << x << ", function=" << __FUNCTION__;                           \
        LOGGER(tag).error(str.str(), _fileNameFromPath(__FILE__), __LINE__); \
    }                                                                        \
    catch (Poco::Exception & e)                                              \
    {                                                                        \
    }

#define LOG_DEBUG_TAG(tag, x)                                                \
    try                                                                      \
    {                                                                        \
        std::ostringstream str;                                              \
        str << x << ", function=" << __FUNCTION__;                           \
        LOGGER(tag).debug(str.str(), _fileNameFromPath(__FILE__), __LINE__); \
    }                                                                        \
    catch (Poco::Exception & e)                                              \
    {                                                                        \
    }

#define LOGD(str) LOG_DEBUG_TAG(_MODEL_TAG_, str)

#define LOGI(str) LOG_INFO_TAG(_MODEL_TAG_, str)

#define LOGW(str) LOG_WARN_TAG(_MODEL_TAG_, str)

#define LOGE(str) LOG_ERROR_TAG(_MODEL_TAG_, str)

#define LOG_D(str) LOG_DEBUG_TAG("", str)

#define LOG_I(str) LOG_INFO_TAG("", str)

#define LOG_W(str) LOG_WARN_TAG("", str)

#define LOG_E(str) LOG_ERROR_TAG("", str)

#define LOG_D_THIS(str) LOG_DEBUG_TAG("", str << " this=" << this)

#define LOG_I_THIS(str) LOG_INFO_TAG("", str << " this=" << this)

#define LOG_W_THIS(str) LOG_WARN_TAG("", str << " this=" << this)

#define LOG_E_THIS(str) LOG_ERROR_TAG("", str << " this=" << this)