#include "pch.h"
#include "../src/KeyDispatcher.hpp"
#include "../src/libraries/json.hpp"
#include "../src/helpers.hpp"

using json = nlohmann::json;

TEST(KeyDispatcher_test, find_correct_fireKeys)
{
  auto symbols = Helpers::getJsonFile("./symbols.json");

  json ruleFiles = json::array({
      Helpers::getJsonFile("./rules1.json"),
      Helpers::getJsonFile("./rules2.json"),
      Helpers::getJsonFile("./rules3.json"),
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
