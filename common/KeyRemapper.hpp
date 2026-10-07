#pragma once

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "./Helpers.hpp"
#include "./vendors/json.hpp"

class KeyRemapper {
  using json = nlohmann::json;
  typedef std::string String;
  typedef std::vector<String> Strings;
  typedef unsigned short ushort;
  typedef std::vector<json> JsonArray;

 public:
  struct KeyEvent {
    String name;
    ushort code;
    ushort state;
    bool isKeyDown;
  };
  typedef std::vector<KeyEvent> KeyEvents;

 private:
  json globals = {};
  json symbols;
  json profile;
  json keybindings;
  json keyPresses;
  json remaps;
  short keyPressesDelay;
  String SPECIAL_KEY = "SK";
  ushort SPECIAL_KEY_CODE = 6969;
  KeyEvents afterKeyUpKeyEvents = {};
  // mappedKeys item of the matched keybinding, for the mappedKey placeholder
  String mappedKey;

  // appName, keyboardId, keyboardDescription, keyEvents
  std::function<void(String, String, String, String)> applyKeysCb;

  double getTimeDifference(double time1, double time2) { return time1 - time2; }

  // keyPresses rule with ifHeldFor for the key being held, until applyHold
  json pendingHold;
  String firedHoldKeyName;

  String lastKeyName;
  short keyPressesCount = 0;
  double keyDownTime = 0;  // in ms
  double keyUpTime = 0;    // in ms

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

      keyDownTime = now();

      if (!keyUpTime ||
          getTimeDifference(keyDownTime, keyUpTime) >= keyPressesDelay)
        keyPressesCount = 0;

      keyUpTime = 0;
      return;
    }

    keyUpTime = now();

    if (keyDownTime != 0 &&
        getTimeDifference(keyUpTime, keyDownTime) < keyPressesDelay) {
      keyDownTime = 0;
      keyPressesCount = lastKeyName == keyName ? keyPressesCount + 1 : 1;
      return;
    }

    keyPressesCount = 0;
    keyDownTime = 0;
    keyUpTime = 0;
  }

 public:
  // In ms, never 0. Replaceable so the tests don't depend on the real time
  std::function<double()> now = [] {
    return std::chrono::system_clock::now().time_since_epoch() /
           std::chrono::milliseconds(1);
  };

  KeyRemapper(json profileEl, json symbolsEl) {
    profile = profileEl;
    symbols = symbolsEl;
    keybindings = profileEl["keybindings"].get<JsonArray>();
    for (auto &keybinding : keybindings) {
      auto mappedKeys = keybinding["mappedKeys"];
      if (mappedKeys.is_null()) continue;
      bool isValid = mappedKeys.is_array() &&
                     mappedKeys.size() == keybinding["keys"].size();
      for (auto &key : mappedKeys) isValid = isValid && key.is_string();
      if (!isValid)
        throw std::runtime_error(
            "\"mappedKeys\" needs one key name per item of \"keys\": " +
            keybinding["keys"].dump());
    }
    keyPressesDelay = profileEl["keyPressesDelay"].is_null()
                          ? 200
                          : profileEl["keyPressesDelay"].get<short>();
    remaps = profileEl["remaps"];
    keyPresses = profileEl["keyPresses"];
    reset();
  }

  KeyEvents applyKeys(KeyEvents keyEvents) {
    KeyEvents newKeyEvents = {};

    for (size_t i = 0; i < keyEvents.size(); i++) {
      KeyEvents localKeyEvents = afterKeyUpKeyEvents;
      afterKeyUpKeyEvents = {};
      mappedKey = "";

      auto keyEvent = keyEvents[i];
      auto code = keyEvent.code;

      if (code == SPECIAL_KEY_CODE) {
        newKeyEvents = Helpers::concatArrays(newKeyEvents, {{keyEvent}});
        continue;
      }

      auto state = keyEvent.state;
      auto parsedKeyEvent = getKeyEvent(code, state);
      auto remappedKeyEvent = getRemappedKeyEvent(parsedKeyEvent);
      auto remappedCode = remappedKeyEvent.code;
      auto remappedState = remappedKeyEvent.state;
      auto keyName = remappedKeyEvent.name;
      auto isKeyDown = remappedKeyEvent.isKeyDown;

      globals["currentKey"] = remappedKeyEvent.name;
      globals["isKeyDown"] = remappedKeyEvent.isKeyDown;
      globals[keyName] = remappedKeyEvent.isKeyDown;

      auto keybindingInfo = getKeybindingInfo(keyName, isKeyDown);
      if (!keybindingInfo.is_null()) {
        mappedKey = keybindingInfo["mappedKey"];
        if (isKeyDown)
          setValues(keybindingInfo["set"]);
        else {
          setValues(keybindingInfo["setOnKeyUp"]);
          afterKeyUpKeyEvents =
              getKeyEventsFromString(keybindingInfo["afterKeyUp"]);
        }

        json send = keybindingInfo["send"];
        localKeyEvents = Helpers::concatArrays(
            localKeyEvents,
            getKeyEventsFromString(isKeyDown ? send[0] : send[1]));
      } else
        localKeyEvents =
            Helpers::concatArrays(localKeyEvents, {remappedKeyEvent});

      // Windows keeps sending key downs while a key is held
      bool isRepeat = isKeyDown && lastKeyName == keyName && keyDownTime != 0;
      if (!isRepeat) {
        pendingHold = isKeyDown ? getKeyHoldInfo(keyName) : json();
      }

      setKeyPressesCount(keyName, isKeyDown);

      // A held key isn't also a tap
      if (!isKeyDown && keyName == firedHoldKeyName) {
        keyPressesCount = 0;
        firedHoldKeyName = "";
      }

      auto keyPressesInfo = getKeyPressesInfo(keyName, isKeyDown);

      // No later tap could fire, so the next one starts a new count
      if (!isKeyDown && !hasKeyPressesAbove(keyName, keyPressesCount))
        keyPressesCount = 0;

      if (!keyPressesInfo.is_null()) {
        setValues(keyPressesInfo["set"]);
        localKeyEvents = Helpers::concatArrays(
            localKeyEvents, getKeyEventsFromString(keyPressesInfo["send"]));
        afterKeyUpKeyEvents = Helpers::concatArrays(
            afterKeyUpKeyEvents,
            getKeyEventsFromString(keyPressesInfo["afterKeyUp"]));
      }

      if (applyKeysCb)
        applyKeysCb(globals["appName"], globals["keyboard"],
                    globals["keyboardDescription"],
                    // inputCode:inputState -> remappedCode:remappedState ->
                    // parsedKeyEvent -> remappedKeyEvent -> keyEventsSent
                    std::to_string(code) + ":" + std::to_string(state) +
                        " -> " + std::to_string(remappedCode) + ":" +
                        std::to_string(remappedState) + " -> " +
                        stringifyKeyEvents({parsedKeyEvent}) + " -> " +
                        stringifyKeyEvents({remappedKeyEvent}) + " -> " +
                        stringifyKeyEvents(localKeyEvents));

      newKeyEvents = Helpers::concatArrays(newKeyEvents, localKeyEvents);
    }

    return newKeyEvents;
  }

  // ms the key that was just pressed has to be held for its ifHeldFor rule, or
  // -1. After that time, the caller calls applyHold
  int getHoldDelay() {
    return pendingHold.is_null() ? -1 : pendingHold["ifHeldFor"].get<int>();
  }

  // Sends the ifHeldFor rule of the held key, unless another key event came
  // after it was pressed
  KeyEvents applyHold() {
    if (pendingHold.is_null()) return {};

    auto hold = pendingHold;
    pendingHold = {};
    if (!ifConditions(hold["if"])) return {};

    firedHoldKeyName = hold["key"];
    setValues(hold["set"]);
    afterKeyUpKeyEvents = Helpers::concatArrays(
        afterKeyUpKeyEvents, getKeyEventsFromString(hold["afterKeyUp"]));
    auto keyEvents = getKeyEventsFromString(hold["send"]);

    if (applyKeysCb)
      applyKeysCb(globals["appName"], globals["keyboard"],
                  globals["keyboardDescription"],
                  firedHoldKeyName + ":hold -> " + stringifyKeyEvents(keyEvents));

    return keyEvents;
  }

  void setAppName(String appName) { globals["appName"] = appName; }

  void setKeyboard(String keyboard, String description) {
    globals["keyboard"] = keyboard;
    globals["keyboardDescription"] = description;
  }

  void setApplyKeysCb(
      std::function<void(String, String, String, String)> _applyKeysCb) {
    applyKeysCb = _applyKeysCb;
  }

  String stringifyKeyEvents(KeyEvents keyEvents) {
    String result = "";

    for (size_t i = 0; i < keyEvents.size(); i++) {
      if (i != 0) result += " ";

      auto keyEvent = keyEvents[i];
      auto keyName = keyEvent.name;
      auto isKeyDown = keyEvent.isKeyDown;

      if (keyEvent.code == SPECIAL_KEY_CODE)
        result += keyName + ":" + std::to_string(keyEvent.state);
      else
        result += keyName + (isKeyDown ? ":down" : ":up");
    }

    return result;
  }

  KeyEvents getKeyEventsFromString(json str) {
    if (str.is_null()) return {};

    Strings strKeys = Helpers::split(str, ' ');
    auto strKeysSize = strKeys.size();
    String currentKey = globals["currentKey"];
    KeyEvents keyEvents = {};

    for (size_t i = 0; i < strKeysSize; i++) {
      String strKey = strKeys[i];
      Strings keyDesc = Helpers::split(strKey, ':');
      String keyName = keyDesc[0];
      if (keyName == "currentKey") keyName = currentKey;
      if (keyName == "mappedKey") keyName = mappedKey;

      if (keyName == SPECIAL_KEY) {
        ushort val = atoi(keyDesc[2].c_str());
        keyEvents = Helpers::concatArrays(
            keyEvents,
            {{keyName + ":" + keyDesc[1], SPECIAL_KEY_CODE, val, true}});
        continue;
      }

      KeyEvent keyEventDown = getKeyEvent(keyName, true);
      KeyEvent keyEventUp = getKeyEvent(keyName, false);
      String keyStateStr;

      if (keyDesc.size() == 2) keyStateStr = keyDesc[1];

      if (keyStateStr == "down")
        keyEvents = Helpers::concatArrays(keyEvents, {keyEventDown});
      else if (keyStateStr == "up")
        keyEvents = Helpers::concatArrays(keyEvents, {keyEventUp});
      else
        keyEvents =
            Helpers::concatArrays(keyEvents, {keyEventDown, keyEventUp});
    }

    return keyEvents;
  }

  void reset() {
    globals = {};
    globals["appName"] = "";
    globals["keyboard"] = "";
    globals["keyboardDescription"] = "";
    globals["currentKey"] = "";
    globals["isKeyDown"] = "";

    afterKeyUpKeyEvents = {};
    pendingHold = {};
    firedHoldKeyName = "";
    lastKeyName = "";
    keyPressesCount = 0;
    keyDownTime = 0;
    keyUpTime = 0;
  }

 private:
  bool ifConditions(json ifConds) {
    if (ifConds.is_null()) return true;

    for (auto &[key, value] : ifConds.items()) {
      auto globalValue = globals[key];

      if (globalValue.is_null() && value == false) continue;
      if (value.is_array()) {
        if (std::find(value.begin(), value.end(), globalValue) == value.end())
          return false;
        continue;
      }
      if (value != globalValue) return false;
    }

    return true;
  }

  void setValues(json values) {
    if (values.is_null()) return;
    for (auto &[key, value] : values.items()) globals[key] = value;
  }

  json getKeybindingInfo(String key, bool isKeyDown) {
    size_t keybindingsSize = keybindings.size();

    for (size_t i = 0; i < keybindingsSize; i++) {
      auto keybinding = keybindings[i];
      auto keys = keybinding["keys"];
      auto mappedKeys = keybinding["mappedKeys"];

      if (!ifConditions(keybinding["if"])) continue;

      for (size_t j = 0; j < keys.size(); j++) {
        if (key != keys[j]) continue;

        return {{"send", keybinding["send"]},
                {"mappedKey", mappedKeys.is_array() ? mappedKeys[j] : json("")},
                {"afterKeyUp", keybinding["afterKeyUp"]},
                {"set", keybinding["set"]},
                {"setOnKeyUp", keybinding["setOnKeyUp"]}};
      }
    }

    return {};
  }

  json getKeyPressesInfo(String key, bool isKeyDown) {
    size_t keyPressesSize = keyPresses.size();

    if (isKeyDown) return {};

    for (size_t i = 0; i < keyPressesSize; i++) {
      auto keypress = keyPresses[i];

      if (!ifConditions(keypress["if"])) continue;
      if (key != keypress["key"]) continue;
      if (keyPressesCount != getPressedNTimes(keypress)) continue;

      return {{"send", keypress["send"]},
              {"set", keypress["set"]},
              {"afterKeyUp", keypress["afterKeyUp"]}};
    }

    return {};
  }

  bool hasKeyPressesAbove(String key, short count) {
    for (auto &keypress : keyPresses) {
      if (key != keypress["key"]) continue;
      if (getPressedNTimes(keypress) <= count) continue;
      if (!ifConditions(keypress["if"])) continue;
      return true;
    }

    return false;
  }

  // A single tap by default. -1 for the ifHeldFor ones, they aren't taps
  int getPressedNTimes(json &keypress) {
    if (keypress["ifPressedNTimes"].is_number())
      return keypress["ifPressedNTimes"].get<int>();
    return keypress["ifHeldFor"].is_number() ? -1 : 1;
  }

  json getKeyHoldInfo(String key) {
    for (auto &keypress : keyPresses) {
      if (!keypress["ifHeldFor"].is_number()) continue;
      if (key != keypress["key"]) continue;
      if (!ifConditions(keypress["if"])) continue;
      return keypress;
    }

    return {};
  }

  KeyEvent getRemappedKeyEvent(KeyEvent keyEvent) {
    size_t remapsSize = remaps.size();
    auto keyName = keyEvent.name;

    for (size_t i = 0; i < remapsSize; i++) {
      auto remap = remaps[i];
      if (keyName != remap["from"]) continue;
      if (!ifConditions(remap["if"])) continue;

      String newKeyName = remap["to"];
      return getKeyEvent(newKeyName, keyEvent.isKeyDown);
    }

    return keyEvent;
  }

  KeyEvent getKeyEvent(ushort code, ushort state) {
    for (auto &[key, value] : symbols.items()) {
      if (value[0] != code) continue;
      if (state != value[1] && state != value[2]) continue;
      return {key, code, state, state == value[1]};
    }

    return {"Unknown", code, state, false};
  }

  KeyEvent getKeyEvent(String keyName, bool isKeyDown) {
    auto symbol = symbols[keyName];
    if (symbol.is_null()) return {"Unknown", 0, 0, false};
    return {keyName, symbol[0], isKeyDown ? symbol[1] : symbol[2], isKeyDown};
  }
};
