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
  typedef unsigned short ushort;

  struct Key
  {
    String name;
    ushort code;
    ushort downState;
    ushort upState;
  };
  typedef std::vector<Key> Keys;

  struct KeyEvent
  {
    ushort code;
    ushort state;
  };
  typedef std::vector<KeyEvent> KeyEvents;

private:
  json globals = {};
  json symbols;
  json rules;
  json keybindings;
  json remaps;
  json tests;

public:
  KeyDispatcher(json rulesEl, json symbolsEl)
  {
    rules = rulesEl;
    symbols = symbolsEl;
    keybindings = rulesEl["keybindings"];
    tests = rulesEl["tests"];
    remaps = rulesEl["remaps"];
  }

  String stringifyKeyEvents(KeyEvents keyEvents)
  {
    String result = "";

    for (auto i = 0; i < keyEvents.size(); i++)
    {
      if (i != 0)
        result += " ";

      auto [code, state] = keyEvents[i];
      auto keyName = getKeyName(code, state);
      result += keyName + (isKeyDown(state) ? ":down" : ":up");
    }

    return result;
  }

  KeyEvents apply(KeyEvents keyEvents)
  {
    KeyEvents allKeyEvents = {};

    for (int i = 0; i < keyEvents.size(); i++)
    {
      KeyEvents localKeyEvents = {};
      auto keyEvent = keyEvents[i];
      auto [code, state] = keyEvent;
      auto keyName = getKeyName(code, state);
      bool isKeyDownEl = isKeyDown(state);

      auto newKeyEvent = getRemappedKeyEvent(keyName, code, state, isKeyDownEl);
      auto [newCode, newState] = newKeyEvent;
      auto newKeyName = getKeyName(newCode, newState);
      bool newIsKeyDownEl = isKeyDown(newState);

      globals["currentKey"] = newKeyName;
      globals["currentKeyDown"] = isKeyDownEl;
      globals[newKeyName] = isKeyDownEl;

      auto fireKeys = getFireFromKeybindings();

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
        localKeyEvents = concatKeyEvents(localKeyEvents, {{newCode, newState}});
      }

      Helpers::print(
          std::to_string(code) + ":" + std::to_string(state) + ":" +
          stringifyKeyEvents({keyEvent}) + " ==> " +
          stringifyKeyEvents({newKeyEvent}) + " ==> " +
          stringifyKeyEvents(localKeyEvents));

      allKeyEvents = concatKeyEvents(allKeyEvents, localKeyEvents);
    }

    return allKeyEvents;
  }

  KeyEvents getKeyEventsFromString(json str)
  {
    if (str.is_null())
      return {};

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
        key = getVimArrowKey(currentKey);
      else if (keyName == "vimHomeEndKey")
        key = getVimHomeEndKey(currentKey);
      else if (keyName == "currentKey")
        key = getKey(currentKey);
      else
        key = getKey(keyName);

      auto code = key.code;
      auto downState = key.downState;
      auto upState = key.upState;

      if (keyStateStr == "down")
        keyEvents = concatKeyEvents(keyEvents, {{code, downState}});
      else if (keyStateStr == "up")
        keyEvents = concatKeyEvents(keyEvents, {{code, upState}});
      else
      {
        keyEvents = concatKeyEvents(keyEvents, {{code, downState}});
        keyEvents = concatKeyEvents(keyEvents, {{code, upState}});
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

    for (auto i = 0; i < testsSize; i++)
    {
      globals = {};

      Strings test = tests[i];
      auto inputKeys = getKeyEventsFromString(test[0]);
      String expectedKeysStr = test[1];
      auto expectedKeys = getKeyEventsFromString(expectedKeysStr);
      auto resultKeysStr = stringifyKeyEvents(apply(inputKeys));

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
    if (keyName == "H")
      return getKey("Left");
    else if (keyName == "J")
      return getKey("Down");
    else if (keyName == "K")
      return getKey("Up");
    else if (keyName == "L")
      return getKey("Right");

    return getKey(keyName);
  }

  Key getVimHomeEndKey(String keyName)
  {
    if (keyName == "H" || keyName == "K")
      return getKey("Home");
    else if (keyName == "J" || keyName == "L")
      return getKey("End");

    return getKey(keyName);
  }

  static bool isKeyDown(ushort state)
  {
    return state == 0 || state == 2;
  }

  KeyEvent getRemappedKeyEvent(String keyName, ushort code, ushort state, bool isKeyDown)
  {
    auto newKeyName = remaps[keyName];

    if (newKeyName.is_null())
      return {code, state};

    auto newKey = getKey(newKeyName);
    auto [name, newCode, downState, upState] = newKey;

    return {newCode, isKeyDown ? downState : upState};
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

        return keybinding["fire"];
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

  Key getKey(String keyName)
  {
    auto symbolDef = symbols[keyName];
    Key key = {};

    key.name = keyName;
    key.code = symbolDef[0];
    key.downState = symbolDef[1];
    key.upState = symbolDef[2];
    return key;
  }
};
