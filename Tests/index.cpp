#include <mach-o/dyld.h>

#include <iostream>

#include "../common/Helpers.hpp"
#include "../common/KeyRemapper.hpp"
#include "../common/TestHelpers.hpp"
#include "../common/vendors/json.hpp"

using json = nlohmann::json;

void expect(bool value, std::string errorMsg) {
  if (value) return;

  Helpers::print("Error: " + errorMsg);
  exit(1);
}

bool throws(std::function<void()> fn) {
  try {
    fn();
  } catch (const std::runtime_error &) {
    return true;
  }
  return false;
}

int main(int argc, const char *argv[]) {
  char path[1024];
  uint32_t size = sizeof(path);
  if (_NSGetExecutablePath(path, &size) != 0) {
    printf("buffer too small; need size %u\n", size);
    return 1;
  }

  std::string dirPath = path;
  dirPath = dirPath.substr(0, dirPath.find_last_of('/'));

  auto symbols = Helpers::getJsonFile(
      dirPath + "/../mac/KeyRemapper/Resources/symbols.json");

  for (auto name : {"send", "modifiers", "conditions", "taps", "holds", "keys",
                    "vim"}) {
    auto profile = Helpers::getJsonFile(dirPath + "/" + name + ".json");
    auto results = TestHelpers::runTests(profile["tests"], profile, symbols);

    expect(!results.is_null() && int(results["testsSize"]) > 0,
           std::string(name) + ".json: no tests ran");
    Helpers::print(std::string(name) +
                   ".json: " + std::string(results["message"]));
    expect(bool(results["ok"]), results["message"]);
  }

  // Key events from and to strings

  KeyRemapper keyRemapper(json::object(), symbols);
  auto keyEvents = keyRemapper.getKeyEventsFromString(
      "A CmdL+B:down CmdL+B:up delay:25 currentKey NoExist", "C");
  expect(keyRemapper.stringifyKeyEvents(keyEvents) ==
             "A:down A:up CmdL:down B:down B:up CmdL:up delay:25 C:down C:up "
             "Unknown:down Unknown:up",
         "getKeyEventsFromString/stringifyKeyEvents");
  expect(keyEvents[2].code == 227 && keyEvents[2].state == 0 &&
             keyEvents[3].code == 5 && keyEvents[4].state == 1,
         "getKeyEventsFromString codes and states");

  // Key codes without a symbol are dropped

  expect(keyRemapper.applyKeys({{"", 999, 0, true}, {"", 4, 0, true}}).size() ==
             1,
         "applyKeys with an unknown key code");

  Helpers::print("Key events tests passed");

  // Invalid profiles fail when they load

  for (auto profile : {
           R"({ "keybindings": [] })",
           R"({ "tapDelay": "100" })",
           R"({ "rules": {} })",
           R"({ "rules": [{ "send": "A" }] })",
           R"({ "rules": [{ "keys": [] }] })",
           R"({ "rules": [{ "keys": ["NoExist"] }] })",
           R"({ "rules": [{ "keys": ["A"], "if": {} }] })",
           R"({ "rules": [{ "keys": ["A"], "modifiers": ["NoExist"] }] })",
           R"({ "rules": [{ "keys": ["A"], "optional": ["NoExist"] }] })",
           R"({ "rules": [{ "keys": ["A"], "app": 1 }] })",
           R"({ "rules": [{ "keys": ["A"], "send": "NoExist" }] })",
           R"({ "rules": [{ "keys": ["A"], "send": "A:up" }] })",
           R"({ "rules": [{ "keys": ["A"], "tap": "delay:x" }] })",
           R"({ "rules": [{ "keys": ["A", "B"], "hold": ["C"] }] })",
           R"({ "rules": [{ "keys": ["A"], "send": ["C", 1] }] })",
           R"({ "remaps": [{ "from": "A" }] })",
           R"({ "remaps": [{ "from": "A", "to": "NoExist" }] })",
           R"({ "remaps": [{ "from": "A", "to": "B", "if": {} }] })",
       }) {
    expect(throws([&] { KeyRemapper(json::parse(profile), symbols); }),
           std::string("no error for the profile ") + profile);
  }

  expect(!throws([&] {
           KeyRemapper(json::parse(R"({
             "name": "Profile",
             "tapDelay": 100,
             "holdDelay": 300,
             "remaps": [{ "from": "A", "to": "B", "app": ["com.app"] }],
             "rules": [{
               "keys": ["A", "B"],
               "modifiers": ["CmdL", "F"],
               "optional": ["any"],
               "keyboard": "1",
               "send": ["CmdL+C:down delay:5 currentKey", "D"]
             }]
           })"),
                       symbols);
         }),
         "error for a valid profile");

  Helpers::print("Profile validation tests passed");

  // Imports

  expect(Helpers::getJsonFile(dirPath + "/imports.json") ==
             Helpers::getJsonFile(dirPath + "/importsExpected.json"),
         "imports.json and importsExpected.json didn't match");
  expect(throws([&] { Helpers::getJsonFile(dirPath + "/noExist.json"); }),
         "no error for a missing file");

  Helpers::print("Imports tests passed");

  Helpers::print("SUCCESS!");
  return 0;
}
