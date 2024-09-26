#pragma once

#include <string>
namespace SystemUpgradeService {
void Upgrade(const std::string& file, const std::string& file_name);
void Rollback();
}  // namespace SystemUpgradeService