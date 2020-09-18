#pragma once

#include <vector>
#include <string>
#include "json.hpp"
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

  bool isCtrlAsAlt;
  bool isAltAsCtrl;
  json keyDownStatuses = {};
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
      keyDownStatuses["currentKey"] = keyValue;
      keyDownStatuses["currentKeyDown"] = isKeyDownEl;
      keyDownStatuses[keyValue] = isKeyDownEl;

      auto fireKeysRes = getFireKeys();

      if (!fireKeysRes.is_null())
      {
        auto fireKeys = fireKeysRes["fireKeys"];
        auto interceptAll = fireKeysRes["interceptAll"];
        auto isCurrentKeyInModifier = fireKeysRes["isCurrentKeyInModifier"];

        if (isKeyDownEl)
        {
          keys = fireKeys[0].is_null() ? keys : concatKeys(keys, parseKeys(fireKeys[0]));
        }
        else
        {
          keys = fireKeys[1].is_null() ? keys : concatKeys(keys, parseKeys(fireKeys[1]));
        }

        if (!interceptAll && !isCurrentKeyInModifier)
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
    auto currentKeyCode = keyDownStatuses["currentKey"];
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

      if (keySymbol == "keyDownAltAsCtrl")
      {
        keys = concatKeys(keys, keyDownAltAsCtrl());
        continue;
      }
      else if (keySymbol == "keyDownAltAsAlt")
      {
        keys = concatKeys(keys, keyDownAltAsAlt());
        continue;
      }
      else if (keySymbol == "keyUpAlt")
      {
        keys = concatKeys(keys, keyUpAlt());
        continue;
      }
      else if (keySymbol == "keyDownCtrlAsCtrl")
      {
        keys = concatKeys(keys, keyDownCtrlAsCtrl());
        continue;
      }
      else if (keySymbol == "keyDownCtrlAsAlt")
      {
        keys = concatKeys(keys, keyDownCtrlAsAlt());
        continue;
      }
      else if (keySymbol == "keyUpCtrl")
      {
        keys = concatKeys(keys, keyUpCtrl());
        continue;
      }
      else if (keySymbol == "currentKey")
      {
        keyCode = Symbols::getSymbolScanCode(currentKeyCode);
      }
      else if (keySymbol == "vimArrowKey")
      {
        keyCode = getVimArrowKeyCode(currentKeyCode);
      }
      else if (keySymbol == "vimHomeEndKey")
      {
        keyCode = getVimHomeEndKeyCode(currentKeyCode);
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

  String runTests()
  {
    if (tests.is_null())
    {
      return "NO TESTS FOUND";
    }

    for (int i = 0; i < tests.size(); i++)
    {
      keyDownStatuses = {};

      auto test = tests[i];
      auto inputKeys = parseKeys(test[0]);
      String expectedKeysStr = test[1];
      auto expectedKeys = parseKeys(expectedKeysStr);
      auto resultKeysStr = stringifyKeys(getKeyEvents(inputKeys));

      if (resultKeysStr != stringifyKeys(expectedKeys))
      {
        return "TEST FAILED: expected \"" + expectedKeysStr + "\" but got \"" + resultKeysStr + "\"";
      }
    }

    return "ALL TESTS PASSED! Number of tests: " + std::to_string(tests.size());
  }

private:
  Keys concatKeys(Keys keys, Keys keys2, Keys keys3 = {}, Keys keys4 = {})
  {
    keys.insert(keys.end(), keys2.begin(), keys2.end());
    keys.insert(keys.end(), keys3.begin(), keys3.end());
    keys.insert(keys.end(), keys4.begin(), keys4.end());
    return keys;
  }

  Keys keyDownCtrlAsAlt()
  {
    Keys keys;

    if (!isCtrlAsAlt && keyDownStatuses["Ctrl"])
    {
      keys.insert(keys.begin(), {KeyUp(SC_Ctrl)});
    }

    isCtrlAsAlt = true;
    keys.insert(keys.end(), {KeyDown(SC_Alt)});
    return keys;
  }
  Keys keyDownCtrlAsCtrl()
  {
    Keys keys;

    if (isCtrlAsAlt && keyDownStatuses["Ctrl"])
    {
      keys.insert(keys.begin(), {KeyUp(SC_Alt)});
    }

    isCtrlAsAlt = false;
    keys.insert(keys.end(), {KeyDown(SC_Ctrl)});
    return keys;
  }
  Keys keyUpCtrl()
  {
    Keys keys;

    if (isCtrlAsAlt)
    {
      keys.insert(keys.begin(), {KeyUp(SC_Alt)});
    }
    else
    {
      keys.insert(keys.begin(), {KeyUp(SC_Ctrl)});
    }

    isCtrlAsAlt = true;
    return keys;
  }

  Keys keyDownAltAsCtrl()
  {
    Keys keys;

    if (!isAltAsCtrl && keyDownStatuses["Alt"])
    {
      keys.insert(keys.begin(), {KeyUp(SC_Alt)});
    }

    isAltAsCtrl = true;
    keys.insert(keys.end(), {KeyDown(SC_Ctrl)});
    return keys;
  }
  Keys keyDownAltAsAlt()
  {
    Keys keys;

    if (isAltAsCtrl && keyDownStatuses["Alt"])
    {
      keys.insert(keys.begin(), {KeyUp(SC_Ctrl)});
    }

    isAltAsCtrl = false;
    keys.insert(keys.end(), {KeyDown(SC_Alt)});
    return keys;
  }
  Keys keyUpAlt()
  {
    Keys keys;

    if (isAltAsCtrl)
    {
      keys.insert(keys.begin(), {KeyUp(SC_Ctrl)});
    }
    else
    {
      keys.insert(keys.end(), {KeyUp(SC_Alt)});
    }

    isAltAsCtrl = true;
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

  static bool isKeyMatches(json ruleKey, std::string key)
  {
    if (ruleKey.is_null())
    {
      return true;
    }
    else if (ruleKey.is_string())
    {
      return ruleKey == key;
    }
    else if (ruleKey.is_array())
    {
      for (auto k = ruleKey.begin(); k != ruleKey.end(); ++k)
      {
        if (k.value() == key)
        {
          return true;
        }
      }
    }

    return false;
  }

  bool isConditionMatches(json conditions)
  {
    if (conditions.is_object())
    {
      for (json::iterator i = conditions.begin(); i != conditions.end(); ++i)
      {
        auto key = i.key();
        auto value = i.value();
        auto keyStatus = keyDownStatuses[key];

        if (value == "isDown")
        {
          if (keyStatus.is_null() || keyStatus == false)
          {
            return false;
          }
        }
        else if (value == "isUp")
        {
          if (keyStatus == true)
          {
            return false;
          }
        }
      }
    }

    return true;
  }

  json getFireKeysFromRules(json rules)
  {
    for (auto i = 0; i < rules.size(); i++)
    {
      auto rule = rules[i];
      auto ruleKey = rule["key"];
      auto conditions = rule["conditions"];
      auto fire = rule["fire"];
      auto currentKey = keyDownStatuses["currentKey"];

      bool keyMatches = isKeyMatches(ruleKey, currentKey);
      bool conditionMatches = isConditionMatches(conditions);

      if (!keyMatches || !conditionMatches)
      {
        continue;
      }

      if (!fire.is_null())
      {
        return fire;
      }

      return getFireKeysFromRules(rule["rules"]);
    }

    return {};
  }

  json getFireKeys()
  {
    for (auto i = 0; i < modifiers.size(); i++)
    {
      auto modifierValue = modifiers[i];
      auto modifier = modifierValue["modifier"];
      auto fire = modifierValue["fire"];
      auto rules = modifierValue["rules"];

      auto currentKey = keyDownStatuses["currentKey"];
      auto interceptAll = modifierValue["interceptAll"];
      bool interceptAllValue = !interceptAll.is_null() && interceptAll == true;
      auto skipKeybinding = false;
      auto isCurrentKeyInModifier = false;

      // verify all modifiers are pressed down
      for (auto m = 0; m < modifier.size(); m++)
      {
        auto mod = std::string(modifier[m]);
        auto modKeyDownStatus = keyDownStatuses[mod];
        isCurrentKeyInModifier = false;

        if (currentKey == mod)
        {
          isCurrentKeyInModifier = true;
        }

        if ((modKeyDownStatus.is_null() || modKeyDownStatus == false) && !isCurrentKeyInModifier)
        {
          skipKeybinding = true;
          break;
        }

        if (m == modifier.size() - 1 && !isCurrentKeyInModifier && !interceptAllValue)
        {
          skipKeybinding = true;
          break;
        }
      }

      if (skipKeybinding)
      {
        continue;
      }

      return {
          {"fireKeys", fire.is_null() ? getFireKeysFromRules(rules) : fire},
          {"interceptAll", interceptAllValue},
          {"isCurrentKeyInModifier", isCurrentKeyInModifier}};
    }

    return {};
  }
};
