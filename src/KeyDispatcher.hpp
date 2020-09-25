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
  typedef std::vector<json> JsonArray;
  typedef std::tuple<int, String, String> KeyPress;
  typedef std::vector<KeyPress> KeyPresses;

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
  json appsDefinitions;
  json remaps;
  json tests;
  json keyPresses;
  int REPEAT_TIME;

public:
  KeyDispatcher(json rulesEl, json symbolsEl)
  {
    rules = rulesEl;
    symbols = symbolsEl;
    keybindings = rulesEl["keybindings"];
    REPEAT_TIME = rulesEl["keyPressesDelay"].is_null() ? 200 : rulesEl["keyPressesDelay"].get<int>();
    tests = rulesEl["tests"];
    remaps = rulesEl["remaps"];
    keyPresses = rulesEl["keyPresses"];
    appsDefinitions = rulesEl["apps"];
  }

  double getTimeDifference(double time1, double time2)
  {
    return (time1 - time2) / CLOCKS_PER_SEC * 1000;
  }

  String lastKeyName;
  int multiplePressesCount = 0;
  double keyDownTime = 0;
  double keyUpTime = 0;

  String getMultiplePressesFireItem(String keyName, bool isKeyDown)
  {
    if (keyPresses.is_null())
      return {};

    if (multiplePressesCount == 0)
      lastKeyName = keyName;

    if (lastKeyName != keyName)
    {
      multiplePressesCount = 0;
      keyDownTime = 0;
      keyUpTime = 0;
    }

    if (isKeyDown)
    {
      if (!keyDownTime)
      {
        keyDownTime = clock();
      }

      if (!keyUpTime || getTimeDifference(keyDownTime, keyUpTime) >= REPEAT_TIME)
      {
        multiplePressesCount = 0;
      }

      keyUpTime = 0;
      return {};
    }
    else
    {
      keyUpTime = clock();

      if (keyDownTime != 0 && getTimeDifference(keyUpTime, keyDownTime) < REPEAT_TIME)
      {
        keyDownTime = 0;
        multiplePressesCount = lastKeyName == keyName ? multiplePressesCount + 1 : 1;
      }
      else
      {
        multiplePressesCount = 0;
        keyDownTime = 0;
        keyUpTime = 0;
        return {};
      }
    }

    auto keyPressesParsed = keyPresses.get<KeyPresses>();

    for (auto i = 0; i < keyPressesParsed.size(); i++)
    {
      auto keyPress = keyPressesParsed[i];
      auto numberOfPresses = std::get<0>(keyPress);
      auto keyPressKeyName = std::get<1>(keyPress);
      auto ruleItem = std::get<2>(keyPress);

      if (numberOfPresses == multiplePressesCount && keyPressKeyName == keyName)
      {
        return ruleItem;
      }
    }

    return {};
  }

  KeyEvents applyKeys(KeyEvents keyEvents)
  {
    KeyEvents newKeyEvents = {};

    for (int i = 0; i < keyEvents.size(); i++)
    {
      auto keyEvent = keyEvents[i];
      auto [code, state] = keyEvent;
      auto keyName = getKeyName(code, state);
      bool isKeyDownEl = isKeyDown(state);

      auto newKeyEvent = getRemappedKeyEvent(keyName, code, state, isKeyDownEl);
      auto [newCode, newState] = newKeyEvent;
      auto newKeyName = getKeyName(newCode, newState);
      bool newIsKeyDownEl = isKeyDown(newState);

      globals["currentKey"] = newKeyName;
      globals["currentKeyDown"] = newIsKeyDownEl;
      globals[newKeyName] = newIsKeyDownEl;

      // Ignore FakeShiftL coming from special keys
      // like NumPadRight when NumLock is on
      if (newKeyName == "FakeShiftL")
      {
        continue;
      }

      auto multiplePressesFireItem = getMultiplePressesFireItem(newKeyName, newIsKeyDownEl);
      if (!multiplePressesFireItem.empty())
      {
        keyEvents = Helpers::concatArrays(keyEvents, getKeyEventsFromString(multiplePressesFireItem), i + 1);
      }

      KeyEvents localKeyEvents = {};
      auto fireKeys = getFireFromKeybindings();

      if (!fireKeys.is_null())
      {
        if (isKeyDownEl)
        {
          localKeyEvents = Helpers::concatArrays(localKeyEvents, getKeyEventsFromString(fireKeys[0]));
        }
        else
        {
          localKeyEvents = Helpers::concatArrays(localKeyEvents, getKeyEventsFromString(fireKeys[1]));
        }
      }
      else
      {
        localKeyEvents = Helpers::concatArrays(localKeyEvents, {{newCode, newState}});
      }

      Helpers::print(
          std::to_string(code) + ":" + std::to_string(state) + ":" +
          stringifyKeyEvents({keyEvent}) + " ==> " +
          stringifyKeyEvents({newKeyEvent}) + " ==> " +
          stringifyKeyEvents(localKeyEvents));

      newKeyEvents = Helpers::concatArrays(newKeyEvents, localKeyEvents);
    }

    return newKeyEvents;
  }

  void setAppName(String appName)
  {
    globals["appName"] = appName;
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
        keyEvents = Helpers::concatArrays(keyEvents, {{code, downState}});
      else if (keyStateStr == "up")
        keyEvents = Helpers::concatArrays(keyEvents, {{code, upState}});
      else
      {
        keyEvents = Helpers::concatArrays(keyEvents, {{code, downState}});
        keyEvents = Helpers::concatArrays(keyEvents, {{code, upState}});
      }
    }

    return keyEvents;
  }

  void reset()
  {
    globals = {};
    lastKeyName = "";
    multiplePressesCount = 0;
    keyDownTime = 0;
    keyUpTime = 0;
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
      Strings test = tests[i];
      auto inputKeys = getKeyEventsFromString(test.at(0));
      String expectedKeysStr = test.at(1);

      reset();
      try
      {
        globals["appName"] = test.at(2);
      }
      catch (...)
      {
      }

      auto expectedKeys = getKeyEventsFromString(expectedKeysStr);
      auto resultKeysStr = stringifyKeyEvents(applyKeys(inputKeys));

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
    String appName = globals["appName"].is_null() ? "" : globals["appName"];
    auto appKeybindings = appsDefinitions[appName]["keybindings"];
    auto allKeybindings = keybindings.get<JsonArray>();

    if (!appKeybindings.is_null())
    {
      allKeybindings = Helpers::concatArrays(allKeybindings, appKeybindings.get<JsonArray>(), 0);
    }

    for (auto i = 0; i < allKeybindings.size(); i++)
    {
      auto keybinding = allKeybindings[i];
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
