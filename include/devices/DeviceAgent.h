#pragma once
#include <string>
#include <memory>
#include "devices/Device.h"
#include "devices/DeviceController.h"

class DeviceAgent : public Device
{
public:
    DeviceAgent(const DeviceNetworkInfo& info, DeviceController* controller = nullptr);
    virtual ~DeviceAgent() = default;
    virtual bool GetInfo(DeviceInfo&) const override;
    virtual std::string GetId() const override;
    virtual bool CheckSpeaker() override;
    virtual bool Reset() override;
    virtual bool SetLock(bool lock) override;
    virtual bool SetVolume(uint16_t volume) override;
    virtual bool Flashing() override;
    virtual bool Disconnect() override;
    virtual bool IsConnecting() override;
    virtual bool SetMute(bool mute) override;
private:
    std::shared_ptr<DeviceController> controller_;
};
