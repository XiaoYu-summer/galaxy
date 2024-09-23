#pragma once

namespace ServiceUpgradeService {
void Upgrade(const std::string& file);
void Rollback();
}  // namespace ServiceUpgradeService