#pragma once
#include <crow.h>

#include <string>

namespace ServiceUpgradeService {
void UpgradeService(const std::string& filePath, const std::string& fileName);
void RollbackService();
}  // namespace ServiceUpgradeService