#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
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

// 将 MAC 地址转换为字符串形式
inline std::string MacToString(const uint8_t mac[6])
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < 6; ++i)
    {
        oss << std::setw(2) << static_cast<int>(mac[i]);
        if (i != 5)
        {
            oss << ":";
        }
    }
    return oss.str();
}

// 将 IP 地址转换为字符串形式
inline std::string IpToString(const uint8_t ip[4])
{
    std::ostringstream oss;
    for (int i = 0; i < 4; ++i)
    {
        oss << static_cast<int>(ip[i]);
        if (i != 3)
        {
            oss << ".";
        }
    }
    return oss.str();
}

};