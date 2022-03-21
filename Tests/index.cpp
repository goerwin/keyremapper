#include <iostream>

#if __APPLE__
  #include <mach-o/dyld.h>
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  #include <windows.h>
#endif

#include "../common/vendors/json.hpp"
#include "../common/Helpers.hpp"
#include "../common/TestHelpers.hpp"
#include "../common/KeyRemapper.hpp"

void expect(bool value, std::string errorMsg = "") {
  if (value) return;

  Helpers::print(!errorMsg.empty() ? "Error: " + errorMsg : "Error, expect condition failed");
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

  auto symbols = Helpers::getJsonFile(dirPath + "/symbols.json");

  // TESTS

  auto ruleFiles = nlohmann::json::array({
    Helpers::getJsonFile(dirPath + "/rules1.json"),
    Helpers::getJsonFile(dirPath + "/rules2.json"),
    Helpers::getJsonFile(dirPath + "/rules3.json"),
    Helpers::getJsonFile(dirPath + "/rules4.json"),
    Helpers::getJsonFile(dirPath + "/rules5.json"),
    Helpers::getJsonFile(dirPath + "/rules6.json"),
    Helpers::getJsonFile(dirPath + "/rules7.json"),
    Helpers::getJsonFile(dirPath + "/rules8.json"),
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

  // Test1: Special Keys are properly parsed

  auto t1_keyRemapper = new KeyRemapper(Helpers::getJsonFile(dirPath + "/rules7.json"), symbols);
  auto t1_keyEvents = t1_keyRemapper->getKeyEventsFromString("A SK:kekw:1234 B");

  std::vector<KeyRemapper::KeyEvent> t1_keyEvents2 = {
    {"A", 30, 0, true},
    {"A", 30, 1, false},
    {"SK:kekw", 6969, 1234, true},
    {"B", 48, 0, true},
    {"B", 48, 1, false}
  };

  expect(t1_keyEvents.size() == 5, "Test1: results wrong size");
  expect(t1_keyEvents.size() == t1_keyEvents2.size(), "Test1: results not same size");

  for (size_t i = 0; i < t1_keyEvents.size(); i++) {
    auto keyEvent = t1_keyEvents[i];
    auto keyEvent2 = t1_keyEvents2[i];

    expect(keyEvent.name == keyEvent2.name, "Test1: results not same name");
    expect(keyEvent.code == keyEvent2.code, "Test1: results not same code");
    expect(keyEvent.state == keyEvent2.state, "Test1: results not same state");
    expect(keyEvent.isKeyDown == keyEvent2.isKeyDown, "Test1: results not same isKeyDown");
  }

  expect(t1_keyRemapper->stringifyKeyEvents(t1_keyEvents2) == "A:down A:up SK:kekw:1234 B:down B:up", "Test1: results for stringifyKeyEvents not equal");

  Helpers::print("Test1: Special keys (SK:) tests passed");

  // Test2: Unknown keyEvents passed to applyKeys are passed through

  auto t2_keyRemapper = new KeyRemapper(Helpers::getJsonFile(dirPath + "/rules7.json"), symbols);
  std::vector<KeyRemapper::KeyEvent> t2_keyEvents = {
    {"", 30, 0, false},
    {"", 30, 1, false},
    {"", 420, 0, false},
    {"", 420, 1, false},
  };
  std::vector<std::tuple<std::string, ushort, ushort, bool>> t2_expectedResults = {
    {"A", 30, 0, true},
    {"A", 30, 1, false},
    {"Unknown", 420, 0, false},
    {"Unknown", 420, 1, false},
  };

  auto t2_keyEventsRes = t2_keyRemapper->applyKeys(t2_keyEvents);
  expect(t2_keyEventsRes.size() == t2_keyEvents.size(), "Test2: wrong size");

  for (size_t i = 0; i < t2_keyEventsRes.size(); i++) {
    auto keyEvent = t2_keyEvents[i];
    auto keyEvent2 = t2_keyEventsRes[i];

    expect(keyEvent2.name == std::get<0>(t2_expectedResults[i]), "Test2: not same name");
    expect(keyEvent2.code == std::get<1>(t2_expectedResults[i]), "Test2: not same code");
    expect(keyEvent2.state == std::get<2>(t2_expectedResults[i]), "Test2: not same state");
    expect(keyEvent2.isKeyDown == std::get<3>(t2_expectedResults[i]), "Test2: not same isKeyDown");
  }

  Helpers::print("Test2: Unknown keys not in symbols tests passed");

  // Test3: Unknown keys passed through string are properly handled

  auto t3_keyRemapper = new KeyRemapper(Helpers::getJsonFile(dirPath + "/rules7.json"), symbols);
  auto t3_keyEvents = t3_keyRemapper->getKeyEventsFromString("A NoExist:down NoExist:up B");

  expect(t3_keyRemapper->stringifyKeyEvents(t3_keyEvents) == "A:down A:up Unknown:up Unknown:up B:down B:up", "Test3: Not same string output");

  Helpers::print("Test3: Unknown keys passed through string tests passed");

  // Array Object JSON helpers

  auto jsonFile =
      Helpers::getJsonFile(dirPath + "/imports.json");
  auto expectedFile =
      Helpers::getJsonFile(dirPath + "/importsExpected.json");
  expect(jsonFile.dump() == expectedFile.dump(), "imports.json and importsExpected.json didn't match");

  Helpers::print("SUCCESS!");
  return 0;
}
