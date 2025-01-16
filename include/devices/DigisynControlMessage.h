#pragma once
#include <string>
#include <vector>
#include <map>
#include <boost/hana.hpp>
#include <nlohmann/json.hpp>

// 使用命名空间
namespace hana = boost::hana;
using json = nlohmann::json;

// 序列化函数
template <typename T>
void to_json(json& j, const T& obj) {
    hana::for_each(hana::accessors<T>(), [&](auto pair) {
        auto name = hana::first(pair);
        auto accessor = hana::second(pair);
        j[hana::to<const char*>(name)] = accessor(obj);
    });
}

// 反序列化函数
template <typename T>
void from_json(const json& j, T& obj) {
    hana::for_each(hana::accessors<T>(), [&](auto pair) {
        auto name = hana::first(pair);
        auto accessor = hana::second(pair);
        j.at(hana::to<const char*>(name)).get_to(accessor(obj));
    });
}

// 特化std::vector的序列化与反序列化
template <typename T>
void to_json(json& j, const std::vector<T>& vec) {
    j = json::array();
    for (const auto& item : vec) {
        j.push_back(item);
    }
}

template <typename T>
void from_json(const json& j, std::vector<T>& vec) {
    for (const auto& item : j) {
        vec.push_back(item.get<T>());
    }
}

// 特化std::map的序列化与反序列化
template <typename K, typename V>
void to_json(json& j, const std::map<K, V>& map) {
    j = json::object();
    for (const auto& item : map) {
        j[item.first] = item.second;
    }
}

template <typename K, typename V>
void from_json(const json& j, std::map<K, V>& map) {
    for (auto it = j.begin(); it != j.end(); ++it) {
        map[it.key()] = it.value().get<V>();
    }
}

// 消息类型
namespace MessageType
{
    const std::string DEVIDE_DISCOVERY = "iAmDigisyn";            // 设备发现
    const std::string FIND_SPECIFIED_DEVICE = "whoIsDigisyn";     // 查找指定设备
};

struct DeviceDiscoveryMsg
{
    std::string ops;           // 命令
    std::string protVer;       // 协议版本
    std::string devId;         // 设备 ID
    std::string name;          // len < 32，下位机名称
    std::string ip;            // 下位机 IP 地址
    std::string port;          // 下位机单播端口
    std::string chNumRx;       // 接收通道数量
    std::string chNumTx;       // 发送通道数量
    std::string ucastNumRx;    // 最大单播接收流数量
    std::string ucastNumTx;    // 最大单播发送流数量
    std::string mcastNumRx;    // 最大组播接收流数量
    std::string mcastNumTx;    // 最大组播发送流数量
    std::string sampleRate;    // 采样率
    std::string sampleByteNum; // 接收采样深度 BYTEs 3=24bits，2=16bits，4=32bits
    std::string pFrameNum;     // also called pTime
    std::string delayUs;       // only rx
    std::string myClkId;       // 本设备 ClkId
    std::string gmClkId;       // 主时钟 ClkId
    std::string model;         // 设备类型
    std::string phyChNumRx;    // phyCh is opposite from netCh, normally i2s or vSndCard
    std::string phyChNumTx;    // phyCh depend on specific model
    std::string canWork;       // 设备状态是否正常 1-表示正常，0-表示异常
};
