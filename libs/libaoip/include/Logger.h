#pragma once
#include <iostream>
#include <sstream>

#ifdef __GNUC__
#define __FUNC__ __PRETTY_FUNCTION__ 
#else
#define __FUNC__ __FUNCTION__ 
#endif

#define AOIP_CUSTOM_LOG 1

#if AOIP_CUSTOM_LOG

#include "common/LoggerWrapper.h"

#define AOIP_LOG_DEBUG(message) \
	{ \
		std::ostringstream oss; \
		oss << message << ", this=" << this << ", function=" << __FUNC__ << " (" << __FILE__ << ":" << __LINE__ << ")"; \
		LOG_D(oss.str()); \
	}

#define AOIP_LOG_INFO(message)  \
	{ \
		std::ostringstream oss; \
		oss << message << ", this=" << this << ", function=" << __FUNC__ << " (" << __FILE__ << ":" << __LINE__ << ")"; \
		LOG_I(oss.str()); \
	}

#define AOIP_LOG_WARN(message)  \
	{ \
		std::ostringstream oss; \
		oss << message << ", this=" << this << ", function=" << __FUNC__ << " (" << __FILE__ << ":" << __LINE__ << ")"; \
		LOG_W(oss.str()); \
	}

#define AOIP_LOG_ERROR(message)  \
	{ \
		std::ostringstream oss; \
		oss << message << ", this=" << this << ", function=" << __FUNC__ << " (" << __FILE__ << ":" << __LINE__ << ")"; \
		LOG_E(oss.str()); \
	}
#else

#define AOIP_LOG_DEBUG(message) \
	{ \
		std::ostringstream oss; \
		oss << message << ", this=" << this << ", function=" << __FUNC__ << " (" << __FILE__ << ":" << __LINE__ << ")"; \
		std::cout << "[D] " << oss.str() << std::endl; \
	}

#define AOIP_LOG_INFO(message)  \
	{ \
		std::ostringstream oss; \
		oss << message << ", this=" << this << ", function=" << __FUNC__ << " (" << __FILE__ << ":" << __LINE__ << ")"; \
		std::cout << "[I] " << oss.str() << std::endl; \
	}

#define AOIP_LOG_WARN(message)  \
	{ \
		std::ostringstream oss; \
		oss << message << ", this=" << this << ", function=" << __FUNC__ << " (" << __FILE__ << ":" << __LINE__ << ")"; \
		std::cout << "[W] " << oss.str() << std::endl; \
	}

#define AOIP_LOG_ERROR(message)  \
	{ \
		std::ostringstream oss; \
		oss << message << ", this=" << this << ", function=" << __FUNC__ << " (" << __FILE__ << ":" << __LINE__ << ")"; \
		std::cout << "[E] " << oss.str() << std::endl; \
	}

#endif
