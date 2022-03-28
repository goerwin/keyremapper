#pragma once

#include "vendors/json.hpp"
#include "KeyRemapper.hpp"

using json = nlohmann::json;
typedef std::string String;
typedef unsigned short ushort;

namespace TestHelpers {
json runTests(json tests, json rules, json symbols) {
  if (tests.is_null())
    return {};

  auto keyRemapper = new KeyRemapper(rules, symbols);

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

    while(ss.good()) {
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

      if (delayTokenIdx != std::string::npos) {
        auto delayTimeStr = item.substr(delayKey.size(), item.size());
        int delayTimeMs = atoi(delayTimeStr.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(delayTimeMs));
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
}
