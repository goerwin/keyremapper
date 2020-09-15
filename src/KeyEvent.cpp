#ifndef UNICODE
#define UNICODE
#endif

#include <vector>
#include <tuple>
#include <fstream>
#include "KeyEvent.h"
#include "utils.h"
#include "json.hpp"

using json = nlohmann::json;

String g_activeProcessName;

bool g_isCtrlAsAlt;
bool g_isAltAsCtrl;

json g_keyDownStatus = {};

void print(std::string string, std::string string2 = "\n")
{
  auto str = string + string2;
  OutputDebugStringA(str.c_str());
}

json coreJson;
auto getJsonFile() {
  if (!coreJson.is_null()) {
    return coreJson;
  }
  std::ifstream i("./src/core.json");
  std::string jsonValue;
  i >> jsonValue;

  std::ifstream coreFile("./src/core.json");
  std::string coreStr((std::istreambuf_iterator<char>(coreFile)),
    std::istreambuf_iterator<char>());

  coreJson = json::parse(coreStr);
  return coreJson;
}

String getScanCodeSymbol(unsigned short code) {
	if (code == SC_A) { return "A"; }
	if (code == SC_B) { return "B"; }
	if (code == SC_C) { return "C"; }
	if (code == SC_D) { return "D"; }
	if (code == SC_E) { return "E"; }
	if (code == SC_F) { return "F"; }
	if (code == SC_G) { return "G"; }
	if (code == SC_H) { return "H"; }
	if (code == SC_I) { return "I"; }
	if (code == SC_J) { return "J"; }
	if (code == SC_K) { return "K"; }
	if (code == SC_L) { return "L"; }
	if (code == SC_M) { return "M"; }
	if (code == SC_O) { return "O"; }
	if (code == SC_P) { return "P"; }
	if (code == SC_Q) { return "Q"; }
	if (code == SC_R) { return "R"; }
	if (code == SC_S) { return "S"; }
	if (code == SC_T) { return "T"; }
	if (code == SC_U) { return "U"; }
	if (code == SC_V) { return "V"; }
	if (code == SC_W) { return "W"; }
	if (code == SC_X) { return "X"; }
	if (code == SC_Y) { return "Y"; }
	if (code == SC_Z) { return "Z"; }
	if (code == SC_1) { return "1"; }
	if (code == SC_2) { return "2"; }
	if (code == SC_3) { return "3"; }
	if (code == SC_4) { return "4"; }
	if (code == SC_5) { return "5"; }
	if (code == SC_6) { return "6"; }
	if (code == SC_7) { return "7"; }
	if (code == SC_8) { return "8"; }
	if (code == SC_9) { return "9"; }
	if (code == SC_0) { return "0"; }
	if (code == SC_NP1) { return "NP1"; }
	if (code == SC_NP2) { return "NP2"; }
	if (code == SC_NP3) { return "NP3"; }
	if (code == SC_NP4) { return "NP4"; }
	if (code == SC_NP5) { return "NP5"; }
	if (code == SC_NP6) { return "NP6"; }
	if (code == SC_NP7) { return "NP7"; }
	if (code == SC_NP8) { return "NP8"; }
	if (code == SC_NP9) { return "NP9"; }
	if (code == SC_NP0) { return "NP0"; }
	if (code == SC_MUTE) { return "MUTE"; }
	if (code == SC_VOLUMEDOWN) { return "VOLUMEDOWN"; }
	if (code == SC_VOLUMEUP) { return "VOLUMEUP"; }
	if (code == SC_ESC) { return "Esc"; }
	if (code == SC_CAPSLOCK) { return "Caps"; }
	if (code == SC_LEFT) { return "Left"; }
	if (code == SC_RIGHT) { return "Right"; }
	if (code == SC_UP) { return "Up"; }
	if (code == SC_DOWN) { return "Down"; }
	if (code == SC_SPACE) { return "Space"; }
	if (code == SC_LWIN) { return "Win"; }
	if (code == SC_RWIN) { return "Win"; }
	if (code == SC_LALT) { return "Alt"; }
	if (code == SC_RALT) { return "Alt"; }
	if (code == SC_LCTRL) { return "Ctrl"; }
	if (code == SC_RCTRL) { return "Ctrl"; }
	if (code == SC_LSHIFT) { return "Shift"; }
	if (code == SC_RSHIFT) { return "Shift"; }
	if (code == SC_LBSLASH) { return "LBSLASH"; }
	if (code == SC_RETURN) { return "Enter"; }
	if (code == SC_SUPR) { return "Supr"; }
	if (code == SC_BACK) { return "Back"; }
	if (code == SC_TAB) { return "Tab"; }
	if (code == SC_HOME) { return "Home"; }
	if (code == SC_END) { return "End"; }
	if (code == SC_PRIOR) { return "Prior"; }
	if (code == SC_NEXT) { return "Next"; }
	if (code == SC_SEMI) { return ";"; }
	if (code == SC_MINUS) { return "-"; }
	if (code == SC_GRAVE) { return "`"; }
	if (code == SC_NUMLOCK) { return "NumLock"; }
	if (code == SC_INSERT) { return "Insert"; }
	if (code == SC_DELETE) { return "Del"; }
	if (code == SC_F1) { return "F1"; }
	if (code == SC_F2) { return "F2"; }
	if (code == SC_F3) { return "F3"; }
	if (code == SC_F4) { return "F4"; }
	if (code == SC_F5) { return "F5"; }
	if (code == SC_F6) { return "F6"; }
	if (code == SC_F7) { return "F7"; }
	if (code == SC_F8) { return "F8"; }
	if (code == SC_F9) { return "F9"; }
	if (code == SC_F10) { return "F10"; }
	if (code == SC_F11) { return "F11"; }
	if (code == SC_F12) { return "F12"; }
	if (code == SC_MOUSELEFT) { return "MOUSELEFT"; }
	if (code == SC_MOUSERIGHT) { return "MOUSERIGHT"; }
	if (code == SC_BRIGHTNESSDOWN) { return "BRIGHTNESSDOWN"; }
	if (code == SC_BRIGHTNESSUP) { return "BRIGHTNESSUP"; }
	if (code == SC_GENERAL) { return "_"; }
  if (code == SC_NULL) { return "NULL"; }

	return "KEY_CODE_NOT_FOUND";
}

String getStateSymbol(unsigned short state) {
	switch (state) {
		case 0: case 2:
			return "↓";
		case 1: case 3:
			return "↑";
		case 4: case 5:
			return "↕";
	}

	return "KEY_STATE_NOT_FOUND";
}

ScanCodes getScanCode(std::string symbol) {
	if (symbol == "A") { return SC_A; }
	if (symbol == "B") { return SC_B; }
	if (symbol == "C") { return SC_C; }
	if (symbol == "D") { return SC_D; }
	if (symbol == "E") { return SC_E; }
	if (symbol == "F") { return SC_F; }
	if (symbol == "G") { return SC_G; }
	if (symbol == "H") { return SC_H; }
	if (symbol == "I") { return SC_I; }
	if (symbol == "J") { return SC_J; }
	if (symbol == "K") { return SC_K; }
	if (symbol == "L") { return SC_L; }
	if (symbol == "M") { return SC_M; }
	if (symbol == "N") { return SC_N; }
	if (symbol == "O") { return SC_O; }
	if (symbol == "P") { return SC_P; }
	if (symbol == "Q") { return SC_Q; }
	if (symbol == "R") { return SC_R; }
	if (symbol == "S") { return SC_S; }
	if (symbol == "T") { return SC_T; }
	if (symbol == "U") { return SC_U; }
	if (symbol == "V") { return SC_V; }
	if (symbol == "W") { return SC_W; }
	if (symbol == "X") { return SC_X; }
	if (symbol == "Y") { return SC_Y; }
	if (symbol == "Z") { return SC_Z; }
	if (symbol == "1") { return SC_1; }
	if (symbol == "2") { return SC_2; }
	if (symbol == "3") { return SC_3; }
	if (symbol == "4") { return SC_4; }
	if (symbol == "5") { return SC_5; }
	if (symbol == "6") { return SC_6; }
	if (symbol == "7") { return SC_7; }
	if (symbol == "8") { return SC_8; }
	if (symbol == "9") { return SC_9; }
	if (symbol == "0") { return SC_0; }
	if (symbol == "NP1") { return SC_NP1; }
	if (symbol == "NP2") { return SC_NP2; }
	if (symbol == "NP3") { return SC_NP3; }
	if (symbol == "NP4") { return SC_NP4; }
	if (symbol == "NP5") { return SC_NP5; }
	if (symbol == "NP6") { return SC_NP6; }
	if (symbol == "NP7") { return SC_NP7; }
	if (symbol == "NP8") { return SC_NP8; }
	if (symbol == "NP9") { return SC_NP9; }
	if (symbol == "NP0") { return SC_NP0; }
	if (symbol == "MUTE") { return SC_MUTE; }
	if (symbol == "VOLUMEDOWN") { return SC_VOLUMEDOWN; }
	if (symbol == "VOLUMEUP") { return SC_VOLUMEUP; }
	if (symbol == "Esc") { return SC_ESC; }
	if (symbol == "Caps") { return SC_CAPSLOCK; }
	if (symbol == "Left") { return SC_LEFT; }
	if (symbol == "Right") { return SC_RIGHT; }
	if (symbol == "Up") { return SC_UP; }
	if (symbol == "Down") { return SC_DOWN; }
	if (symbol == "Space") { return SC_SPACE; }
	if (symbol == "Win") { return SC_LWIN; }
	if (symbol == "Win") { return SC_RWIN; }
	if (symbol == "Alt") { return SC_LALT; }
	if (symbol == "Alt") { return SC_RALT; }
	if (symbol == "Ctrl") { return SC_LCTRL; }
	if (symbol == "Ctrl") { return SC_RCTRL; }
	if (symbol == "Shift") { return SC_LSHIFT; }
	if (symbol == "Shift") { return SC_RSHIFT; }
	if (symbol == "LBSLASH") { return SC_LBSLASH; }
	if (symbol == "Enter") { return SC_RETURN; }
	if (symbol == "Supr") { return SC_SUPR; }
	if (symbol == "Back") { return SC_BACK; }
	if (symbol == "Tab") { return SC_TAB; }
	if (symbol == "Home") { return SC_HOME; }
	if (symbol == "End") { return SC_END; }
	if (symbol == "Prior") { return SC_PRIOR; }
	if (symbol == "Next") { return SC_NEXT; }
	if (symbol == ";") { return SC_SEMI; }
	if (symbol == "-") { return SC_MINUS; }
	if (symbol == "`") { return SC_GRAVE; }
	if (symbol == "NumLock") { return SC_NUMLOCK; }
	if (symbol == "Insert") { return SC_INSERT; }
	if (symbol == "Del") { return SC_DELETE; }
	if (symbol == "F1") { return SC_F1; }
	if (symbol == "F2") { return SC_F2; }
	if (symbol == "F3") { return SC_F3; }
	if (symbol == "F4") { return SC_F4; }
	if (symbol == "F5") { return SC_F5; }
	if (symbol == "F6") { return SC_F6; }
	if (symbol == "F7") { return SC_F7; }
	if (symbol == "F8") { return SC_F8; }
	if (symbol == "F9") { return SC_F9; }
	if (symbol == "F10") { return SC_F10; }
	if (symbol == "F11") { return SC_F11; }
	if (symbol == "F12") { return SC_F12; }
	if (symbol == "MOUSELEFT") { return SC_MOUSELEFT; }
	if (symbol == "MOUSERIGHT") { return SC_MOUSERIGHT; }
	if (symbol == "BRIGHTNESSDOWN") { return SC_BRIGHTNESSDOWN; }
	if (symbol == "BRIGHTNESSUP") { return SC_BRIGHTNESSUP; }
	if (symbol == "_") { return SC_GENERAL; }
  if (symbol == "NULL") { return SC_NULL; }

	return SC_NULL;
}

Keys concatKeyVectors(Keys keys, Keys keys2, Keys keys3 = {}, Keys keys4 = {}) {
	keys.insert(keys.end(), keys2.begin(), keys2.end());
	keys.insert(keys.end(), keys3.begin(), keys3.end());
	keys.insert(keys.end(), keys4.begin(), keys4.end());
	return keys;
}

Keys keyDownCtrlAsAlt() {
	Keys keys;

	if (!g_isCtrlAsAlt && g_keyDownStatus["Ctrl"]) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isCtrlAsAlt = true;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
Keys keyDownCtrlAsCtrl() {
	Keys keys;

	if (g_isCtrlAsAlt && g_keyDownStatus["Ctrl"]) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	}

	g_isCtrlAsAlt = false;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
Keys keyUpCtrl() {
	Keys keys;

	if (g_isCtrlAsAlt) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	} else {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isCtrlAsAlt = true;
	return keys;
}

Keys keyDownAltAsCtrl() {
	Keys keys;

	if (!g_isAltAsCtrl && g_keyDownStatus["Alt"]) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	}

	g_isAltAsCtrl = true;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
Keys keyDownAltAsAlt() {
	Keys keys;

	if (g_isAltAsCtrl && g_keyDownStatus["Alt"]) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isAltAsCtrl = false;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
Keys keyUpAlt() {
	Keys keys;

	if (g_isAltAsCtrl) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	} else {
		keys.insert(keys.end(), { KeyUp(SC_LALT) });
	}

	g_isAltAsCtrl = true;
	return keys;
}

struct KeyInfo {
	ScanCodes code;
	String program;

	KeyInfo() {}

	KeyInfo(ScanCodes _code, String _program = "") {
		code = _code;
		program = _program;
	}
};

Key g_nullKey = KeyUp(SC_NULL);

unsigned short getVimArrowKeyCode(unsigned short keyCode) {
	return keyCode == SC_K ? SC_UP :
		keyCode == SC_J ? SC_DOWN :
		keyCode == SC_L ? SC_RIGHT :
		keyCode == SC_H ? SC_LEFT :
		SC_NULL;
}

unsigned short getVimHomeEndKeyCode(unsigned short keyCode) {
	return (keyCode == SC_K || keyCode == SC_H) ? SC_HOME :
		(keyCode == SC_J || keyCode == SC_L) ? SC_END :
		SC_NULL;
}

namespace KeyEvent {
	bool isKeyDown(Key key) {
		return key.state == 0 || key.state == 2;
	}

  bool isKeyMatches(json ruleKey, std::string key)
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

  bool isWhenMatches(json when) {
    if (when.is_object()) {
      for (json::iterator i = when.begin(); i != when.end(); ++i) {
        auto key = i.key();
        auto value = i.value();
        auto keyStatus = g_keyDownStatus[key];

        if (value == "isDown") {
          if (keyStatus.is_null() || keyStatus == false) {
            return false;
          }
        }
        else if (value == "isUp") {
          if (keyStatus == true) {
            return false;
          }
        }
      }
    }

    return true;
  }

  json getFireKeysFromRule(json rule, std::string key = NULL)
  {
    auto ruleKey = rule["key"];
    auto when = rule["when"];
    auto fire = rule["fire"];
    auto rules = rule["rules"];

    bool keyMatches = isKeyMatches(ruleKey, key);
    //bool conditionMatches = isConditionMatches(condition);
    bool whenMatches = isWhenMatches(when);

    if (!keyMatches || !whenMatches)
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

  json getFireKeys(json keybindings)
  {
    for (auto kb = keybindings.begin(); kb != keybindings.end(); ++kb)
    {
      auto kbValue = kb.value();
      auto kbModifier = kbValue["modifier"];

      if (!kbModifier.is_array()) {
        return nullptr;
      }

      // verify all modifiers are pressed down
        auto currentKey = g_keyDownStatus["currentKey"];
        auto interceptAll = kbValue["interceptAll"];
        bool interceptAllValue = !interceptAll.is_null() && interceptAll == true;
        auto skipKeybinding = false;
        auto isCurrentKeyInModifier = false;

        for (auto m = 0; m < kbModifier.size(); m++) {
          auto mod = std::string(kbModifier[m]);
          auto modKeyDownStatus = g_keyDownStatus[mod];
          isCurrentKeyInModifier = false;

          if (currentKey == mod) {
            isCurrentKeyInModifier = true;
          }

          if ((modKeyDownStatus.is_null() || modKeyDownStatus == false) && !isCurrentKeyInModifier) {
            skipKeybinding = true;
            break;
          }

          if (m == kbModifier.size() - 1 && !isCurrentKeyInModifier && !interceptAllValue) {
            skipKeybinding = true;
            break;
          }
        }

        if (skipKeybinding) {
          continue;
        }

        return {
          { "fireKeys", getFireKeysFromRule(kbValue, currentKey) },
          { "interceptAll", interceptAllValue },
          { "isCurrentKeyInModifier", isCurrentKeyInModifier }
        };
      }
  }

  Keys concatKeys(Keys keys, Key key) {
    keys.insert(keys.end(), key);

    return keys;
  }

  Keys getKeysFromDescription(json keysDescription, short currentKeyCode) {
    if (keysDescription.is_null() || !keysDescription.is_array()) {
      return {};
    }

      Keys allKeys;

      for (auto i = keysDescription.begin(); i != keysDescription.end(); ++i) {
        auto keyDesc = i.value();

        if (keyDesc.is_null()) {
          return allKeys;
        }

        std::string keyDescStr = keyDesc;
        std::string keyValue = keyDescStr;
        std::string keyStatus;

        json keyStatuses = { "_d", "_u", "_du" };
        for (auto i = keyStatuses.begin(); i < keyStatuses.end(); ++i) {
          std::string keyStatusLc = i.value();
          size_t upDownIdx = keyDescStr.find(keyStatusLc);

          if (upDownIdx != std::string::npos) {
            keyStatus = keyStatusLc;
            keyValue = keyDescStr.substr(0, upDownIdx);
            break;
          }
        }

        if (keyValue == "keyDownAltAsCtrl") {
          allKeys = concatKeyVectors(allKeys, keyDownAltAsCtrl());
          continue;
        }
        else if (keyValue == "keyDownAltAsAlt") {
          allKeys = concatKeyVectors(allKeys, keyDownAltAsAlt());
          continue;
        }
        else if (keyValue == "keyUpAlt") {
          allKeys = concatKeyVectors(allKeys, keyUpAlt());
          continue;
        }
        else if (keyValue == "keyDownCtrlAsCtrl") {
          allKeys = concatKeyVectors(allKeys, keyDownCtrlAsCtrl());
          continue;
        }
        else if (keyValue == "keyDownCtrlAsAlt") {
          allKeys = concatKeyVectors(allKeys, keyDownCtrlAsAlt());
          continue;
        }
        else if (keyValue == "keyUpCtrl") {
          allKeys = concatKeyVectors(allKeys, keyUpCtrl());
          continue;
        }

        short keyCode;
        if (keyValue == "vimArrowKey") {
          keyCode = getVimArrowKeyCode(currentKeyCode);
        }
        else if (keyValue == "vimHomeEndKey") {
          keyCode = getVimHomeEndKeyCode(currentKeyCode);
        }
        else {
          keyCode = getScanCode(keyValue);
        }

        if (keyStatus == "_d") {
          allKeys = concatKeys(allKeys, KeyDown(keyCode));
        }
        else if (keyStatus == "_u") {
          allKeys = concatKeys(allKeys, KeyUp(keyCode));
        }
        else {
          allKeys = concatKeys(allKeys, KeyDown(keyCode));
          allKeys = concatKeys(allKeys, KeyUp(keyCode));
        }
      }

      return allKeys;
  }

  // PRINTING
  void printKeys(Keys keys) {
    for (auto i = 0; i < keys.size(); i++) {
      auto key = keys[i];
      print(getScanCodeSymbol(key.code) + (KeyEvent::isKeyDown(key) ? ":down" : ":up "));
    }
  }

	Keys getKeyEvents(Keys keys) {
		Keys allKeys;
		int size = keys.size();

    for (int i = 0; i < size; i++) {
      Key key = keys[i];
      Keys keys;
      auto keyCode = key.code;

      bool isKeyDownEl = isKeyDown(key);
      auto keyValue = getScanCodeSymbol(keyCode);
      g_keyDownStatus["currentKey"] = keyValue;
      g_keyDownStatus["currentKeyDown"] = isKeyDownEl;
      g_keyDownStatus[keyValue] = isKeyDownEl;

      auto file = getJsonFile();
      auto fireKeysRes = getFireKeys(file);

      if (!fireKeysRes.is_null()) {
        auto fireKeys = fireKeysRes["fireKeys"];
        auto interceptAll = fireKeysRes["interceptAll"];
        auto isCurrentKeyInModifier = fireKeysRes["isCurrentKeyInModifier"];

        //print(keyValue + ":" + (isKeyDownEl ? "down:" : "up:") + fireKeys.dump());

        if (isKeyDownEl) {
          allKeys = concatKeyVectors(allKeys, getKeysFromDescription(fireKeys[0], keyCode));
        }
        else {
          allKeys = concatKeyVectors(allKeys, getKeysFromDescription(fireKeys[1], keyCode));
        }

        if (!interceptAll && !isCurrentKeyInModifier) {
          //print(keyValue + ":" + (isKeyDownEl ? "down:" : "up:") + keyValue);

          if (isKeyDownEl) {
            allKeys = concatKeys(allKeys, KeyDown(keyCode));
          }
          else {
            allKeys = concatKeys(allKeys, KeyUp(keyCode));
          }
        }
      }
      else {
        //print(keyValue + ":" + (isKeyDownEl ? "down:" : "up:") + keyValue);
        if (isKeyDownEl) {
          allKeys = concatKeys(allKeys, KeyDown(keyCode));
        }
        else {
          allKeys = concatKeys(allKeys, KeyUp(keyCode));
        }
      }
    }

    printKeys(allKeys);
    //return {};
		return allKeys;
	}

	void setActiveProcessName(std::string _activeProcessName) {
		g_activeProcessName = _activeProcessName;

		OutputDebugStringA(String(g_activeProcessName + "\n").c_str());
	}
}
