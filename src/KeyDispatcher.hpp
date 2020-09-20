#pragma once

#include <vector>
#include <string>
#include "libraries/json.hpp"
#include "helpers.hpp"

class KeyDispatcher
{
  using json = nlohmann::json;
  typedef std::string String;
  typedef std::vector<String> Strings;

private:
  short SC_H;
  short SC_J;
  short SC_K;
  short SC_L;
  short SC_Up;
  short SC_Down;
  short SC_Right;
  short SC_Left;
  short SC_Home;
  short SC_End;

  json globals = {};
  json symbols;
  json rules;
  json keybindings;
  std::map<int, int> remaps = {};
  json tests;

public:
  KeyDispatcher(json rulesEl, json symbolsEl)
  {
    rules = rulesEl;
    symbols = symbolsEl;
    keybindings = rulesEl["keybindings"];
    tests = rulesEl["tests"];

    /*auto remapsEl = rulesEl["remaps"];
    if (!remapsEl.is_null())
    {
      for (auto &[key, value] : remapsEl.items())
      {
        int remap1 = symbols[key];
        int remap2 = symbols[String(value)];
        remaps[remap1] = remap2;
      }
    }

    
    SC_H = getSymbolScanCode("H");
    SC_J = getSymbolScanCode("J");
    SC_K = getSymbolScanCode("K");
    SC_L = getSymbolScanCode("L");
    SC_Up = getSymbolScanCode("Up");
    SC_Down = getSymbolScanCode("Down");
    SC_Right = getSymbolScanCode("Right");
    SC_Left = getSymbolScanCode("Left");
    SC_Home = getSymbolScanCode("Home");
    SC_End = getSymbolScanCode("End");*/
  }

  // [30, 0, 1]
  typedef std::tuple<int, int, int> Key;
  typedef std::vector<Key> Keys;

  // [30, 0]
  typedef std::pair<unsigned short, unsigned short> KeyEvent;
  typedef std::vector<KeyEvent> KeyEvents;

  String stringifyKeyEvents(KeyEvents keyEvents) {
    String result = "";

    for (auto i = 0; i < keyEvents.size(); i++) {
      if (i != 0)
        result += " ";

      auto [code, state] = keyEvents[i];
      auto keyName = getKeyName(code, state);
      result += keyName + (isKeyDown(state) ? ":down" : ":up");  
    }

    return result;
  }
  
  KeyEvents getKeyEvents(KeyEvents keyEvents)
  {
    KeyEvents allKeyEvents = {};

    for (int i = 0; i < keyEvents.size(); i++)
    {
      KeyEvents localKeyEvents = {};
      auto keyEvent = keyEvents[i];
      auto [code, state] = keyEvent;

      // TODO: 
      // key.code = remapKeyCode(key.code);

      bool isKeyDownEl = isKeyDown(state);
      auto keyName = getKeyName(code, state);
      globals["currentKey"] = keyName;
      globals["currentKeyDown"] = isKeyDownEl;
      globals[keyName] = isKeyDownEl;

      auto fireKeysRes = getFireFromKeybindings();
      auto fireKeys = fireKeysRes["fireKeys"];

      if (!fireKeys.is_null())
      {
        if (isKeyDownEl)
        {
          localKeyEvents = concatKeyEvents(localKeyEvents, getKeyEventsFromString(fireKeys[0]));
        }
        else
        {
          localKeyEvents = concatKeyEvents(localKeyEvents, getKeyEventsFromString(fireKeys[1]));
        }
      }
      else
      {
        localKeyEvents = concatKeyEvents(localKeyEvents, { {code, state} });
      }

      Helpers::print(stringifyKeyEvents({ keyEvent }) + " ==> " +
        stringifyKeyEvents(localKeyEvents));

      allKeyEvents = concatKeyEvents(allKeyEvents, localKeyEvents);
    }

    return allKeyEvents;
  }

  KeyEvents getKeyEventsFromString(json str)
  {
    if (str.is_null()) return {};

    Strings strKeys = Helpers::split(str, ' ');
    auto strKeysSize = strKeys.size();
    auto currentKey = globals["currentKey"];
    KeyEvents keyEvents = {};

    for (auto i = 0; i < strKeysSize; i++)
    {
      String keyStateStr;
      Key key;
      String strKey = strKeys[i];
      Strings keyDesc = Helpers::split(strKey, ':');
      String keyName = keyDesc[0];

      if (keyDesc.size() == 2)
        keyStateStr = keyDesc[1];

      if (keyName == "vimArrowKey")
      {
        key = getVimArrowKey(currentKey);
      }
      else if (keyName == "vimHomeEndKey")
      {
        key = getVimHomeEndKey(currentKey);
      }
      else if (keyName == "currentKey")
      {
        key = getKey(currentKey);
      
      }
      else
      {
        key = getKey(keyName);
      }

      auto [code, downState, upState] = key;

      if (keyStateStr == "down")
      {
        keyEvents = concatKeyEvents(keyEvents, { {code, downState} });
      }
      else if (keyStateStr == "up")
      {
        keyEvents = concatKeyEvents(keyEvents, { {code, upState} });
      }
      else
      {
        keyEvents = concatKeyEvents(keyEvents, { {code, downState} });
        keyEvents = concatKeyEvents(keyEvents, { {code, upState} });
      }
    }

    return keyEvents;
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

      Strings test = tests[i];
      auto inputKeys = getKeyEventsFromString(test[0]);
      String expectedKeysStr = test[1];
      auto expectedKeys = getKeyEventsFromString(expectedKeysStr);
      auto resultKeysStr = stringifyKeyEvents(getKeyEvents(inputKeys));

      if (resultKeysStr != stringifyKeyEvents(expectedKeys))
      {
        ok = false;
        message = "TEST FAILED: expected \"" + expectedKeysStr + "\", got \n\"" + resultKeysStr + "\"";
        break;
      }
    }

    return {
        {"ok", ok},
        {"testsSize", testsSize},
        {"message", message}};
  }

private:
  KeyEvents concatKeyEvents(KeyEvents keys, KeyEvents keys2)
  {
    keys.insert(keys.end(), keys2.begin(), keys2.end());
    return keys;
  }

  Key getVimArrowKey(String keyName)
  {
    if (keyName == "H") {
      return getKey("Left");
    }
    else if (keyName == "J") {
      return getKey("Down");
    }
    else if (keyName == "K") {
      return getKey("Up");
    }
    else if (keyName == "L") {
      return getKey("Right");
    }

    return getKey(keyName);
  }

  Key getVimHomeEndKey(String keyName)
  {
    if (keyName == "H" || keyName == "K") {
      return getKey("Home");
    }
    else if (keyName == "J" || keyName == "L") {
      return getKey("End");
    }

    return getKey(keyName);
  }

  static bool isKeyDown(int state)
  {
    return state == 0 || state == 2;
  }

  int remapKeyCode(int key) {
    auto value = remaps[key];

    if (value != 0) return value;
    return key;
  }

  bool isWhen(json when)
  {
    if (when.is_null())
      return true;

    for (auto &[key, value] : when.items())
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

  json getFireFromKeybindings()
  {
    auto currentKey = globals["currentKey"];

    for (auto i = 0; i < keybindings.size(); i++)
    {
      auto keybinding = keybindings[i];
      auto hotkeys = keybinding["hotkeys"];

      if (!isWhen(keybinding["when"]))
      {
        continue;
      }

      for (auto j = 0; j < hotkeys.size(); j++)
      {
        String hotkey = hotkeys[j];
        auto hotkeyKeys = Helpers::split(hotkey, ' ');
        auto hotkeyKeysSize = hotkeyKeys.size();
        auto skipHotkey = false;

        for (auto k = 0; k < hotkeyKeysSize; k++)
        {
          auto hotkeyKey = std::string(hotkeyKeys[k]);
          auto hotkeyKeyDownState = globals[hotkeyKey];

          if (k < hotkeyKeysSize - 1 && (hotkeyKeyDownState.is_null() || hotkeyKeyDownState == false))
          {
            skipHotkey = true;
            break;
          }

          if (k == hotkeyKeysSize - 1 && currentKey != hotkeyKey)
          {
            skipHotkey = true;
            break;
          }
        }

        if (skipHotkey)
        {
          continue;
        }

        setValues(keybinding["set"]);

        return {{"fireKeys", keybinding["fire"]}};
      }
    }

    return {};
  }

  String getKeyName(short scanCode, short keyState)
  {
    for (auto &[key, value] : symbols.items())
    {
      if (value[0] == scanCode &&
         (value[1] == keyState || value[2] == keyState))
        return key;
    }

    return {};
  }

  Key getKey(String symbol)
  {
    return symbols[symbol];
  }
};
