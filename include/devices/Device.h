#pragma once
#include <string>
#include <memory>

class DeviceController;
class DeviceNetworkInfo;
class DeviceInfo;

class Device
{
private:
    Device(const DeviceNetworkInfo& info);
    Device(const std::shared_ptr<Device>&);
    Device(Device&)=delete;
    Device(Device&&)=delete;
    Device(const Device&)=delete;
    Device(const Device&&)=delete;
    Device& operator=(Device)=delete;
    Device& operator=(Device&)=delete;
    Device& operator=(Device&&)=delete;
    Device& operator=(const Device&)=delete;
    Device& operator=(const Device&&)=delete;
public:
    static std::shared_ptr<Device> CreateDevice(const DeviceNetworkInfo& info);

    ~Device() = default;
    /**
     * 获取设备信息
     * @param info [out] 设备信息
     * @return true: 获取成功
     * */ 
    bool GetInfo(DeviceInfo&) const;
    /**
     * 获取设备ID
     * @return 设备ID
     * */
    std::string GetId() const;
    /**
     * 锁定设备
     * @param lock 是否锁定
     * @return true: 执行成功
     *  */    
    bool SetLock(bool lock);
    /**
     * 设置音量
     * @param volume 音量值
     * @return true: 设置成功
     *  */
    bool SetVolume(uint16_t volume);
    /**
     * 设备指示(设备寻址)
     * @return true: 寻址成功
    */
    bool Flashing();
    /**
     * 设备扬声器检查
     * @return true: 检查成功
    */
    bool CheckSpeaker();

    /**
     * 设备断开连接
     * @return true: 连接成功
    */
    bool Disconnect();

    /**
     * 设备是否连接
     * @return true: 连接成功
    */
    bool IsConnecting();

    /**
     * 设备静音
     * @param mute 是否静音
     * @return true: 执行成功
    */
    bool SetMute(bool mute);

private:
    std::shared_ptr<DeviceController> controller_;
};
