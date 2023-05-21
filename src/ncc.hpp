#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace ncc {

enum class build_type_e { DEBUG, RELEASE };

struct config_s {
  std::filesystem::path target_path;
  std::filesystem::path output_path;
  std::vector<std::filesystem::path> include_paths;
  build_type_e build_type{build_type_e::DEBUG};
};

extern config_s &get_ncc_config();

} // namespace ncc
