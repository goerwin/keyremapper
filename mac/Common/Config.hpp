#pragma once

#include <stdexcept>
#include <string>

#include "../../common/Helpers.hpp"

namespace Config {
// The config at path with its imports resolved, as JSON. Throws when it or a
// file it imports isn't valid JSON
inline std::string resolve(std::string path) {
  auto config = Helpers::getJsonFile(path);
  if (config.is_discarded())
    throw std::runtime_error(path + " or a file it imports isn't valid JSON");
  return config.dump();
}
}  // namespace Config
