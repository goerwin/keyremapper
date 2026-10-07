#pragma once

#include <memory>

#include "KeyRemapper.hpp"
#include "vendors/json.hpp"

using json = nlohmann::json;
typedef std::string String;
typedef unsigned short ushort;

namespace TestHelpers {
json runTests(json tests, json profile, json symbols) {
  if (tests.is_null()) return {};

  auto keyRemapper = std::make_unique<KeyRemapper>(profile, symbols);
  // test_delay moves it, so slow machines (CI) don't change the results
  double time = 1;
  keyRemapper->now = [&time] { return time; };

  bool ok = true;
  auto testsSize = tests.size();
  String message =
      "ALL TESTS PASSED! Number of tests: " + std::to_string(testsSize);

  for (size_t i = 0; i < testsSize; i++) {
    std::vector<String> test = tests[i];
    auto inputKeysStr = test[0];

    keyRemapper->reset();

    auto resultKeyEvents = keyRemapper->getKeyEventsFromString("");
    std::stringstream ss(inputKeysStr);

    while (ss.good()) {
      String item;
      getline(ss, item, ' ');
      String delayKey = "test_delay:";
      String appNameKey = "appName:";
      String keyboardKey = "keyboard:";
      auto delayTokenIdx = item.find(delayKey);
      auto appNameTokenIdx = item.find(appNameKey);
      auto keyboardTokenIdx = item.find(keyboardKey);

      if (keyboardTokenIdx != std::string::npos) {
        auto keyboard = item.substr(keyboardKey.size(), item.size());
        keyboard = keyboard == "_" ? "" : keyboard;
        keyRemapper->setKeyboard(keyboard, "keyboard description");
        continue;
      }

      if (appNameTokenIdx != std::string::npos) {
        auto appName = item.substr(appNameKey.size(), item.size());
        appName = appName == "_" ? "" : appName;
        keyRemapper->setAppName(appName);
        continue;
      }

      // As if the held key's ifHeldFor time passed
      if (item == "test_hold") {
        resultKeyEvents =
            Helpers::concatArrays(resultKeyEvents, keyRemapper->applyHold());
        continue;
      }

      if (delayTokenIdx != std::string::npos) {
        auto delayTimeStr = item.substr(delayKey.size(), item.size());
        time += atoi(delayTimeStr.c_str());
        continue;
      }

      auto inputKeys = keyRemapper->getKeyEventsFromString(item);
      auto resKeyEvents = keyRemapper->applyKeys(inputKeys);
      resultKeyEvents = Helpers::concatArrays(resultKeyEvents, resKeyEvents);
    }

    String expectedKeysStr = test[1];
    auto expectedKeys = keyRemapper->getKeyEventsFromString(expectedKeysStr);
    auto resultKeysStr = keyRemapper->stringifyKeyEvents(resultKeyEvents);

    if (resultKeysStr != keyRemapper->stringifyKeyEvents(expectedKeys)) {
      ok = false;
      message = "TEST " + std::to_string(i) + " FAILED: expected \"" +
                expectedKeysStr + "\", got \n\"" + resultKeysStr + "\"";
      break;
    }
  }

  return {{"ok", ok}, {"testsSize", testsSize}, {"message", message}};
}
}  // namespace TestHelpers
