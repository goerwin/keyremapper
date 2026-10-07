#pragma once

#include <string>

#include "../Engine/Helpers.hpp"

namespace Config {
// The config at path with its imports resolved, as JSON. Throws when it or a
// file it imports isn't valid JSON
inline std::string resolve(std::string path) {
  return Helpers::getJsonFile(path).dump();
}
}  // namespace Config
