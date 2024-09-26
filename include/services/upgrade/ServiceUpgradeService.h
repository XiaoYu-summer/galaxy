#pragma once
#include <crow.h>

#include <string>
namespace ServiceUpgradeService {
void Upgrade(const std::string& file, const std::string& file_name);
void Rollback();
}  // namespace ServiceUpgradeService