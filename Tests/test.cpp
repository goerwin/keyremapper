#include "pch.h"
#include "../src/KeyDispatcher.hpp"
#include "../src/libraries/json.hpp"
#include "../src/helpers.hpp"

using json = nlohmann::json;

TEST(KeyDispatcher_test, find_correct_fireKeys)
{
  json schemas = json::array({
    Helpers::getJsonSchema("./schema1.json"),
    Helpers::getJsonSchema("./schema2.json"),
    Helpers::getJsonSchema("./schema3.json"),
    Helpers::getJsonSchema("./schema4.json")
  });

  for (auto i = 0; i < schemas.size(); i++)
  {
    auto keyDispatcher = new KeyDispatcher(schemas[i]);
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
