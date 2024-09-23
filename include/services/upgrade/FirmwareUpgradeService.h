#pragma once

namespace FirmwareUpgradeService {
void Upgrade(const std::string& file);
void Rollback();
}  // namespace FirmwareUpgradeService