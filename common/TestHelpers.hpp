#pragma once

#include "vendors/json.hpp"
#include "KeyDispatcher.hpp"

using json = nlohmann::json;
typedef std::string String;
typedef unsigned short ushort;

namespace TestHelpers {
json runTests(json tests, json rules, json symbols) {
  if (tests.is_null())
    return {};

  auto keyDispatcher = new KeyDispatcher(rules, symbols);

  bool ok = true;
  auto testsSize = tests.size();
  String message =
      "ALL TESTS PASSED! Number of tests: " + std::to_string(testsSize);

  for (size_t i = 0; i < testsSize; i++) {
    std::vector<String> test = tests[i];
    auto inputKeysStr = test[0];
    auto testSize = test.size();

    keyDispatcher->reset();

    auto resultKeyEvents = keyDispatcher->getKeyEventsFromString("");
    std::stringstream ss(inputKeysStr);

    if (testSize == 3) keyDispatcher->setAppName(test[2]);

    while(ss.good()) {
      String item;
      getline(ss, item, ' ');
      String delayKey = "__delay";
      auto delayTokenIdx = item.find(delayKey);

      if (delayTokenIdx != std::string::npos) {
        auto delayTimeStr = item.substr(delayKey.size(), item.size());
        int delayTimeMs = atoi(delayTimeStr.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(delayTimeMs));
      } else {
        auto inputKey = keyDispatcher->getKeyEventsFromString(item);
        auto resKeyEvents = keyDispatcher->applyKeys({inputKey});
        resultKeyEvents = Helpers::concatArrays(resultKeyEvents, resKeyEvents);
      }
    }

    String expectedKeysStr = test[1];
    auto expectedKeys = keyDispatcher->getKeyEventsFromString(expectedKeysStr);
    auto resultKeysStr = keyDispatcher->stringifyKeyEvents(resultKeyEvents);

    if (resultKeysStr != keyDispatcher->stringifyKeyEvents(expectedKeys)) {
      ok = false;
      message = "TEST " + std::to_string(i) + " FAILED: expected \"" +
                expectedKeysStr + "\", got \n\"" + resultKeysStr + "\"";
      break;
    }
  }

  return {{"ok", ok}, {"testsSize", testsSize}, {"message", message}};
}
}
