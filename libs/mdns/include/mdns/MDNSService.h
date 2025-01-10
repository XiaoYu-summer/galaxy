#pragma once

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <cstdint>

namespace mdns {

/**
 * @brief mDNS服务发现的结果信息
 */
struct ServiceInfo {
    std::string name;        ///< 服务名称
    std::string type;        ///< 服务类型 (如 "_http._tcp")
    std::string domain;      ///< 域名 (通常是 "local")
    std::string host;        ///< 主机名
    uint16_t port{0};        ///< 服务端口
    std::string ip;          ///< IP地址
    std::vector<std::pair<std::string, std::string>> txt;  ///< TXT记录
};

// 前向声明
class MDNSServiceImpl;

/**
 * @brief mDNS服务类，提供服务发布和发现功能
 * 
 * 这个类提供了一个高级别的C++ API来使用mDNS协议。它可以：
 * 1. 发布本地服务，让其他设备能够发现
 * 2. 发现网络上的其他服务
 * 3. 自动处理服务的生命周期
 */
class MDNSService {
public:
    /**
     * @brief 构造函数
     */
    MDNSService();
    
    /**
     * @brief 析构函数
     */
    ~MDNSService();

    /**
     * @brief 发布一个服务
     * 
     * @param name 服务名称
     * @param type 服务类型 (如 "_http._tcp")
     * @param port 服务端口
     * @param txt TXT记录，键值对形式
     * @param domain 域名，默认为"local"
     * @return true 发布成功
     * @return false 发布失败
     */
    bool publish(const std::string& name, 
                const std::string& type,
                uint16_t port,
                const std::vector<std::pair<std::string, std::string>>& txt = {},
                const std::string& domain = "local");

    /**
     * @brief 浏览特定类型的服务
     * 
     * @param type 服务类型 (如 "_http._tcp")
     * @param callback 发现服务时的回调函数
     * @param domain 域名，默认为"local"
     * @return true 开始浏览成功
     * @return false 开始浏览失败
     */
    bool browse(const std::string& type,
               std::function<void(const ServiceInfo&)> callback,
               const std::string& domain = "local");

    /**
     * @brief 停止浏览服务
     */
    void stopBrowse();

    /**
     * @brief 停止发布服务
     */
    void stopPublish();

    /**
     * @brief 检查服务是否正在运行
     * 
     * @return true 服务正在运行
     * @return false 服务未运行
     */
    bool isRunning() const;

private:
    std::unique_ptr<MDNSServiceImpl> impl_;
    
    // 禁止拷贝和赋值
    MDNSService(const MDNSService&) = delete;
    MDNSService& operator=(const MDNSService&) = delete;
};

} // namespace mdns
