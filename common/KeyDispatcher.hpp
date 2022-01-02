#pragma once

#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include "./Helpers.hpp"
#include "./vendors/json.hpp"

class KeyDispatcher {
  using json = nlohmann::json;
  typedef std::string String;
  typedef std::vector<String> Strings;
  typedef unsigned short ushort;
  typedef std::vector<json> JsonArray;

  struct KeyEvent {
    ushort code;
    bool isKeyDown;
  };
  typedef std::vector<KeyEvent> KeyEvents;

private:
  json globals = {};
  json symbols;
  json rules;
  json keybindings;
  json keyPresses;
  json remaps;
  short keyPressesDelay;

  // appName, keyboardId, keyboardDescription, keyEvents
  std::function<void(String, String, String, String)> applyKeysCb;

  double getTimeDifference(double time1, double time2) {
      return time1 - time2;
  }

  String lastKeyName;
  short keyPressesCount = 0;
  double keyDownTime = 0; // in ms
  double keyUpTime = 0; // in ms

  void setKeyPressesCount(String keyName, bool isKeyDown) {
    if (lastKeyName != keyName) {
      keyPressesCount = 0;
      keyDownTime = 0;
      keyUpTime = 0;
      lastKeyName = keyName;
    }

    if (isKeyDown) {
      // this is for windows, since interception keeps sending the keydown
      // events when key is held down
      if (lastKeyName == keyName && keyDownTime != 0) {
        keyUpTime = 0;
        return;
      }

      keyDownTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
      //WindowsHelpers::print(std::to_string(keyDownTime));

      if (!keyUpTime || getTimeDifference(keyDownTime, keyUpTime) >= keyPressesDelay)
        keyPressesCount = 0;

      keyUpTime = 0;
      return;
    }

    keyUpTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

    if (keyDownTime != 0 && getTimeDifference(keyUpTime, keyDownTime) < keyPressesDelay) {
      keyDownTime = 0;
      keyPressesCount = lastKeyName == keyName ? keyPressesCount + 1 : 1;
      return;
    }

    keyPressesCount = 0;
    keyDownTime = 0;
    keyUpTime = 0;
  }

public:
  KeyDispatcher(json rulesEl, json symbolsEl) {
    rules = rulesEl;
    symbols = symbolsEl;
    keybindings = rulesEl["keybindings"].get<JsonArray>();
    keyPressesDelay = rulesEl["keyPressesDelay"].is_null()
                      ? 200
                      : rulesEl["keyPressesDelay"].get<short>();
    remaps = rulesEl["remaps"];
    keyPresses = rulesEl["keyPresses"];
    globals["appName"] = "";
    globals["keyboard"] = "";
    globals["keyboardDescription"] = "";
    globals["currentKey"] = "";
    globals["isKeyDown"] = "";
  }

  KeyEvents applyKeys(KeyEvents keyEvents) {
    KeyEvents newKeyEvents = {};

    for (int i = 0; i < keyEvents.size(); i++) {
      auto keyEvent = keyEvents[i];
      auto [code, isKeyDown] = keyEvent;
      auto keyName = getKeyName(code);
      KeyEvents localKeyEvents = {};

      auto [newKeyName, newCode] = getRemappedKey(keyName, code);

      globals["currentKey"] = newKeyName;
      globals["isKeyDown"] = isKeyDown;
      globals[newKeyName] = isKeyDown;

      auto fireKeys = getFireFromKeybindings(newKeyName, isKeyDown);
      if (!fireKeys.is_null())
        localKeyEvents = Helpers::concatArrays(localKeyEvents,
            getKeyEventsFromString(isKeyDown ? fireKeys[0] : fireKeys[1]));
      else
        localKeyEvents = Helpers::concatArrays(localKeyEvents, {{newCode, isKeyDown}});

      setKeyPressesCount(newKeyName, isKeyDown);
      if (!isKeyDown)
        localKeyEvents = Helpers::concatArrays(
          localKeyEvents,
          getKeyEventsFromString(getSendFromKeyPresses(newKeyName))
        );

      if (applyKeysCb) {
        applyKeysCb(
          globals["appName"],
          globals["keyboard"],
          globals["keyboardDescription"],
          std::to_string(code) + ":" +
            (isKeyDown ? "down" : "up") + " -> " +
            stringifyKeyEvents({keyEvent}) + " -> " +
            stringifyKeyEvents({{newCode, isKeyDown}}) + " -> " +
            stringifyKeyEvents(localKeyEvents)
        );
      }

      newKeyEvents = Helpers::concatArrays(newKeyEvents, localKeyEvents);
    }

    return newKeyEvents;
  }

  void setAppName(String appName) {
    globals["appName"] = appName;
  }

  void setKeyboard(String keyboard, String description) {
    globals["keyboard"] = keyboard;
    globals["keyboardDescription"] = description;
  }

  void setApplyKeysCb(std::function<void(String, String, String, String)> _applyKeysCb) {
    applyKeysCb = _applyKeysCb;
  }

  String stringifyKeyEvents(KeyEvents keyEvents) {
    String result = "";

    for (size_t i = 0; i < keyEvents.size(); i++) {
      if (i != 0) result += " ";

      auto [code, isKeyDown] = keyEvents[i];
      auto keyName = getKeyName(code);
      result += keyName + (isKeyDown ? ":down" : ":up");
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
      ushort code;
      String strKey = strKeys[i];
      Strings keyDesc = Helpers::split(strKey, ':');
      String keyName = keyDesc[0];

      if (keyDesc.size() == 2) keyStateStr = keyDesc[1];

      if (keyName == "currentKey") code = getKey(currentKey);
      else code = getKey(keyName);

      if (keyStateStr == "down")
        keyEvents = Helpers::concatArrays(keyEvents, {{code, true}});
      else if (keyStateStr == "up")
        keyEvents = Helpers::concatArrays(keyEvents, {{code, false}});
      else
        keyEvents = Helpers::concatArrays(keyEvents, {{code, true}, {code, false}});
    }

    return keyEvents;
  }

  void reset() {
    globals = {};
    lastKeyName = "";
    keyPressesCount = 0;
    keyDownTime = 0;
    keyUpTime = 0;
  }

private:
  std::pair<String, ushort> getRemappedKey(String keyName, ushort code) {
    size_t remapsSize = remaps.size();

    for (size_t i = 0; i < remapsSize; i++) {
      auto remap = remaps[i];
      if (keyName != remap["from"]) continue;
      if (!isWhen(remap["when"])) continue;

      String newKeyName = remap["to"];
      return {newKeyName, getKey(newKeyName)};
    }

    return {keyName, code};
  }

  bool isWhen(json when) {
    if (when.is_null()) return true;

    for (auto &[key, value] : when.items()) {
      auto globalValue = globals[key];

      if (globalValue.is_null() && value == false) continue;
      if (value != globalValue) return false;
    }

    return true;
  }

  void setValues(json values) {
    if (values.is_null()) return;
    for (auto &[key, value] : values.items()) globals[key] = value;
  }

  json getFireFromKeybindings(String key, bool isKeyDown) {
    size_t keybindingsSize = keybindings.size();

    for (size_t i = 0; i < keybindingsSize; i++) {
      auto keybinding = keybindings[i];
      auto keys = keybinding["keys"];

      if (!isWhen(keybinding["when"])) continue;

      for (size_t j = 0; j < keys.size(); j++) {
        if (key != keys[j]) continue;

        if (isKeyDown) setValues(keybinding["set"]);
        else setValues(keybinding["setOnKeyUp"]);

        return keybinding["fire"];
      }
    }

    return {};
  }

  json getSendFromKeyPresses(String key) {
    size_t keyPressesSize = keyPresses.size();

    for (size_t i = 0; i < keyPressesSize; i++) {
      auto keypress = keyPresses[i];

      if (!isWhen(keypress["when"])) continue;
      if (key != keypress["key"]) continue;
      if (keyPressesCount != keypress["ifPressedNTimes"]) continue;

      return keypress["send"];
    }

    return {};
  }

  String getKeyName(short scanCode) {
    for (auto &[key, value] : symbols.items()) {
      if (value[0] == scanCode) return key;
    }

    return {};
  }

  ushort getKey(String keyName) {
    return symbols[keyName][0];
  }
};
