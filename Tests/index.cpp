#include <iostream>

#if __APPLE__
  #include <mach-o/dyld.h>
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  #include <windows.h>
#endif

#include "../common/vendors/json.hpp"
#include "../common/Helpers.hpp"
#include "../common/TestHelpers.hpp"

void expect(bool value, std::string errorMsg = "") {
  if (value) return;

  Helpers::print(!errorMsg.empty() ? errorMsg : "Error, expect condition failed");
  exit(1);
}

int main(int argc, const char *argv[]) {
  std::string dirPath;

  // find absolute path for executable in different OSs
  // https://stackoverflow.com/a/1024937/1623282
  #if __APPLE__
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0) {
      printf("buffer too small; need size %u\n", size);
      return 1;
    }

    dirPath = Helpers::replaceAll(path, "/./", "/");
    dirPath = Helpers::replaceAll(path, "/output", "");
  #elif defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    char moduleFilepath[MAX_PATH];
    GetModuleFileNameA(NULL, moduleFilepath, MAX_PATH);
    auto strModuleFilepath = std::string(moduleFilepath);

    dirPath = strModuleFilepath.substr(0, strModuleFilepath.find_last_of("\\"));
    Helpers::print(dirPath);
  #endif

  auto symbols = Helpers::getJsonFile(dirPath, "symbols.json");

  // TESTS

  auto ruleFiles = nlohmann::json::array({
    Helpers::getJsonFile(dirPath, "rules1.json"),
    Helpers::getJsonFile(dirPath, "rules2.json"),
    Helpers::getJsonFile(dirPath, "rules3.json"),
    Helpers::getJsonFile(dirPath, "rules4.json"),
    Helpers::getJsonFile(dirPath, "rules5.json"),
  });

  for (size_t i = 0; i < ruleFiles.size(); i++) {
    auto results = TestHelpers::runTests(ruleFiles[i]["tests"], ruleFiles[i], symbols);
    int testsSize = results["testsSize"];

    expect(!results.is_null() && testsSize > 0, "No tests ran");

    Helpers::print(
      "rules" + std::to_string(i + 1) + ".json: " +
      std::string(results["message"])
    );

    expect(bool(results["ok"]) == true, results["message"]);
  }

  // Array Object JSON helpers

  auto jsonFile =
      Helpers::getJsonFile(dirPath, "imports.json");
  auto expectedFile =
      Helpers::getJsonFile(dirPath, "importsExpected.json");
  expect(jsonFile.dump() == expectedFile.dump(), "imports.json and importsExpected.json didn't match");

  return 0;
}
