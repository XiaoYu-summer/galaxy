#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mdns
{

struct ServiceInfo
{
    std::string name;                                      ///< 服务名称，例如 "My Printer"
    std::string type;                                      ///< 服务类型，例如 "_http._tcp"
    std::string host;                                      ///< 提供服务的主机名
    std::string ip;                                        ///< 服务的IP地址
    uint16_t port;                                         ///< 服务端口号
    std::vector<std::pair<std::string, std::string>> txt;  ///< TXT记录，包含额外的服务信息
};

class MDNSServiceImpl;

/**
 * @brief mDNS服务类，提供服务发布和发现功能
 *
 * 这个类提供了一个高级别的C++ API来使用mDNS协议。它可以：
 * 1. 发布本地服务，让其他设备能够发现
 * 2. 发现网络上的其他服务
 * 3. 自动处理服务的生命周期
 *
 * 使用PIMPL（Pointer to Implementation）模式来隐藏实现细节，提高ABI兼容性。
 */

class MDNSService
{
   public:
    /**
     * @brief 构造函数
     *
     * 初始化mDNS服务，但不会立即开始发布或浏览。
     */
    MDNSService();
    /**
     * @brief 析构函数
     *
     * 停止所有正在进行的mDNS操作，释放资源。
     */
    ~MDNSService();

    /**
     * @brief 发布一个服务
     *
     * 使服务在本地网络上可被发现。这个方法是非阻塞的，
     * 它会启动一个后台线程来处理mDNS通信。
     *
     * @param name 服务名称，如 "My Web Server"
     * @param type 服务类型，如 "_http._tcp"
     * @param port 服务端口
     * @param txt TXT记录，键值对形式的额外服务信息
     * @param domain 域名，默认为"local"
     * @return true 发布成功启动
     * @return false 发布失败
     */
    bool Publish(const std::string& name, const std::string& type, uint16_t port,
                 const std::vector<std::pair<std::string, std::string>>& txt, const std::string& domain = "local");

    /**
     * @brief 浏览特定类型的服务
     *
     * 开始在网络上搜索指定类型的服务。这个方法是非阻塞的，
     * 它会启动一个后台线程来处理mDNS通信。每当发现新服务时，
     * 都会调用提供的回调函数。
     *
     * @param type 要搜索的服务类型，如 "_http._tcp"
     * @param callback 发现服务时的回调函数
     * @param domain 搜索的域名范围，默认为"local"
     * @return true 浏览操作成功启动
     * @return false 浏览操作启动失败
     */
    bool Browse(const std::string& type, std::function<void(const ServiceInfo&)> callback,
                const std::string& domain = "local");

    /**
     * @brief 停止浏览服务
     *
     * 终止当前的服务浏览操作，停止相关的后台线程。
     */
    void StopBrowse();
    /**
     * @brief 停止发布服务
     *
     * 终止当前的服务发布，从网络上移除服务，停止相关的后台线程。
     */
    void StopPublish();
    /**
     * @brief 检查服务是否正在运行
     *
     * @return true 服务正在运行（正在发布或浏览）
     * @return false 服务未运行
     */
    bool IsRunning() const;

   private:
    std::unique_ptr<MDNSServiceImpl> impl_;
};

}  // namespace mdns