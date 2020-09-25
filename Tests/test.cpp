#include "pch.h"
#include <chrono>
#include <thread>
#include "../src/KeyDispatcher.hpp"
#include "../src/libraries/json.hpp"
#include "../src/helpers.hpp"

using json = nlohmann::json;

TEST(KeyDispatcher_test, find_correct_fireKeys)
{
  auto symbols = Helpers::getJsonFile("./symbols.json");

  auto ruleFiles = json::array({
      Helpers::getJsonFile("./rules1.json"),
      Helpers::getJsonFile("./rules2.json"),
      Helpers::getJsonFile("./rules3.json"),
      Helpers::getJsonFile("./rules4.json"),
  });

  for (auto i = 0; i < ruleFiles.size(); i++)
  {
    auto keyDispatcher = new KeyDispatcher(ruleFiles[i], symbols);
    auto results = keyDispatcher->runTests();
    int testsSize = results["testsSize"];

    if (results.is_null() || testsSize == 0)
    {
      FAIL();
    }

    std::cout << "test " + std::to_string(i) + ": " + std::string(results["message"]) << std::endl;

    EXPECT_EQ(bool(results["ok"]), true);
  }
}

TEST(KeyDispatcher_test, keyPresses)
{
  auto symbols = Helpers::getJsonFile("./symbols.json");
  auto ruleFile = Helpers::getJsonFile("./rules5.json");
  std::string resultKeysStr;

  auto keyDispatcher = new KeyDispatcher(ruleFile, symbols);
  auto inputKeys = keyDispatcher->getKeyEventsFromString("");
  auto expectedKeys = keyDispatcher->getKeyEventsFromString("");
  auto partialKeyEvents = keyDispatcher->getKeyEventsFromString("");

  keyDispatcher->reset();
  inputKeys = keyDispatcher->getKeyEventsFromString("A Caps");
  expectedKeys = keyDispatcher->getKeyEventsFromString("B Caps Esc");
  resultKeysStr = keyDispatcher->stringifyKeyEvents(keyDispatcher->applyKeys(inputKeys));
  EXPECT_EQ(resultKeysStr, keyDispatcher->stringifyKeyEvents(expectedKeys));

  keyDispatcher->reset();
  inputKeys = keyDispatcher->getKeyEventsFromString("A:down Caps:down A:up Caps:up");
  expectedKeys = keyDispatcher->getKeyEventsFromString("B:down Caps:down B:up Caps:up");
  resultKeysStr = keyDispatcher->stringifyKeyEvents(keyDispatcher->applyKeys(inputKeys));
  EXPECT_EQ(resultKeysStr, keyDispatcher->stringifyKeyEvents(expectedKeys));

  keyDispatcher->reset();
  inputKeys = keyDispatcher->getKeyEventsFromString("Win");
  expectedKeys = keyDispatcher->getKeyEventsFromString("");
  resultKeysStr = keyDispatcher->stringifyKeyEvents(keyDispatcher->applyKeys(inputKeys));
  EXPECT_EQ(resultKeysStr, keyDispatcher->stringifyKeyEvents(expectedKeys));

  keyDispatcher->reset();
  inputKeys = keyDispatcher->getKeyEventsFromString("Win Win C");
  expectedKeys = keyDispatcher->getKeyEventsFromString("Shift C");
  resultKeysStr = keyDispatcher->stringifyKeyEvents(keyDispatcher->applyKeys(inputKeys));
  EXPECT_EQ(resultKeysStr, keyDispatcher->stringifyKeyEvents(expectedKeys));

  keyDispatcher->reset();
  inputKeys = keyDispatcher->getKeyEventsFromString("Win C:down Win");
  expectedKeys = keyDispatcher->getKeyEventsFromString("C:down");
  resultKeysStr = keyDispatcher->stringifyKeyEvents(keyDispatcher->applyKeys(inputKeys));
  EXPECT_EQ(resultKeysStr, keyDispatcher->stringifyKeyEvents(expectedKeys));

  keyDispatcher->reset();
  inputKeys = keyDispatcher->getKeyEventsFromString("Caps");
  expectedKeys = keyDispatcher->getKeyEventsFromString("Caps Esc");
  resultKeysStr = keyDispatcher->stringifyKeyEvents(keyDispatcher->applyKeys(inputKeys));
  EXPECT_EQ(resultKeysStr, keyDispatcher->stringifyKeyEvents(expectedKeys));

  keyDispatcher->reset();
  inputKeys = keyDispatcher->getKeyEventsFromString("Alt:down Caps Alt:up");
  expectedKeys = keyDispatcher->getKeyEventsFromString("Alt:down Caps Esc Alt:up");
  resultKeysStr = keyDispatcher->stringifyKeyEvents(keyDispatcher->applyKeys(inputKeys));
  EXPECT_EQ(resultKeysStr, keyDispatcher->stringifyKeyEvents(expectedKeys));

  keyDispatcher->reset();
  inputKeys = keyDispatcher->getKeyEventsFromString("A Caps:down");
  partialKeyEvents = keyDispatcher->applyKeys(inputKeys);
  inputKeys = keyDispatcher->getKeyEventsFromString("Caps:up");
  partialKeyEvents = Helpers::concatArrays(partialKeyEvents, keyDispatcher->applyKeys(inputKeys));
  resultKeysStr = keyDispatcher->stringifyKeyEvents(partialKeyEvents);
  expectedKeys = keyDispatcher->getKeyEventsFromString("B Caps Esc");
  EXPECT_EQ(resultKeysStr, keyDispatcher->stringifyKeyEvents(expectedKeys));

  keyDispatcher->reset();
  inputKeys = keyDispatcher->getKeyEventsFromString("A Caps:down");
  partialKeyEvents = keyDispatcher->applyKeys(inputKeys);
  inputKeys = keyDispatcher->getKeyEventsFromString("Caps:up");
  std::this_thread::sleep_for(std::chrono::milliseconds(11));
  partialKeyEvents = Helpers::concatArrays(partialKeyEvents, keyDispatcher->applyKeys(inputKeys));
  resultKeysStr = keyDispatcher->stringifyKeyEvents(partialKeyEvents);
  expectedKeys = keyDispatcher->getKeyEventsFromString("B Caps");
  EXPECT_EQ(resultKeysStr, keyDispatcher->stringifyKeyEvents(expectedKeys));
}
