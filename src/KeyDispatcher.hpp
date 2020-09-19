#pragma once

#include <vector>
#include <string>
#include "libraries/json.hpp"
#include "symbols.hpp"
#include "helpers.hpp"

class KeyDispatcher
{
  using json = nlohmann::json;
  using String = std::string;

public:
  static struct Key
  {
    unsigned short code;
    unsigned short state;

    Key() {}

    Key(unsigned short cCode, unsigned short cState = 4)
    {
      code = cCode;
      state = cState;
    }
  };

  static struct KeyDown : Key
  {
    KeyDown(unsigned short cCode, unsigned short cState = 0) : Key(cCode, cState) {}
  };

  static struct KeyUp : Key
  {
    KeyUp(unsigned short cCode, unsigned short cState = 1) : Key(cCode, cState) {}
  };

  typedef std::vector<Key> Keys;

private:
  const short SC_H = Symbols::getSymbolScanCode("H");
  const short SC_J = Symbols::getSymbolScanCode("J");
  const short SC_K = Symbols::getSymbolScanCode("K");
  const short SC_L = Symbols::getSymbolScanCode("L");
  const short SC_Up = Symbols::getSymbolScanCode("Up");
  const short SC_Down = Symbols::getSymbolScanCode("Down");
  const short SC_Right = Symbols::getSymbolScanCode("Right");
  const short SC_Left = Symbols::getSymbolScanCode("Left");
  const short SC_Home = Symbols::getSymbolScanCode("Home");
  const short SC_End = Symbols::getSymbolScanCode("End");
  const short SC_Ctrl = Symbols::getSymbolScanCode("Ctrl");
  const short SC_Alt = Symbols::getSymbolScanCode("Alt");

  json globals = {};
  json jsonSchema;
  json modifiers;
  json tests;

public:
  KeyDispatcher(json jsonSchemaEl)
  {
    jsonSchema = jsonSchemaEl;
    modifiers = jsonSchemaEl["modifiers"];
    tests = jsonSchemaEl["tests"];
  }

  Keys getKeyEvents(Keys keys)
  {
    Keys allKeys;
    int size = keys.size();

    for (int i = 0; i < size; i++)
    {
      Key key = keys[i];
      Keys keys = {};
      auto keyCode = key.code;

      bool isKeyDownEl = isKeyDown(key);
      auto keyValue = Symbols::getSymbolByScanCode(keyCode);
      globals["currentKey"] = keyValue;
      globals["currentKeyDown"] = isKeyDownEl;
      globals[keyValue] = isKeyDownEl;

      auto fireKeysRes = getFireKeysFromModifiers();
      auto fireKeys = fireKeysRes["fireKeys"];

      if (!fireKeys.is_null())
      {
        if (isKeyDownEl)
        {
          keys = fireKeys[0].is_null() ? keys : concatKeys(keys, parseKeys(fireKeys[0]));
        }
        else
        {
          keys = fireKeys[1].is_null() ? keys : concatKeys(keys, parseKeys(fireKeys[1]));
        }
      }
      else
      {
        if (isKeyDownEl)
        {
          keys = concatKeys(keys, {KeyDown(keyCode)});
        }
        else
        {
          keys = concatKeys(keys, {KeyUp(keyCode)});
        }
      }

      Helpers::print(stringifyKeys({key}) + " ==> " + stringifyKeys(keys));
      allKeys = concatKeys(allKeys, keys);
    }

    return allKeys;
  }

  static String stringifyKeys(Keys keys)
  {
    String result = "";
    auto keysSize = keys.size();
    for (auto i = 0; i < keysSize; i++)
    {
      if (i != 0)
        result += " ";

      auto key = keys[i];
      result += Symbols::getSymbolByScanCode(key.code) + (isKeyDown(key) ? ":down" : ":up");
    }

    return result;
  }

  Keys parseKeys(String str)
  {
    if (str.empty())
    {
      return {};
    }

    auto strKeys = Helpers::split(Helpers::trim(str), ' ');
    auto strKeysSize = strKeys.size();
    auto currentKey = globals["currentKey"];
    Keys keys = {};

    for (auto i = 0; i < strKeysSize; i++)
    {
      auto strKey = strKeys[i];
      auto keyDesc = Helpers::split(strKey, ':');
      auto keySymbol = keyDesc[0];
      String keyStatusStr;
      short keyCode;

      if (keyDesc.size() == 2)
        keyStatusStr = keyDesc[1];

      if (keySymbol == "currentKey")
      {
        keyCode = Symbols::getSymbolScanCode(currentKey);
      }
      else if (keySymbol == "vimArrowKey")
      {
        keyCode = getVimArrowKeyCode(currentKey);
      }
      else if (keySymbol == "vimHomeEndKey")
      {
        keyCode = getVimHomeEndKeyCode(currentKey);
      }
      else
      {
        keyCode = Symbols::getSymbolScanCode(keySymbol);
      }

      if (keyStatusStr == "down")
      {
        keys = concatKeys(keys, {KeyDown(keyCode)});
      }
      else if (keyStatusStr == "up")
      {
        keys = concatKeys(keys, {KeyUp(keyCode)});
      }
      else
      {
        keys = concatKeys(keys, {KeyDown(keyCode)});
        keys = concatKeys(keys, {KeyUp(keyCode)});
      }
    }

    return keys;
  }

  json runTests()
  {
    if (tests.is_null())
    {
      return {};
    }

    bool ok = true;
    auto testsSize = tests.size();
    String message = "ALL TESTS PASSED! Number of tests: " + std::to_string(testsSize);

    for (int i = 0; i < testsSize; i++)
    {
      globals = {};

      auto test = tests[i];
      auto inputKeys = parseKeys(test[0]);
      String expectedKeysStr = test[1];
      auto expectedKeys = parseKeys(expectedKeysStr);
      auto resultKeysStr = stringifyKeys(getKeyEvents(inputKeys));

      if (resultKeysStr != stringifyKeys(expectedKeys))
      {
        ok = false;
        message = "TEST FAILED: expected \"" + expectedKeysStr + "\", got \n\"" + resultKeysStr + "\"";
        break;
      }
    }

    return {
      { "ok", ok },
      { "testsSize", testsSize },
      { "message", message }
    };
  }

private:
  Keys concatKeys(Keys keys, Keys keys2, Keys keys3 = {}, Keys keys4 = {})
  {
    keys.insert(keys.end(), keys2.begin(), keys2.end());
    keys.insert(keys.end(), keys3.begin(), keys3.end());
    keys.insert(keys.end(), keys4.begin(), keys4.end());
    return keys;
  }

  short getVimArrowKeyCode(String key)
  {
    auto keyCode = key.empty() ? 0 : Symbols::getSymbolScanCode(key);
    return keyCode == SC_K ? SC_Up : keyCode == SC_J ? SC_Down : keyCode == SC_L ? SC_Right : keyCode == SC_H ? SC_Left : 0;
  }

  short getVimHomeEndKeyCode(String key)
  {
    auto keyCode = key.empty() ? 0 : Symbols::getSymbolScanCode(key);
    return (keyCode == SC_K || keyCode == SC_H) ? SC_Home : (keyCode == SC_J || keyCode == SC_L) ? SC_End : 0;
  }

  static bool isKeyDown(Key key)
  {
    return key.state == 0 || key.state == 2;
  }

  static bool isKeyMatches(json ruleKey, std::string currentKey)
  {
    if (ruleKey.is_null())
      return true;
    if (ruleKey.is_string())
      return ruleKey == currentKey;

    for (auto i = 0; i < ruleKey.size(); i++)
    {
      if (ruleKey[i] == currentKey)
        return true;
    }

    return false;
  }

  bool isConditions(json conditions)
  {
    if (conditions.is_null())
      return true;

    for (auto &[key, value] : conditions.items())
    {
      auto globalValue = globals[key];

      if (globalValue.is_null())
      {
        if (value != false)
          return false;
        continue;
      }

      if (value != globalValue)
      {
        return false;
      }
    }

    return true;
  }

  void setValues(json values)
  {
    if (values.is_null())
      return;

    for (auto &[key, value] : values.items())
    {
      globals[key] = value;
    }
  }

  json getFireKeysFromRules(json rules)
  {
    for (auto i = 0; i < rules.size(); i++)
    {
      auto rule = rules[i];
      auto ruleKey = rule["key"];
      auto conditions = rule["conditions"];
      auto fire = rule["fire"];
      auto currentKey = globals["currentKey"];

      bool keyMatches = isKeyMatches(ruleKey, currentKey);
      bool isConditionsEl = isConditions(conditions);

      if (!keyMatches || !isConditionsEl)
      {
        continue;
      }

      setValues(rule["set"]);

      if (!fire.is_null())
      {
        return fire;
      }

      return getFireKeysFromRules(rule["rules"]);
    }

    return {};
  }

  json getFireKeysFromModifiers()
  {
    auto currentKey = globals["currentKey"];

    for (auto i = 0; i < modifiers.size(); i++)
    {
      auto modifierValue = modifiers[i];
      auto modifier = modifierValue["modifier"];
      auto skipModifier = false;
      auto modifierSize = modifier.size();

      for (auto j = 0; modifierSize >= 2 && j <= modifierSize - 2; j++)
      {
        auto mod = std::string(modifier[j]);
        auto modKeyDownStatus = globals[mod];

        if (modKeyDownStatus.is_null() || modKeyDownStatus == false)
        {
          skipModifier = true;
          break;
        }
      }

      std::string lastMod = modifier[modifierSize - 1];
      bool lastModIsCurrentKey = lastMod == currentKey;

      if (!skipModifier) {
        if (!lastModIsCurrentKey && globals[lastMod] != true) {
          skipModifier = true;
        }
      }

      if (skipModifier)
      {
        continue;
      }

      auto fire = modifierValue["fire"];

      if (!fire.is_null() && lastModIsCurrentKey) {
        return { { "fireKeys", fire } };
      }

      auto rules = modifierValue["rules"];
      json response;

      if (!rules.is_null()) {
        response = getFireKeysFromRules(rules);
      }

      if (!response.is_null()) {
        return {{ "fireKeys", response }};
      }
    }

    return {};
  }
};
