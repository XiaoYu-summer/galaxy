#pragma once
#include <string>

namespace FirmwareUpgradeService {
void Upgrade(const std::string& file);
void Rollback();
}  // namespace FirmwareUpgradeService