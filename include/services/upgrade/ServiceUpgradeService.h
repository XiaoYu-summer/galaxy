#pragma once

namespace ServiceUpgradeService {
void Upgrade(const std::string& file, const std::string& file_name);
void Rollback();
}  // namespace ServiceUpgradeService