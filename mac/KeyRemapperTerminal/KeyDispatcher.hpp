#pragma once

#include "Helpers.hpp"
#include "vendor_json.hpp"
#include <string>
#include <vector>
#include <chrono>

class KeyDispatcher {
  using json = nlohmann::json;
  typedef std::string String;
  typedef std::vector<String> Strings;
  typedef unsigned short ushort;
  typedef std::vector<json> JsonArray;

  struct Key {
    String name;
    ushort code;
    ushort downState;
    ushort upState;
  };
  typedef std::vector<Key> Keys;

  struct KeyEvent {
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

  double getTimeDifference(double time1, double time2) {
      return time1 - time2;
  }

  String lastKeyName;
  int multiplePressesCount = 0;
  double keyDownTime = 0; // in ms
  double keyUpTime = 0; // in ms

  json getMultiplePressesFireItem(String keyName, bool isKeyDown) {
    if (keyPresses.is_null())
      return {};

    if (multiplePressesCount == 0)
      lastKeyName = keyName;

    if (lastKeyName != keyName) {
      multiplePressesCount = 0;
      keyDownTime = 0;
      keyUpTime = 0;
    }

    if (isKeyDown) {
      if (!keyDownTime)
          keyDownTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
      if (!keyUpTime ||
          getTimeDifference(keyDownTime, keyUpTime) >= REPEAT_TIME)
        multiplePressesCount = 0;

      keyUpTime = 0;
      return {};
    } else {
      keyUpTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

      if (keyDownTime != 0 &&
          getTimeDifference(keyUpTime, keyDownTime) < REPEAT_TIME) {
        keyDownTime = 0;
        multiplePressesCount =
            lastKeyName == keyName ? multiplePressesCount + 1 : 1;
      } else {
        multiplePressesCount = 0;
        keyDownTime = 0;
        keyUpTime = 0;
        return {};
      }
    }

    for (size_t i = 0; i < keyPresses.size(); i++) {
      auto keyPress = keyPresses[i];
      int repeat = keyPress["repeat"];
      String inputKey = keyPress["inputKey"];
      String ruleItem = keyPress["fire"];
      auto skipKeyBindings = keyPress["skipKeyBindings"];

      if (repeat == multiplePressesCount && inputKey == keyName) {
        return {ruleItem, skipKeyBindings.is_null()
                              ? false
                              : skipKeyBindings.get<bool>()};
      }
    }

    return {};
  }

public:
  KeyDispatcher(json rulesEl, json symbolsEl) {
    rules = rulesEl;
    symbols = symbolsEl;
    keybindings = rulesEl["keybindings"];
    REPEAT_TIME = rulesEl["keyPressesDelay"].is_null()
                      ? 200
                      : rulesEl["keyPressesDelay"].get<int>();
    tests = rulesEl["tests"];
    remaps = rulesEl["remaps"];
    keyPresses = rulesEl["keyPresses"];
    appsDefinitions = rulesEl["apps"];
  }

  KeyEvents applyKeys(KeyEvents keyEvents) {
    KeyEvents newKeyEvents = {};

    for (size_t i = 0; i < keyEvents.size(); i++) {
      auto keyEvent = keyEvents[i];
      auto [code, state] = keyEvent;
      auto keyName = getKeyName(code, state);
      bool isKeyDownEl = isKeyDown(state);

      auto newKeyEvent = getRemappedKeyEvent(keyName, code, state, isKeyDownEl);
      auto [newCode, newState] = newKeyEvent;
      auto newKeyName = getKeyName(newCode, newState);
      bool newIsKeyDownEl = isKeyDown(newState);

      globals["currentKey"] = newKeyName;
      // TODO: I think I can delete this currentKeyDown
      globals["currentKeyDown"] = newIsKeyDownEl;
      globals[newKeyName] = newIsKeyDownEl;

      // Ignore FakeShiftL coming from special keys
      // like NumPadRight when NumLock is on
      if (newKeyName == "FakeShiftL")
        continue;

      KeyEvents localKeyEvents = {};
      auto fireKeys = getFireFromKeybindings();

      if (!fireKeys.is_null())
        localKeyEvents = Helpers::concatArrays(
            localKeyEvents,
            getKeyEventsFromString(isKeyDownEl ? fireKeys[0] : fireKeys[1]));
      else
        localKeyEvents =
            Helpers::concatArrays(localKeyEvents, {{newCode, newState}});

      auto multiplePressesFireItem =
          getMultiplePressesFireItem(newKeyName, newIsKeyDownEl);
      if (!multiplePressesFireItem.is_null()) {
        auto fire = multiplePressesFireItem[0].get<String>();
        auto skipKeyBindings = multiplePressesFireItem[1].get<bool>();
        auto keyPressKeyEvents = getKeyEventsFromString(fire);

        if (skipKeyBindings)
          localKeyEvents =
              Helpers::concatArrays(localKeyEvents, keyPressKeyEvents);
        else
          keyEvents =
              Helpers::concatArrays(keyEvents, keyPressKeyEvents, i + 1);
      }

      Helpers::print(std::to_string(code) + ":" + std::to_string(state) + ":" +
                     stringifyKeyEvents({keyEvent}) + " ==> " +
                     stringifyKeyEvents({newKeyEvent}) + " ==> " +
                     stringifyKeyEvents(localKeyEvents));

      newKeyEvents = Helpers::concatArrays(newKeyEvents, localKeyEvents);
    }

    return newKeyEvents;
  }

  void setAppName(String appName) { globals["appName"] = appName; }

  String stringifyKeyEvents(KeyEvents keyEvents) {
    String result = "";

    for (size_t i = 0; i < keyEvents.size(); i++) {
      if (i != 0)
        result += " ";

      auto [code, state] = keyEvents[i];
      auto keyName = getKeyName(code, state);
      result += keyName + (isKeyDown(state) ? ":down" : ":up");
    }

    return result;
  }

  KeyEvents getKeyEventsFromString(json str) {
    if (str.is_null())
      return {};

    Strings strKeys = Helpers::split(str, ' ');
    auto strKeysSize = strKeys.size();
    auto currentKey = globals["currentKey"];
    KeyEvents keyEvents = {};

    for (size_t i = 0; i < strKeysSize; i++) {
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
      else {
        keyEvents = Helpers::concatArrays(keyEvents, {{code, downState}});
        keyEvents = Helpers::concatArrays(keyEvents, {{code, upState}});
      }
    }

    return keyEvents;
  }

  void reset() {
    globals = {};
    lastKeyName = "";
    multiplePressesCount = 0;
    keyDownTime = 0;
    keyUpTime = 0;
  }

  json runTests() {
    if (tests.is_null())
      return {};

    bool ok = true;
    auto testsSize = tests.size();
    String message =
        "ALL TESTS PASSED! Number of tests: " + std::to_string(testsSize);

    for (size_t i = 0; i < testsSize; i++) {
      Strings test = tests[i];
      auto inputKeys = getKeyEventsFromString(test[0]);
      String expectedKeysStr = test[1];
      auto testSize = test.size();
      reset();

      if (testSize == 3)
        globals["appName"] = test[2];

      auto expectedKeys = getKeyEventsFromString(expectedKeysStr);
      auto resultKeysStr = stringifyKeyEvents(applyKeys(inputKeys));

      if (resultKeysStr != stringifyKeyEvents(expectedKeys)) {
        ok = false;
        message = "TEST " + std::to_string(i) + " FAILED: expected \"" +
                  expectedKeysStr + "\", got \n\"" + resultKeysStr + "\"";
        break;
      }
    }

    return {{"ok", ok}, {"testsSize", testsSize}, {"message", message}};
  }

private:
  Key getVimArrowKey(String keyName) {
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

  Key getVimHomeEndKey(String keyName) {
    if (keyName == "H" || keyName == "K")
      return getKey("Home");
    else if (keyName == "J" || keyName == "L")
      return getKey("End");

    return getKey(keyName);
  }

  static bool isKeyDown(ushort state) { return state == 0 || state == 2; }

  KeyEvent getRemappedKeyEvent(String keyName, ushort code, ushort state,
                               bool isKeyDown) {
    auto newKeyName = remaps[keyName];

    if (newKeyName.is_null())
      return {code, state};

    auto newKey = getKey(newKeyName);
    auto [name, newCode, downState, upState] = newKey;

    return {newCode, isKeyDown ? downState : upState};
  }

  bool isWhen(json when) {
    if (when.is_null())
      return true;

    for (auto &[key, value] : when.items()) {
      auto globalValue = globals[key];

      if (globalValue.is_null()) {
        if (value != false)
          return false;
        continue;
      }

      if (value != globalValue) {
        return false;
      }
    }

    return true;
  }

  void setValues(json values) {
    if (values.is_null())
      return;

    for (auto &[key, value] : values.items())
      globals[key] = value;
  }

  json getFireFromKeybindings() {
    auto currentKey = globals["currentKey"];
    auto allKeybindings = keybindings.get<JsonArray>();

    for (size_t i = 0; i < allKeybindings.size(); i++) {
      auto keybinding = allKeybindings[i];
      auto keys = keybinding["keys"];

      if (!isWhen(keybinding["when"]))
        continue;

      for (size_t j = 0; j < keys.size(); j++) {
        if (currentKey != keys[j])
          continue;

        setValues(keybinding["set"]);
        return keybinding["fire"];
      }
    }

    return {};
  }

  String getKeyName(short scanCode, short keyState) {
    for (auto &[key, value] : symbols.items()) {
      if (value[0] == scanCode &&
          (value[1] == keyState || value[2] == keyState))
        return key;
    }

    return {};
  }

  Key getKey(String keyName) {
    auto symbolDef = symbols[keyName];
    Key key = {};

    key.name = keyName;
    key.code = symbolDef[0];
    key.downState = symbolDef[1];
    key.upState = symbolDef[2];
    return key;
  }
};
