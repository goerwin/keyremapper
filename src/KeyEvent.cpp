#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <vector>
#include <tuple>
#include <fstream>
#include "json.hpp"
#include "KeyEvent.h"
#include "utils.h"
#include "symbols.hpp"

namespace KeyEvent
{
  using namespace KeyEvent;
  using json = nlohmann::json;
  using String = std::string;

  short Class::SC_H = Symbols::getSymbolScanCode("H");
  short Class::SC_J = Symbols::getSymbolScanCode("J");
  short Class::SC_K = Symbols::getSymbolScanCode("K");
  short Class::SC_L = Symbols::getSymbolScanCode("L");
  short Class::SC_Up = Symbols::getSymbolScanCode("Up");
  short Class::SC_Down = Symbols::getSymbolScanCode("Down");
  short Class::SC_Right = Symbols::getSymbolScanCode("Right");
  short Class::SC_Left = Symbols::getSymbolScanCode("Left");
  short Class::SC_Home = Symbols::getSymbolScanCode("Home");
  short Class::SC_End = Symbols::getSymbolScanCode("End");
  short Class::SC_Ctrl = Symbols::getSymbolScanCode("Ctrl");
  short Class::SC_Alt = Symbols::getSymbolScanCode("Alt");

  void Class::print(String string, String string2 = "\n")
  {
    auto str = string + string2;
    OutputDebugStringA(str.c_str());
  }

  String Class::getStateSymbol(unsigned short state)
  {
    switch (state)
    {
    case 0:
    case 2:
      return "↓";
    case 1:
    case 3:
      return "↑";
    case 4:
    case 5:
      return "↕";
    }

    return "KEY_STATE_NOT_FOUND";
  }

  Keys Class::concatKeyVectors(Keys keys, Keys keys2, Keys keys3 = {}, Keys keys4 = {})
  {
    keys.insert(keys.end(), keys2.begin(), keys2.end());
    keys.insert(keys.end(), keys3.begin(), keys3.end());
    keys.insert(keys.end(), keys4.begin(), keys4.end());
    return keys;
  }

  Keys Class::keyDownCtrlAsAlt()
  {
    Keys keys;

    if (!isCtrlAsAlt && keyDownStatus["Ctrl"])
    {
      keys.insert(keys.begin(), {KeyUp(SC_Ctrl)});
    }

    isCtrlAsAlt = true;
    keys.insert(keys.end(), {KeyDown(SC_Alt)});
    return keys;
  }
  Keys Class::keyDownCtrlAsCtrl()
  {
    Keys keys;

    if (isCtrlAsAlt && keyDownStatus["Ctrl"])
    {
      keys.insert(keys.begin(), {KeyUp(SC_Alt)});
    }

    isCtrlAsAlt = false;
    keys.insert(keys.end(), {KeyDown(SC_Ctrl)});
    return keys;
  }
  Keys Class::keyUpCtrl()
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

  Keys Class::keyDownAltAsCtrl()
  {
    Keys keys;

    if (!isAltAsCtrl && keyDownStatus["Alt"])
    {
      keys.insert(keys.begin(), {KeyUp(SC_Alt)});
    }

    isAltAsCtrl = true;
    keys.insert(keys.end(), {KeyDown(SC_Ctrl)});
    return keys;
  }
  Keys Class::keyDownAltAsAlt()
  {
    Keys keys;

    if (isAltAsCtrl && keyDownStatus["Alt"])
    {
      keys.insert(keys.begin(), {KeyUp(SC_Ctrl)});
    }

    isAltAsCtrl = false;
    keys.insert(keys.end(), {KeyDown(SC_Alt)});
    return keys;
  }
  Keys Class::keyUpAlt()
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

  short Class::getVimArrowKeyCode(short keyCode)
  {
    return keyCode == SC_K ? SC_Up : keyCode == SC_J ? SC_Down : keyCode == SC_L ? SC_Right : keyCode == SC_H ? SC_Left : 0;
  }

  short Class::getVimHomeEndKeyCode(short keyCode)
  {
    return (keyCode == SC_K || keyCode == SC_H) ? SC_Home : (keyCode == SC_J || keyCode == SC_L) ? SC_End : 0;
  }

  bool Class::isKeyDown(Key key)
  {
    return key.state == 0 || key.state == 2;
  }

  bool Class::isKeyMatches(json ruleKey, std::string key)
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

  bool Class::isConditionMatches(json conditions)
  {
    if (conditions.is_object())
    {
      for (json::iterator i = conditions.begin(); i != conditions.end(); ++i)
      {
        auto key = i.key();
        auto value = i.value();
        auto keyStatus = keyDownStatus[key];

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

  json Class::getFireKeysFromRule(json rule, std::string key = NULL)
  {
    auto ruleKey = rule["key"];
    auto conditions = rule["conditions"];
    auto fire = rule["fire"];
    auto rules = rule["rules"];

    bool keyMatches = isKeyMatches(ruleKey, key);
    bool conditionMatches = isConditionMatches(conditions);

    if (!keyMatches || !conditionMatches)
    {
      return NULL;
    }

    if (rules.is_array())
    {
      for (auto r = rules.begin(); r != rules.end(); ++r)
      {
        auto res = getFireKeysFromRule(r.value(), key);
        if (res != NULL)
        {
          return res;
        }
      }

      return NULL;
    }

    //auto set = rule["set"];
    //if (set.is_object()) {
    //for (json::iterator i = set.begin(); i != set.end(); ++i) {
    //  setGlobalVarValue(i.key(), i.value());
    //  }
    //}

    return fire;
  }

  json Class::getFireKeys()
  {
    for (auto kb = jsonSchema.begin(); kb != jsonSchema.end(); ++kb)
    {
      auto kbValue = kb.value();
      auto kbModifier = kbValue["modifier"];

      if (!kbModifier.is_array())
      {
        return nullptr;
      }

      // verify all modifiers are pressed down
      auto currentKey = keyDownStatus["currentKey"];
      auto interceptAll = kbValue["interceptAll"];
      bool interceptAllValue = !interceptAll.is_null() && interceptAll == true;
      auto skipKeybinding = false;
      auto isCurrentKeyInModifier = false;

      for (auto m = 0; m < kbModifier.size(); m++)
      {
        auto mod = std::string(kbModifier[m]);
        auto modKeyDownStatus = keyDownStatus[mod];
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

        if (m == kbModifier.size() - 1 && !isCurrentKeyInModifier && !interceptAllValue)
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
          {"fireKeys", getFireKeysFromRule(kbValue, currentKey)},
          {"interceptAll", interceptAllValue},
          {"isCurrentKeyInModifier", isCurrentKeyInModifier}};
    }
  }

  Keys Class::concatKeys(Keys keys, Key key)
  {
    keys.insert(keys.end(), key);

    return keys;
  }

  Keys Class::getKeysFromDescription(json keysDescription, short currentKeyCode)
  {
    if (keysDescription.is_null() || !keysDescription.is_array())
    {
      return {};
    }

    Keys allKeys;

    for (auto i = keysDescription.begin(); i != keysDescription.end(); ++i)
    {
      auto keyDesc = i.value();

      if (keyDesc.is_null())
      {
        return allKeys;
      }

      std::string keyDescStr = keyDesc;
      std::string keyValue = keyDescStr;
      std::string keyStatus;

      json keyStatuses = {"_d", "_u", "_du"};
      for (auto i = keyStatuses.begin(); i < keyStatuses.end(); ++i)
      {
        std::string keyStatusLc = i.value();
        size_t upDownIdx = keyDescStr.find(keyStatusLc);

        if (upDownIdx != std::string::npos)
        {
          keyStatus = keyStatusLc;
          keyValue = keyDescStr.substr(0, upDownIdx);
          break;
        }
      }

      if (keyValue == "keyDownAltAsCtrl")
      {
        allKeys = concatKeyVectors(allKeys, keyDownAltAsCtrl());
        continue;
      }
      else if (keyValue == "keyDownAltAsAlt")
      {
        allKeys = concatKeyVectors(allKeys, keyDownAltAsAlt());
        continue;
      }
      else if (keyValue == "keyUpAlt")
      {
        allKeys = concatKeyVectors(allKeys, keyUpAlt());
        continue;
      }
      else if (keyValue == "keyDownCtrlAsCtrl")
      {
        allKeys = concatKeyVectors(allKeys, keyDownCtrlAsCtrl());
        continue;
      }
      else if (keyValue == "keyDownCtrlAsAlt")
      {
        allKeys = concatKeyVectors(allKeys, keyDownCtrlAsAlt());
        continue;
      }
      else if (keyValue == "keyUpCtrl")
      {
        allKeys = concatKeyVectors(allKeys, keyUpCtrl());
        continue;
      }

      short keyCode;
      if (keyValue == "vimArrowKey")
      {
        keyCode = getVimArrowKeyCode(currentKeyCode);
      }
      else if (keyValue == "vimHomeEndKey")
      {
        keyCode = getVimHomeEndKeyCode(currentKeyCode);
      }
      else
      {
        keyCode = Symbols::getSymbolScanCode(keyValue);
      }

      if (keyStatus == "_d")
      {
        allKeys = concatKeys(allKeys, KeyDown(keyCode));
      }
      else if (keyStatus == "_u")
      {
        allKeys = concatKeys(allKeys, KeyUp(keyCode));
      }
      else
      {
        allKeys = concatKeys(allKeys, KeyDown(keyCode));
        allKeys = concatKeys(allKeys, KeyUp(keyCode));
      }
    }

    return allKeys;
  }

  // PRINTING
  void Class::printKeys(Keys keys)
  {
    for (auto i = 0; i < keys.size(); i++)
    {
      auto key = keys[i];
      print(Symbols::getSymbolByScanCode(key.code) + (isKeyDown(key) ? ":down" : ":up"));
    }
  }

  Class::Class(json jsonSchemaEl)
  {
    jsonSchema = jsonSchemaEl;
  }

  Keys Class::getKeyEvents(Keys keys)
  {
    Keys allKeys;
    int size = keys.size();

    for (int i = 0; i < size; i++)
    {
      Key key = keys[i];
      Keys keys;
      auto keyCode = key.code;

      bool isKeyDownEl = isKeyDown(key);
      auto keyValue = Symbols::getSymbolByScanCode(keyCode);
      keyDownStatus["currentKey"] = keyValue;
      keyDownStatus["currentKeyDown"] = isKeyDownEl;
      keyDownStatus[keyValue] = isKeyDownEl;

      auto fireKeysRes = getFireKeys();

      if (!fireKeysRes.is_null())
      {
        auto fireKeys = fireKeysRes["fireKeys"];
        auto interceptAll = fireKeysRes["interceptAll"];
        auto isCurrentKeyInModifier = fireKeysRes["isCurrentKeyInModifier"];

        if (isKeyDownEl)
        {
          allKeys = concatKeyVectors(allKeys, getKeysFromDescription(fireKeys[0], keyCode));
        }
        else
        {
          allKeys = concatKeyVectors(allKeys, getKeysFromDescription(fireKeys[1], keyCode));
        }

        if (!interceptAll && !isCurrentKeyInModifier)
        {
          if (isKeyDownEl)
          {
            allKeys = concatKeys(allKeys, KeyDown(keyCode));
          }
          else
          {
            allKeys = concatKeys(allKeys, KeyUp(keyCode));
          }
        }
      }
      else
      {
        if (isKeyDownEl)
        {
          allKeys = concatKeys(allKeys, KeyDown(keyCode));
        }
        else
        {
          allKeys = concatKeys(allKeys, KeyUp(keyCode));
        }
      }
    }

    printKeys(allKeys);
    //return {};
    return allKeys;
  }
}; // namespace KeyEvent
