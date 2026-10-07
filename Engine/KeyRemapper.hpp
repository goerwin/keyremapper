#pragma once

#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "./Helpers.hpp"
#include "./vendors/json.hpp"

class KeyRemapper {
  using json = nlohmann::json;
  typedef std::string String;
  typedef std::vector<String> Strings;
  typedef unsigned short ushort;

 public:
  struct KeyEvent {
    String name;  // "delay" for a delay
    ushort code;  // scancode
    bool isKeyDown;
    bool repeats = true;  // while it's down, like a held key
    int delay = 0;        // ms, for a delay
  };
  typedef std::vector<KeyEvent> KeyEvents;

  // In ms, never 0. Replaceable so the tests don't depend on the real time
  std::function<double()> now = [] {
    return std::chrono::system_clock::now().time_since_epoch() /
           std::chrono::milliseconds(1);
  };

  // symbols is { "keyName": [scanCode, vkCode?] }
  KeyRemapper(json profile, json symbols) {
    for (auto &[name, symbol] : symbols.items()) {
      codes[name] = symbol[0];
      names[symbol[0]] = name;
    }

    validateFields(profile, "The profile",
                   {"name", "tapDelay", "holdDelay", "delayUntilRepeat",
                    "keyRepeatInterval", "doubleClickSpeed", "remaps", "rules",
                    "tests"});
    tapDelay = getNumber(profile, "tapDelay", 200);
    holdDelay = getNumber(profile, "holdDelay", 400);

    for (auto &remap : getArray(profile, "remaps", "The profile"))
      remaps.push_back(parseRemap(remap));
    for (auto &rule : getArray(profile, "rules", "The profile"))
      rules.push_back(parseRule(rule));

    reset();
  }

  KeyEvents applyKeys(KeyEvents keyEvents) {
    KeyEvents result = {};

    for (auto &keyEvent : keyEvents) {
      auto name = names.find(keyEvent.code);
      if (name == names.end()) continue;
      KeyEvent physical = {name->second, keyEvent.code, keyEvent.isKeyDown};

      // The key keeps the name it got on key down, until it's released
      String key;
      if (physical.isKeyDown) {
        if (keyNames.count(physical.code)) continue;
        key = keyNames[physical.code] = remap(physical.name);
      } else {
        auto it = keyNames.find(physical.code);
        key = it == keyNames.end() ? remap(physical.name) : it->second;
        keyNames.erase(physical.code);
      }

      auto sent = physical.isKeyDown ? onKeyDown(key) : onKeyUp(key);

      if (applyKeysCb)
        applyKeysCb(appName, keyboard, keyboardDescription,
                    std::to_string(physical.code) + " -> " +
                        stringifyKeyEvents({physical}) + " -> " + key +
                        " -> " + stringifyKeyEvents(sent));

      result = Helpers::concatArrays(result, sent);
    }

    return result;
  }

  // ms the key that was just pressed has to be held for its hold, or -1.
  // After that time, the caller calls applyHold
  int getHoldDelay() { return pendingHoldKey.empty() ? -1 : holdDelay; }

  // Sends the hold of the held key, unless another key event came after it was
  // pressed
  KeyEvents applyHold() {
    if (pendingHoldKey.empty()) return {};

    auto key = pendingHoldKey;
    pendingHoldKey = "";
    auto &press = presses.at(key);
    press.isHoldFired = true;

    auto keyEvents =
        applyAction(rules[press.ruleIdx].holds[press.keyIdx], key, true);
    for (auto &keyEvent : keyEvents) keyEvent.repeats = false;
    trackSentKeys(keyEvents, key);

    if (applyKeysCb)
      applyKeysCb(appName, keyboard, keyboardDescription,
                  key + ":hold -> " + stringifyKeyEvents(keyEvents));

    return keyEvents;
  }

  void setAppName(String name) { appName = name; }

  void setKeyboard(String id, String description) {
    keyboard = id;
    keyboardDescription = description;
  }

  void setApplyKeysCb(std::function<void(String, String, String, String)> cb) {
    applyKeysCb = cb;
  }

  String stringifyKeyEvents(KeyEvents keyEvents) {
    String result = "";

    for (size_t i = 0; i < keyEvents.size(); i++) {
      if (i != 0) result += " ";
      auto &keyEvent = keyEvents[i];

      if (keyEvent.name == DELAY)
        result += DELAY + ":" + std::to_string(keyEvent.delay);
      else
        result += keyEvent.name + (keyEvent.isKeyDown ? ":down" : ":up");
    }

    return result;
  }

  // Space separated: "A" (tap), "A:down", "A:up", "CmdL+A" (with the keys
  // before it held, also with :down or :up) and "delay:25"
  KeyEvents getKeyEventsFromString(String str) {
    KeyEvents keyEvents = {};

    for (auto &token : Helpers::split(str, ' ')) {
      if (token.empty()) continue;

      if (token.rfind(DELAY + ":", 0) == 0) {
        keyEvents.push_back(
            {DELAY, 0, true, false, atoi(token.substr(DELAY.size() + 1).c_str())});
        continue;
      }

      auto [keys, state] = splitToken(token);
      if (state != "up")
        for (auto &key : keys) keyEvents.push_back(getKeyEvent(key, true));
      if (state != "down")
        for (auto it = keys.rbegin(); it != keys.rend(); it++)
          keyEvents.push_back(getKeyEvent(*it, false));
    }

    return keyEvents;
  }

  void reset() {
    keyNames.clear();
    heldKeys.clear();
    variables.clear();
    presses.clear();
    sentKeys.clear();
    pendingHoldKey = "";
    lastTapKey = "";
    tapCount = 0;
    lastTapTime = 0;
    appName = "";
    keyboard = "";
    keyboardDescription = "";
  }

 private:
  const String DELAY = "delay";
  const String SET = "set";
  const String UNSET = "unset";
  const std::set<String> MODIFIERS = {"CmdL",   "CmdR",   "AltL",
                                      "AltR",   "CtrlL",  "CtrlR",
                                      "ShiftL", "ShiftR", "Fn"};

  struct Remap {
    String from, to;
    Strings apps, keyboards;
  };

  struct Rule {
    Strings keys, modifiers, optional, apps, keyboards, ifs, unlesses;
    // One per key, empty when not set
    Strings tos, taps, doubleTaps, holds, afterKeyUps;
  };

  // A pressed key, until it's released
  struct Press {
    int ruleIdx;  // -1 when no rule matched, then the key itself is sent
    size_t keyIdx;
    // Keys of its modifiers that were released while it's pressed, with the
    // modifier that sent them
    std::vector<std::pair<String, String>> liftedKeys;
    bool isAlone;
    bool isHoldFired;
    double downTime;
  };

  std::map<String, ushort> codes;  // key name -> scancode
  std::map<ushort, String> names;  // scancode -> key name
  std::vector<Remap> remaps;
  std::vector<Rule> rules;
  double tapDelay;
  int holdDelay;

  String appName, keyboard, keyboardDescription;
  // appName, keyboardId, keyboardDescription, keyEvents
  std::function<void(String, String, String, String)> applyKeysCb;

  std::map<ushort, String> keyNames;  // scancode -> name after the remaps
  std::set<String> heldKeys;
  std::set<String> variables;  // the ones that are on
  std::map<String, Press> presses;
  // Keys sent down that are still down, with the key that sent them, in the
  // order they were sent
  std::vector<std::pair<String, String>> sentKeys;
  String pendingHoldKey;
  String lastTapKey;
  int tapCount;
  double lastTapTime;

  KeyEvents onKeyDown(String key) {
    pendingHoldKey = "";
    for (auto &[_, press] : presses) press.isAlone = false;
    if (key != lastTapKey) {
      lastTapKey = "";
      tapCount = 0;
    }
    heldKeys.insert(key);

    Press press = {-1, 0, {}, true, false, now()};
    KeyEvents keyEvents = {};

    for (size_t i = 0; i < rules.size() && press.ruleIdx < 0; i++) {
      auto &keys = rules[i].keys;
      auto it = std::find(keys.begin(), keys.end(), key);
      if (it == keys.end() || !matches(rules[i], key)) continue;
      press.ruleIdx = i;
      press.keyIdx = it - keys.begin();
    }

    if (press.ruleIdx < 0) {
      keyEvents = {getKeyEvent(key, true)};
      trackSentKeys(keyEvents, key);
    } else {
      auto &rule = rules[press.ruleIdx];

      for (auto &[sentKey, sender] : sentKeys)
        if (contains(rule.modifiers, sender))
          press.liftedKeys.push_back({sentKey, sender});
      for (auto it = press.liftedKeys.rbegin(); it != press.liftedKeys.rend();
           it++)
        keyEvents.push_back(getKeyEvent(it->first, false));
      trackSentKeys(keyEvents);

      auto sent = applyAction(rule.tos[press.keyIdx], key, true);
      trackSentKeys(sent, key);
      keyEvents = Helpers::concatArrays(keyEvents, sent);

      if (!rule.holds[press.keyIdx].empty()) pendingHoldKey = key;
    }

    presses[key] = press;
    return keyEvents;
  }

  KeyEvents onKeyUp(String key) {
    pendingHoldKey = "";
    heldKeys.erase(key);

    auto it = presses.find(key);
    if (it == presses.end()) return releaseSentKeys(key);
    auto press = it->second;
    presses.erase(it);

    auto keyEvents = releaseSentKeys(key);

    // Only the real modifiers come back, pressing any other key again would
    // type it
    for (auto &[liftedKey, sender] : press.liftedKeys) {
      if (!MODIFIERS.count(liftedKey) || !heldKeys.count(sender) ||
          findSentKey(liftedKey) != sentKeys.end())
        continue;
      keyEvents.push_back(getKeyEvent(liftedKey, true));
      sentKeys.push_back({liftedKey, sender});
    }

    bool isTap = press.ruleIdx >= 0 && press.isAlone && !press.isHoldFired &&
                 now() - press.downTime < tapDelay;
    if (!isTap) {
      lastTapKey = "";
      tapCount = 0;
    }
    if (press.ruleIdx < 0) return keyEvents;

    auto &rule = rules[press.ruleIdx];
    if (isTap) {
      bool isNextTap =
          lastTapKey == key && press.downTime - lastTapTime < tapDelay;
      tapCount = isNextTap ? tapCount + 1 : 1;
      lastTapKey = key;
      lastTapTime = now();

      auto tap = rule.taps[press.keyIdx];
      if (tapCount == 2 && !rule.doubleTaps[press.keyIdx].empty()) {
        tap = rule.doubleTaps[press.keyIdx];
        lastTapKey = "";
        tapCount = 0;
      } else {
        tapCount = 1;
      }

      auto tapped = applyAction(tap, key, false);
      trackSentKeys(tapped);
      keyEvents = Helpers::concatArrays(keyEvents, tapped);
    }

    auto afterKeyUp = applyAction(rule.afterKeyUps[press.keyIdx], key, false);
    trackSentKeys(afterKeyUp);
    return Helpers::concatArrays(keyEvents, afterKeyUp);
  }

  // Sets and unsets the variables of action and returns its keys, tapped. With
  // holdsLast, the last one is left down
  KeyEvents applyAction(String action, String currentKey, bool holdsLast) {
    Strings tokens;
    for (auto &token : Helpers::split(action, ' ')) {
      if (token.rfind(SET + ":", 0) == 0)
        variables.insert(token.substr(SET.size() + 1));
      else if (token.rfind(UNSET + ":", 0) == 0)
        variables.erase(token.substr(UNSET.size() + 1));
      else if (!token.empty())
        tokens.push_back(token);
    }

    String str = "";
    for (size_t i = 0; i < tokens.size(); i++) {
      auto token = tokens[i];
      if (token.rfind(DELAY + ":", 0) != 0) {
        auto keys = Helpers::split(token, '+');
        token = "";
        for (auto &key : keys)
          token += (token.empty() ? "" : "+") +
                   (key == "currentKey" ? currentKey : key);
        if (holdsLast && i == tokens.size() - 1) token += ":down";
      }
      str += token + " ";
    }

    return getKeyEventsFromString(str);
  }

  bool matches(Rule &rule, String key) {
    if (!rule.apps.empty() && !contains(rule.apps, appName)) return false;
    if (!rule.keyboards.empty() && !contains(rule.keyboards, keyboard))
      return false;
    for (auto &variable : rule.ifs)
      if (!variables.count(variable)) return false;
    for (auto &variable : rule.unlesses)
      if (variables.count(variable)) return false;

    for (auto &modifier : rule.modifiers)
      if (!heldKeys.count(modifier)) return false;

    if (contains(rule.optional, "any")) return true;
    for (auto &heldKey : heldKeys) {
      if (heldKey == key || !MODIFIERS.count(heldKey)) continue;
      if (!contains(rule.modifiers, heldKey) &&
          !contains(rule.optional, heldKey))
        return false;
    }

    return true;
  }

  String remap(String key) {
    for (auto &remap : remaps) {
      if (remap.from != key) continue;
      if (!remap.apps.empty() && !contains(remap.apps, appName)) continue;
      if (!remap.keyboards.empty() && !contains(remap.keyboards, keyboard))
        continue;
      return remap.to;
    }

    return key;
  }

  // sender is who keeps the keys down, empty for the keys that aren't left down
  void trackSentKeys(const KeyEvents &keyEvents, String sender = "") {
    for (auto &keyEvent : keyEvents) {
      if (!codes.count(keyEvent.name)) continue;
      auto it = findSentKey(keyEvent.name);
      if (it != sentKeys.end()) sentKeys.erase(it);
      if (keyEvent.isKeyDown && !sender.empty())
        sentKeys.push_back({keyEvent.name, sender});
    }
  }

  // The keys sender left down, the last sent first. The ones the user is
  // holding are released when the user releases them
  KeyEvents releaseSentKeys(String sender) {
    KeyEvents keyEvents = {};
    for (auto it = sentKeys.rbegin(); it != sentKeys.rend(); it++) {
      if (it->second != sender) continue;
      if (heldKeys.count(it->first))
        it->second = it->first;
      else
        keyEvents.push_back(getKeyEvent(it->first, false));
    }
    trackSentKeys(keyEvents);
    return keyEvents;
  }

  std::vector<std::pair<String, String>>::iterator findSentKey(String key) {
    return std::find_if(sentKeys.begin(), sentKeys.end(),
                        [&](auto &sentKey) { return sentKey.first == key; });
  }

  static bool contains(const Strings &strings, const String &str) {
    return std::find(strings.begin(), strings.end(), str) != strings.end();
  }

  // "CmdL+A:down" -> {CmdL, A}, "down"
  std::pair<Strings, String> splitToken(String token) {
    String state = "";
    auto colonIdx = token.find(':');
    if (colonIdx != String::npos) {
      state = token.substr(colonIdx + 1);
      token = token.substr(0, colonIdx);
    }
    return {Helpers::split(token, '+'), state};
  }

  // Config parsing. Everything is validated here, so a mistake fails when the
  // profile loads instead of when a key is pressed

  Remap parseRemap(json &remap) {
    auto where = "The remap " + remap.dump();
    validateFields(remap, where, {"from", "to", "app", "keyboard"});
    return {getKey(remap, "from", where), getKey(remap, "to", where),
            getStrings(remap, "app", where),
            getStrings(remap, "keyboard", where)};
  }

  Rule parseRule(json &rule) {
    auto where = "The rule for " + (rule.is_object() && rule.contains("from")
                                        ? rule["from"].dump()
                                        : rule.dump());
    validateFields(rule, where,
                   {"from", "modifiers", "optional", "app", "keyboard", "if",
                    "unless", "to", "tap", "doubleTap", "hold", "afterKeyUp"});

    Rule parsed;
    parsed.keys = getKeys(rule, "from", where);
    if (parsed.keys.empty())
      throw std::runtime_error(where + " needs \"from\"");
    parsed.modifiers = getKeys(rule, "modifiers", where);
    parsed.optional = getStrings(rule, "optional", where);
    for (auto &key : parsed.optional)
      if (key != "any") validateKey(key, where);
    parsed.apps = getStrings(rule, "app", where);
    parsed.keyboards = getStrings(rule, "keyboard", where);
    parsed.ifs = getStrings(rule, "if", where);
    parsed.unlesses = getStrings(rule, "unless", where);
    for (auto &variable : Helpers::concatArrays(parsed.ifs, parsed.unlesses))
      validateVariable(variable, where);

    parsed.tos = getActions(rule, "to", parsed.keys.size(), where);
    parsed.taps = getActions(rule, "tap", parsed.keys.size(), where);
    parsed.doubleTaps = getActions(rule, "doubleTap", parsed.keys.size(), where);
    parsed.holds = getActions(rule, "hold", parsed.keys.size(), where);
    parsed.afterKeyUps =
        getActions(rule, "afterKeyUp", parsed.keys.size(), where);
    return parsed;
  }

  // A string for all the keys, or an array with one per key
  Strings getActions(json &rule, String field, size_t keysSize, String where) {
    if (!rule.contains(field)) return Strings(keysSize, "");

    auto &value = rule[field];
    Strings actions;
    if (value.is_string())
      actions = Strings(keysSize, value.get<String>());
    else if (value.is_array() && value.size() == keysSize)
      actions = getStrings(rule, field, where);
    else
      throw std::runtime_error(where + ": \"" + field +
                               "\" isn't a string or an array with one item "
                               "per key");

    for (auto &action : actions) validateAction(action, where + ", " + field);
    return actions;
  }

  void validateAction(String action, String where) {
    for (auto &token : Helpers::split(action, ' ')) {
      if (token.empty()) continue;

      if (token.rfind(DELAY + ":", 0) == 0) {
        auto ms = token.substr(DELAY.size() + 1);
        if (ms.empty() || ms.find_first_not_of("0123456789") != String::npos)
          throw std::runtime_error(where + ": invalid \"" + token + "\"");
        continue;
      }

      if (token.rfind(SET + ":", 0) == 0) {
        validateVariable(token.substr(SET.size() + 1), where);
        continue;
      }
      if (token.rfind(UNSET + ":", 0) == 0) {
        validateVariable(token.substr(UNSET.size() + 1), where);
        continue;
      }

      auto [keys, state] = splitToken(token);
      if (state != "")
        throw std::runtime_error(where + ": invalid \"" + token +
                                 "\", keys are released by themselves");
      for (auto &key : keys)
        if (key != "currentKey") validateKey(key, where);
    }
  }

  void validateKey(String key, String where) {
    if (!codes.count(key))
      throw std::runtime_error(where + ": unknown key \"" + key + "\"");
  }

  // Key names aren't variables, "modifiers" checks the held keys
  void validateVariable(String variable, String where) {
    if (variable.empty())
      throw std::runtime_error(where + ": empty variable name");
    if (codes.count(variable))
      throw std::runtime_error(where + ": the variable \"" + variable +
                               "\" is a key, use \"modifiers\" for keys");
  }

  void validateFields(json &object, String where, std::set<String> fields) {
    if (!object.is_object())
      throw std::runtime_error(where + " isn't an object");
    for (auto &[field, _] : object.items())
      if (!fields.count(field))
        throw std::runtime_error(where + " has an unknown field \"" + field +
                                 "\"");
  }

  json getArray(json &object, String field, String where) {
    if (!object.contains(field)) return json::array();
    if (!object[field].is_array())
      throw std::runtime_error(where + ": \"" + field + "\" isn't an array");
    return object[field];
  }

  // A string or an array of strings
  Strings getStrings(json &object, String field, String where) {
    if (!object.contains(field)) return {};

    auto &value = object[field];
    if (value.is_string()) return {value.get<String>()};

    Strings strings;
    bool isValid = value.is_array();
    for (auto &item : value) {
      isValid = isValid && item.is_string();
      if (isValid) strings.push_back(item.get<String>());
    }
    if (!isValid)
      throw std::runtime_error(where + ": \"" + field +
                               "\" isn't a string or an array of strings");
    return strings;
  }

  Strings getKeys(json &object, String field, String where) {
    auto keys = getStrings(object, field, where);
    for (auto &key : keys) validateKey(key, where);
    return keys;
  }

  String getKey(json &object, String field, String where) {
    if (!object.contains(field) || !object[field].is_string())
      throw std::runtime_error(where + " needs \"" + field + "\"");
    auto key = object[field].get<String>();
    validateKey(key, where);
    return key;
  }

  double getNumber(json &object, String field, double defaultValue) {
    if (!object.contains(field)) return defaultValue;
    if (!object[field].is_number())
      throw std::runtime_error("The profile: \"" + field +
                               "\" isn't a number");
    return object[field].get<double>();
  }

  KeyEvent getKeyEvent(String keyName, bool isKeyDown) {
    auto it = codes.find(keyName);
    if (it == codes.end()) return {"Unknown", 0, isKeyDown};
    return {keyName, it->second, isKeyDown};
  }
};
