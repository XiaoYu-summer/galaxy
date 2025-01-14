#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "controllers/DeviceManager.h"
#include "devices/Device.h"
#include "devices/DeviceParams.h"
#include "Logger.h"


// 设备扫描
class DeviceScanner {
public:
    DeviceScanner(boost::asio::io_context& io_context)
        : timer_(io_context, boost::asio::chrono::seconds(10)) {
        // 启动定时器
        timer_.async_wait(boost::bind(&DeviceScanner::scanForDevices, this));
    }

private:
    void scanForDevices() {
        // 执行扫描设备的逻辑
        std::cout << "Scanning for new devices..." << std::endl;

        // 模拟扫描设备的过程
        // 这里可以添加实际的设备扫描逻辑
        bool newDeviceFound = true; // 假设发现了新设备

        if (newDeviceFound) {
            std::cout << "New device found!" << std::endl;
        } else {
            std::cout << "No new devices found." << std::endl;
        }

        // 重置定时器，继续等待下一个 10 秒
        timer_.expires_after(boost::asio::chrono::seconds(10));
        timer_.async_wait(boost::bind(&DeviceScanner::scanForDevices, this));
    }

    boost::asio::steady_timer timer_;
};

DeviceManager::DeviceManager()
{
    Init();
}

void DeviceManager::Init()
{
   // 创建设备发现任务
}

void DeviceManager::AddDevice(const DeviceNetworkInfo& info)
{
    if (!devices_.count(info.deviceId))
    {
        std::shared_ptr<Device> device = Device::CreateDevice(info);
        devices_[info.deviceId] = device;
    }
    else
    {
        AOIP_LOG_WARN("device is exist! deviceId=" << info.deviceId << ", deviceType=" << (int)info.deviceType);
    }
}

void DeviceManager::InitRoutes(CrowApp& app)
{

}

