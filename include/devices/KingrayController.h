#pragma once
#include <string>
#include "devices/DeviceController.h"
#include "AsyncProtocol.h"


struct DeviceNetworkInfo;
struct DeviceAddress;
struct DeviceVersion;

class KingrayController : public DeviceController
                        , public aoip::UdpCallback
                        , public std::enable_shared_from_this<KingrayController>
{
public:
    KingrayController(const DeviceNetworkInfo& info);
    virtual ~KingrayController() = default;

    virtual std::string GetFunctionCode(const std::vector<uint8_t>& response) const override; 
    virtual std::string GetDeviceName(const std::string& deviceId) const override;
    virtual DeviceAddress GetDeviceAddress(const std::string& deviceId) const override;
    virtual DeviceVersion GetDeviceVersion(const std::string& deviceId) const override;
    virtual bool GetDeviceOnlineStatus(const std::string& deviceId) const override;
private:
    void InitTransport();


    std::shared_ptr<aoip::AsyncProtocol> transport_;

};
