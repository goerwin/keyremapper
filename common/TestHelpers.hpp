#pragma once

#include <memory>

#include "KeyRemapper.hpp"
#include "vendors/json.hpp"

using json = nlohmann::json;
typedef std::string String;
typedef unsigned short ushort;

namespace TestHelpers {
// Each test is [input, expected]. The input has the keys pressed, "delay:N" (N
// ms pass, firing the hold of the held key like the real timer), "app:X" and
// "keyboard:X" (empty X for none)
json runTests(json tests, json profile, json symbols) {
  if (tests.is_null()) return {};

  auto keyRemapper = std::make_unique<KeyRemapper>(profile, symbols);
  // delay moves it, so slow machines (CI) don't change the results
  double time = 1;
  double lastKeyTime = time;
  keyRemapper->now = [&time] { return time; };

  bool ok = true;
  auto testsSize = tests.size();
  String message =
      "ALL TESTS PASSED! Number of tests: " + std::to_string(testsSize);

  for (size_t i = 0; i < testsSize; i++) {
    std::vector<String> test = tests[i];
    auto inputKeysStr = test[0];

    keyRemapper->reset();

    KeyRemapper::KeyEvents resultKeyEvents = {};
    std::stringstream ss(inputKeysStr);

    while (ss.good()) {
      String item;
      getline(ss, item, ' ');
      if (item.empty()) continue;

      String delayKey = "delay:";
      String appKey = "app:";
      String keyboardKey = "keyboard:";

      if (item.rfind(keyboardKey, 0) == 0) {
        keyRemapper->setKeyboard(item.substr(keyboardKey.size()),
                                 "keyboard description");
        continue;
      }

      if (item.rfind(appKey, 0) == 0) {
        keyRemapper->setAppName(item.substr(appKey.size()));
        continue;
      }

      if (item.rfind(delayKey, 0) == 0) {
        auto endTime = time + atoi(item.substr(delayKey.size()).c_str());
        auto holdDelay = keyRemapper->getHoldDelay();

        if (holdDelay >= 0 && endTime >= lastKeyTime + holdDelay) {
          time = lastKeyTime + holdDelay;
          resultKeyEvents =
              Helpers::concatArrays(resultKeyEvents, keyRemapper->applyHold());
        }

        time = endTime;
        continue;
      }

      for (auto &inputKey : keyRemapper->getKeyEventsFromString(item)) {
        resultKeyEvents = Helpers::concatArrays(
            resultKeyEvents, keyRemapper->applyKeys({inputKey}));
        lastKeyTime = time;
      }
    }

    String expectedKeysStr = test[1];
    auto expectedKeys = keyRemapper->getKeyEventsFromString(expectedKeysStr);
    auto resultKeysStr = keyRemapper->stringifyKeyEvents(resultKeyEvents);

    if (resultKeysStr != keyRemapper->stringifyKeyEvents(expectedKeys)) {
      ok = false;
      message = "TEST " + std::to_string(i) + " (\"" + inputKeysStr +
                "\") FAILED: expected \"" + expectedKeysStr + "\", got \n\"" +
                resultKeysStr + "\"";
      break;
    }
  }

  return {{"ok", ok}, {"testsSize", testsSize}, {"message", message}};
}
}  // namespace TestHelpers
