#pragma once

#include <string>
namespace SystemUpgradeService {
void UpgradeSystem(const std::string& filePath, const std::string& fileName);
void RollbackSystem();
}  // namespace SystemUpgradeService