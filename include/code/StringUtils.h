#pragma once
#include <string>

namespace StringUtils
{
    inline std::string Split(const void* data, uint32_t len, const char ch)
    {
        std::string str(reinterpret_cast<const char*>(data), len);

        size_t end = str.find_last_not_of(ch);
        if (end != std::string::npos)
        {
            str = str.substr(0, end + 1);
        }

        return str;
    }
};