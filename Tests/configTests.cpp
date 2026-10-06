// Runs the "tests" of every profile in a user config.json
// Usage: configTests <config.json> <symbols.json>

#include <iostream>

#include "../common/Helpers.hpp"
#include "../common/KeyRemapper.hpp"
#include "../common/TestHelpers.hpp"

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    Helpers::print("Usage: configTests <config.json> <symbols.json>");
    return 2;
  }

  auto config = Helpers::getJsonFile(argv[1]);
  if (config.is_discarded() || !config["profiles"].is_array()) {
    Helpers::print("Error: invalid config " + std::string(argv[1]));
    return 1;
  }

  auto symbols = Helpers::getJsonFile(argv[2]);
  auto profiles = config["profiles"];
  int testedProfiles = 0;
  bool ok = true;

  for (size_t i = 0; i < profiles.size(); i++) {
    auto profile = profiles[i];
    if (profile["tests"].is_null()) continue;

    auto results = TestHelpers::runTests(profile["tests"], profile, symbols);
    testedProfiles++;
    ok = ok && bool(results["ok"]);
    Helpers::print("profile " + std::to_string(i) + ": " +
                   std::string(results["message"]));
  }

  if (testedProfiles == 0) {
    Helpers::print("Error: no profile has tests");
    return 1;
  }

  return ok ? 0 : 1;
}
