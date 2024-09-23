#pragma once
#include <crow.h>

#include <boost/process.hpp>
#include <string>
namespace ServiceUpgradeService {
void Upgrade(const std::string& file, const std::string& file_name);
void Rollback();
}  // namespace ServiceUpgradeService