#pragma once
#include <string>

namespace FirmwareUpgradeService {
void UpgradeFirmware(const std::string& filePath);
void RollbackFirmware();
}  // namespace FirmwareUpgradeService