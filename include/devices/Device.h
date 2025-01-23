#pragma once
#include <string>
#include <memory>
#include "devices/DeviceParams.h"


class DeviceController;

class Device
{
public:
    static std::shared_ptr<Device> CreateDevice(const DeviceNetworkInfo& info, DeviceController* controller = nullptr);

    Device() = default;
    virtual ~Device() = default;
    /**
     * 获取设备信息
     * @param info [out] 设备信息
     * @return true: 获取成功
     * */ 
    virtual bool GetInfo(DeviceInfo&) const = 0;
    /**
     * 获取设备ID
     * @return 设备ID
     * */
    virtual std::string GetId() const = 0;
    /**
     * 重置设备
     * @return true: 重置成功
     * */ 
    virtual bool Reset() = 0;
    /**
     * 锁定设备
     * @param lock 是否锁定
     * @return true: 执行成功
     *  */    
    virtual bool SetLock(bool lock) = 0;
    /**
     * 设置音量
     * @param volume 音量值
     * @return true: 设置成功
     *  */
    virtual bool SetVolume(uint16_t volume) = 0;
    /**
     * 设备指示(设备寻址)
     * @return true: 寻址成功
    */
    virtual bool Flashing() = 0;
    /**
     * 设备扬声器检查
     * @return true: 检查成功
    */
    virtual bool CheckSpeaker() = 0;

    /**
     * 设备断开连接
     * @return true: 连接成功
    */
    virtual bool Disconnect() = 0;

    /**
     * 设备是否连接
     * @return true: 连接成功
    */
    virtual bool IsConnecting() = 0;

    /**
     * 设备静音
     * @param mute 是否静音
     * @return true: 执行成功
    */
    virtual bool SetMute(bool mute) = 0;
};
