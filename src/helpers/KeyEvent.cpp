#ifndef UNICODE
#define UNICODE
#endif

#include <vector>
#include <tuple>
#include "KeyEvent.h"
#include "utils.h"

String g_activeProcessName;

bool g_isCapslockKeyDown;
bool g_isShiftKeyDown;
bool g_isCtrlKeyDown;
bool g_isWinKeyDown;
bool g_isAltKeyDown;
bool g_isEscKeyDown;

bool g_isCtrlAsAlt;
bool g_isAltAsCtrl;

bool g_isMouseClickDown;
bool g_isVimShiftKeyDown;

enum LAltLCtrl
{
  _keyDownLAltAsLCtrl = 1,
  _keyDownLAltAsLAlt = 2,
  _keyUpLAlt = 3,
  _keyDownLCtrlAsLAlt = 4,
  _keyDownLCtrlAsLCtrl = 5,
  _keyUpLCtrl = 6,
  _null = 0
};

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

String getKeySymbols(Keys keys) {
	int keysSize = keys.size();
	String result = "";

	for (int i = 0; i < keysSize; i++) {
		auto code = keys[i].code;
		String codeSymbol = getScanCodeSymbol(code);
		String stateSymbol = getStateSymbol(keys[i].state);

		if (i > 0) {
			if (code == keys[i - 1].code) {
				codeSymbol = "";
			} else {
				result = result.append(" ");
			}
		}

		result = result.append(codeSymbol).append(stateSymbol);
	}

	return result;
}

ScanCodes getScanCode(std::wstring symbol) {
	if (symbol == L"A") { return SC_A; }
	if (symbol == L"B") { return SC_B; }
	if (symbol == L"C") { return SC_C; }
	if (symbol == L"D") { return SC_D; }
	if (symbol == L"E") { return SC_E; }
	if (symbol == L"F") { return SC_F; }
	if (symbol == L"G") { return SC_G; }
	if (symbol == L"H") { return SC_H; }
	if (symbol == L"I") { return SC_I; }
	if (symbol == L"J") { return SC_J; }
	if (symbol == L"K") { return SC_K; }
	if (symbol == L"L") { return SC_L; }
	if (symbol == L"M") { return SC_M; }
	if (symbol == L"N") { return SC_N; }
	if (symbol == L"O") { return SC_O; }
	if (symbol == L"P") { return SC_P; }
	if (symbol == L"Q") { return SC_Q; }
	if (symbol == L"R") { return SC_R; }
	if (symbol == L"S") { return SC_S; }
	if (symbol == L"T") { return SC_T; }
	if (symbol == L"U") { return SC_U; }
	if (symbol == L"V") { return SC_V; }
	if (symbol == L"W") { return SC_W; }
	if (symbol == L"X") { return SC_X; }
	if (symbol == L"Y") { return SC_Y; }
	if (symbol == L"Z") { return SC_Z; }
	if (symbol == L"1") { return SC_1; }
	if (symbol == L"2") { return SC_2; }
	if (symbol == L"3") { return SC_3; }
	if (symbol == L"4") { return SC_4; }
	if (symbol == L"5") { return SC_5; }
	if (symbol == L"6") { return SC_6; }
	if (symbol == L"7") { return SC_7; }
	if (symbol == L"8") { return SC_8; }
	if (symbol == L"9") { return SC_9; }
	if (symbol == L"0") { return SC_0; }
	if (symbol == L"NP1") { return SC_NP1; }
	if (symbol == L"NP2") { return SC_NP2; }
	if (symbol == L"NP3") { return SC_NP3; }
	if (symbol == L"NP4") { return SC_NP4; }
	if (symbol == L"NP5") { return SC_NP5; }
	if (symbol == L"NP6") { return SC_NP6; }
	if (symbol == L"NP7") { return SC_NP7; }
	if (symbol == L"NP8") { return SC_NP8; }
	if (symbol == L"NP9") { return SC_NP9; }
	if (symbol == L"NP0") { return SC_NP0; }
	if (symbol == L"MUTE") { return SC_MUTE; }
	if (symbol == L"VOLUMEDOWN") { return SC_VOLUMEDOWN; }
	if (symbol == L"VOLUMEUP") { return SC_VOLUMEUP; }
	if (symbol == L"Esc") { return SC_ESC; }
	if (symbol == L"Caps") { return SC_CAPSLOCK; }
	if (symbol == L"Left") { return SC_LEFT; }
	if (symbol == L"Right") { return SC_RIGHT; }
	if (symbol == L"Up") { return SC_UP; }
	if (symbol == L"Down") { return SC_DOWN; }
	if (symbol == L"Space") { return SC_SPACE; }
	if (symbol == L"Win") { return SC_LWIN; }
	if (symbol == L"Win") { return SC_RWIN; }
	if (symbol == L"Alt") { return SC_LALT; }
	if (symbol == L"Alt") { return SC_RALT; }
	if (symbol == L"Ctrl") { return SC_LCTRL; }
	if (symbol == L"Ctrl") { return SC_RCTRL; }
	if (symbol == L"Shift") { return SC_LSHIFT; }
	if (symbol == L"Shift") { return SC_RSHIFT; }
	if (symbol == L"LBSLASH") { return SC_LBSLASH; }
	if (symbol == L"Enter") { return SC_RETURN; }
	if (symbol == L"Supr") { return SC_SUPR; }
	if (symbol == L"Back") { return SC_BACK; }
	if (symbol == L"Tab") { return SC_TAB; }
	if (symbol == L"Home") { return SC_HOME; }
	if (symbol == L"End") { return SC_END; }
	if (symbol == L"Prior") { return SC_PRIOR; }
	if (symbol == L"Next") { return SC_NEXT; }
	if (symbol == L";") { return SC_SEMI; }
	if (symbol == L"-") { return SC_MINUS; }
	if (symbol == L"`") { return SC_GRAVE; }
	if (symbol == L"NumLock") { return SC_NUMLOCK; }
	if (symbol == L"Insert") { return SC_INSERT; }
	if (symbol == L"Del") { return SC_DELETE; }
	if (symbol == L"F1") { return SC_F1; }
	if (symbol == L"F2") { return SC_F2; }
	if (symbol == L"F3") { return SC_F3; }
	if (symbol == L"F4") { return SC_F4; }
	if (symbol == L"F5") { return SC_F5; }
	if (symbol == L"F6") { return SC_F6; }
	if (symbol == L"F7") { return SC_F7; }
	if (symbol == L"F8") { return SC_F8; }
	if (symbol == L"F9") { return SC_F9; }
	if (symbol == L"F10") { return SC_F10; }
	if (symbol == L"F11") { return SC_F11; }
	if (symbol == L"F12") { return SC_F12; }
	if (symbol == L"MOUSELEFT") { return SC_MOUSELEFT; }
	if (symbol == L"MOUSERIGHT") { return SC_MOUSERIGHT; }
	if (symbol == L"BRIGHTNESSDOWN") { return SC_BRIGHTNESSDOWN; }
	if (symbol == L"BRIGHTNESSUP") { return SC_BRIGHTNESSUP; }
	if (symbol == L"_") { return SC_GENERAL; }
  if (symbol == L"NULL") { return SC_NULL; }

	return SC_NULL;
}

LAltLCtrl getLAltLCtrlCode(std::wstring symbol) {
	if (symbol == L"keyDownLAltAsLCtrl") { return _keyDownLAltAsLCtrl; }
	if (symbol == L"keyDownLAltAsLAlt") { return _keyDownLAltAsLAlt; }
	if (symbol == L"keyUpLAlt") { return _keyUpLAlt; }
	if (symbol == L"keyDownLCtrlAsLAlt") { return _keyDownLCtrlAsLAlt; }
	if (symbol == L"keyDownLCtrlAsLCtrl") { return _keyDownLCtrlAsLCtrl; }
	if (symbol == L"keyUpLCtrl") { return _keyUpLCtrl; }
	if (symbol == L"null") { return _null; }

	return _null;
}

Keys concatKeyVectors(Keys keys, Keys keys2, Keys keys3 = {}, Keys keys4 = {}) {
	keys.insert(keys.end(), keys2.begin(), keys2.end());
	keys.insert(keys.end(), keys3.begin(), keys3.end());
	keys.insert(keys.end(), keys4.begin(), keys4.end());
	return keys;
}

Keys keyDownLCtrlAsLAlt() {
	Keys keys;

	if (!g_isCtrlAsAlt && g_isCtrlKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isCtrlAsAlt = true;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
Keys keyDownLCtrlAsLCtrl() {
	Keys keys;

	if (g_isCtrlAsAlt && g_isCtrlKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	}

	g_isCtrlAsAlt = false;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
Keys keyUpLCtrl() {
	Keys keys;

	if (g_isCtrlAsAlt) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	} else {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isCtrlAsAlt = true;
	return keys;
}

Keys keyDownLAltAsLCtrl() {
	Keys keys;

	if (!g_isAltAsCtrl && g_isAltKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	}

	g_isAltAsCtrl = true;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
Keys keyDownLAltAsLAlt() {
	Keys keys;

	if (g_isAltAsCtrl && g_isAltKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isAltAsCtrl = false;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
Keys keyUpLAlt() {
	Keys keys;

	if (g_isAltAsCtrl) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	} else {
		keys.insert(keys.end(), { KeyUp(SC_LALT) });
	}

	g_isAltAsCtrl = true;
	return keys;
}

struct TemplateKeys {
	LAltLCtrl preLAltLCtrl = _null;
	LAltLCtrl posLAltLCtrl = _null;
	Keys keys;

	TemplateKeys() {}

	TemplateKeys(LAltLCtrl _preLAltLCtrl, Keys _keys, LAltLCtrl _posLAltLCtrl = _null) {
		preLAltLCtrl = _preLAltLCtrl;
		keys = _keys;
		posLAltLCtrl = _posLAltLCtrl;
	}

	TemplateKeys(Keys _keys, LAltLCtrl _posLAltLCtrl = _null) {
		posLAltLCtrl = _posLAltLCtrl;
		keys = _keys;
	}

	TemplateKeys(LAltLCtrl _preLAltLCtrl) {
		preLAltLCtrl = _preLAltLCtrl;
	}

	TemplateKeys replaceGeneral(bool replace, unsigned short code) {
		if (replace) {
			auto keysSize = keys.size();

			for (int i = 0; i < keysSize; i++) {
				if (keys[i].code == SC_GENERAL) {
					keys[i].code = code;
				}
			}
		}
		return *this;
	}

	Keys getParsedKeys() {
		Keys parsedKeys = keys;

		if (preLAltLCtrl == _null) {}
		if (preLAltLCtrl == _keyDownLAltAsLCtrl) {
			parsedKeys = concatKeyVectors(keyDownLAltAsLCtrl(), parsedKeys);
		} else if (preLAltLCtrl == _keyDownLAltAsLAlt) {
			parsedKeys = concatKeyVectors(keyDownLAltAsLAlt(), parsedKeys);
		} else if (preLAltLCtrl == _keyUpLAlt) {
			parsedKeys = concatKeyVectors(keyUpLAlt() , parsedKeys);
		} else if (preLAltLCtrl == _keyDownLCtrlAsLAlt) {
			parsedKeys = concatKeyVectors(keyDownLCtrlAsLAlt(), parsedKeys);
		} else if (preLAltLCtrl == _keyDownLCtrlAsLCtrl) {
			parsedKeys = concatKeyVectors(keyDownLCtrlAsLCtrl(), parsedKeys);
		} else if (preLAltLCtrl == _keyUpLCtrl) {
			parsedKeys = concatKeyVectors(keyUpLCtrl() , parsedKeys);
		}

		if (posLAltLCtrl == _null) {
			return parsedKeys;
		} else if (posLAltLCtrl == _keyDownLAltAsLCtrl) {
			parsedKeys = concatKeyVectors(parsedKeys, keyDownLAltAsLCtrl());
		} else if (posLAltLCtrl == _keyDownLAltAsLAlt) {
			parsedKeys = concatKeyVectors(parsedKeys, keyDownLAltAsLAlt());
		} else if (posLAltLCtrl == _keyUpLAlt) {
			parsedKeys = concatKeyVectors(parsedKeys, keyUpLAlt());
		} else if (posLAltLCtrl == _keyDownLCtrlAsLAlt) {
			parsedKeys = concatKeyVectors(parsedKeys, keyDownLCtrlAsLAlt());
		} else if (posLAltLCtrl == _keyDownLCtrlAsLCtrl) {
			parsedKeys = concatKeyVectors(parsedKeys, keyDownLCtrlAsLCtrl());
		} else if (posLAltLCtrl == _keyUpLCtrl) {
			parsedKeys = concatKeyVectors(parsedKeys, keyUpLCtrl());
		}

		return parsedKeys;
	}
};

struct KeyInfo {
	ScanCodes code;
	String program;

	KeyInfo() {
    //code = SC_A; // TODO: REMOVE THIS
  }

	KeyInfo(ScanCodes _code, String _program = "") {
		code = _code;
		program = _program;
	}
};

typedef std::tuple<KeyInfo, TemplateKeys, TemplateKeys> HotKey;
typedef std::vector<HotKey> HotKeys;

Key g_nullKey = KeyUp(SC_NULL);

TemplateKeys getTemplateKeys(std::wstring symbolKeys) {
	Keys keys;
	auto stateUpDown = Utils::utf82ws(String("↕"))[0];
	auto stateDown = Utils::utf82ws(String("↓"))[0];
	auto stateUp = Utils::utf82ws(String("↑"))[0];
	auto symbolKeysSize = symbolKeys.size();
	int keyIteration = -1;
	size_t pos = -1;
	LAltLCtrl firstLAltLCtrl = _null;
	LAltLCtrl lastLAltLCtrl = _null;

	do {
		keyIteration++;
		auto pos2 = symbolKeys.find(L" ", pos + 1);
		auto keySymbolWithStates = symbolKeys.substr(pos + 1, pos2 - pos - 1);
		pos = pos2;

		auto keySymbolWithStatesSize = keySymbolWithStates.size();
		std::wstring keySymbol;
		std::wstring keyStates;

		for (int i = 0; i < keySymbolWithStatesSize; i++) {
			auto letter = keySymbolWithStates[i];

			if (letter == stateUpDown || letter == stateDown || letter == stateUp) {
				keyStates = keyStates + letter;
			} else {
				keySymbol = keySymbol + letter;
			}
		}

		auto laltLCtrlCode = getLAltLCtrlCode(keySymbol);
		if (laltLCtrlCode != _null) {
			if (keyIteration == 0) {
				firstLAltLCtrl = laltLCtrlCode;
			} else {
				lastLAltLCtrl = laltLCtrlCode;
			}

			continue;
		}

		auto code = getScanCode(keySymbol);
		if (code == SC_NULL) {
			keys.insert(keys.end(), { g_nullKey });
			continue;
		}

		auto keyStatesSize = keyStates.size();
		for (int i = 0; i < keyStatesSize; i++) {
			auto letter = keyStates[i];

			if (letter == stateUpDown) {
				keys.insert(keys.end(), Key(code));
			} else if (letter == stateDown) {
				keys.insert(keys.end(), KeyDown(code));
			} else if (letter == stateUp) {
				keys.insert(keys.end(), KeyUp(code));
			}
		}
	} while (pos < symbolKeysSize);

	return TemplateKeys(firstLAltLCtrl, keys, lastLAltLCtrl);
}

HotKeys g_capsAltHotKeys;
HotKeys g_capsHotKeys;
HotKeys g_ctrlShiftHotKeys;
HotKeys g_ctrlHotKeys;
HotKeys g_winAltHotKeys;
HotKeys g_winShiftHotKeys;
HotKeys g_winHotKeys;
HotKeys g_altShiftHotKeys;
HotKeys g_altHotKeys;
HotKeys g_keyHotKeys;

std::pair<std::wstring, HotKey> getParsedHotKey(std::wstring hotkeySymbol) {
	size_t pos = 0;
	size_t pos2 = 0;
	std::wstring processName;

	pos = hotkeySymbol.find(L"- ", pos2);
	pos2 = hotkeySymbol.find(L" ", pos + 2);
	auto handler = hotkeySymbol.substr(pos + 2, pos2 - pos - 2);

	pos = hotkeySymbol.find(L" ", pos2);
	pos2 = hotkeySymbol.find(L" ", pos + 1);
	auto keyInputSymbol = hotkeySymbol.substr(pos + 1, pos2 - pos - 1);
	auto keyInputCode = getScanCode(keyInputSymbol);

	pos = hotkeySymbol.find(L"Down(", pos2);
	pos2 = hotkeySymbol.find(L")", pos + 1);
	auto keyDownSymbols = hotkeySymbol.substr(pos + 5, pos2 - pos - 5);

	pos = hotkeySymbol.find(L"Up(", pos2);
	pos2 = hotkeySymbol.find(L")", pos + 1);
	auto keyUpSymbols = hotkeySymbol.substr(pos + 3, pos2 - pos - 3);

	if (hotkeySymbol.find(L'"') != std::string::npos) {
		pos = hotkeySymbol.find(L'"');
		pos2 = hotkeySymbol.find(L'"', pos + 1);
		processName = hotkeySymbol.substr(pos + 1, pos2 - pos - 1);
	}

	auto keyInfo = KeyInfo(keyInputCode, Utils::ws2utf8(processName));
	auto templateKeysDown = getTemplateKeys(keyDownSymbols);
	auto templateKeysUp = getTemplateKeys(keyUpSymbols);
	auto hotKey = HotKey(keyInfo, templateKeysDown, templateKeysUp);

	return { handler, hotKey };
}

void updateHotKeys(std::pair<std::wstring, HotKey> parsedHotKey) {
	auto handler = parsedHotKey.first;
	auto hotKey = parsedHotKey.second;

	if (handler == L"CapsAlt") {
		g_capsAltHotKeys.insert(g_capsAltHotKeys.end(), hotKey);
	} else if (handler == L"Caps") {
		g_capsHotKeys.insert(g_capsHotKeys.end(), hotKey);
	} else if (handler == L"CtrlShift") {
		g_ctrlShiftHotKeys.insert(g_ctrlShiftHotKeys.end(), hotKey);
	} else if (handler == L"Ctrl") {
		g_ctrlHotKeys.insert(g_ctrlHotKeys.end(), hotKey);
	} else if (handler == L"WinAlt") {
		g_winAltHotKeys.insert(g_winAltHotKeys.end(), hotKey);
	} else if (handler == L"WinShift") {
		g_winShiftHotKeys.insert(g_winShiftHotKeys.end(), hotKey);
	} else if (handler == L"Win") {
		g_winHotKeys.insert(g_winHotKeys.end(), hotKey);
	} else if (handler == L"AltShift") {
		g_altShiftHotKeys.insert(g_altShiftHotKeys.end(), hotKey);
	} else if (handler == L"Alt") {
		g_altHotKeys.insert(g_altHotKeys.end(), hotKey);
	} else if (handler == L"Key") {
		g_keyHotKeys.insert(g_keyHotKeys.end(), hotKey);
	}
}

void setHotKeysFromFile(String filepath) {
	auto fileLines = Utils::getFileByLine(filepath);

	for (int i = 0; i < fileLines.size(); i++) {
		if (fileLines[i].find(L"- ") != std::string::npos) {
			updateHotKeys(getParsedHotKey(fileLines[i]));
		}
	}
}

Keys getParsedKeyDownUpKeys(
	bool isKeyDown,
	TemplateKeys keyDownTemplateKeys,
	TemplateKeys keyUpTemplateKeys
) {
	if (isKeyDown) {
		return keyDownTemplateKeys.getParsedKeys();
	}
	return keyUpTemplateKeys.getParsedKeys();
}

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

TemplateKeys getTemplateKeysForEsc() {
	if (!g_isAltAsCtrl) {
		return TemplateKeys({ Key(SC_ESC) }, _keyDownLAltAsLCtrl);
	}

	if (!g_isAltKeyDown) {
		return TemplateKeys({ Key(SC_ESC) });
	}

	return TemplateKeys({ g_nullKey });
}

Keys handleHotKeys(unsigned short keyCode, bool isKeyDown, HotKeys hotKeys) {
	auto hotKeysSize = hotKeys.size();

	for (int i = 0; i < hotKeysSize; i++) {
		auto hotKey = hotKeys[i];
		auto keyInfo = std::get<0>(hotKey);
		auto isGeneral = keyInfo.code == SC_GENERAL;

		if (keyCode == keyInfo.code || isGeneral) {
			auto program = keyInfo.program;

			if (program == "" || program == g_activeProcessName) {
				return getParsedKeyDownUpKeys(
					isKeyDown,
					std::get<1>(hotKey).replaceGeneral(isGeneral, keyCode),
					std::get<2>(hotKey).replaceGeneral(isGeneral, keyCode)
				);
			}
		}
	}

	return { g_nullKey };
}


Keys handleEscKey(unsigned short keyCode, bool isKeyDown) {
	if (!g_isEscKeyDown && keyCode != SC_ESC) {
		return {};
	}

	if (keyCode == SC_ESC) {
		return { g_nullKey };
	}

	if (keyCode == SC_1 || keyCode == SC_2 || keyCode == SC_3 || keyCode == SC_4) {
		auto mode = keyCode == SC_1 ? SC_MODE1 : keyCode == SC_2 ? SC_MODE2 :
			keyCode == SC_3 ? SC_MODE3 : SC_MODE4;

		return getParsedKeyDownUpKeys(
			isKeyDown,
			TemplateKeys({ KeyUp(mode) }),
			TemplateKeys({ g_nullKey })
		);
	}

	return {};
}

Keys handleMouseClick(unsigned short keyCode, bool isKeyDown) {
	if (
		!g_isMouseClickDown &&
		!(g_isWinKeyDown && keyCode == SC_C)
	) {
		return {};
	}

	if (keyCode == SC_C) {
		if (g_isCapslockKeyDown) {
			if (isKeyDown && !g_isMouseClickDown) {
				g_isMouseClickDown = true;
				return { KeyDown(SC_MOUSERIGHT) };
			}

			if (!isKeyDown && g_isMouseClickDown) {
				g_isMouseClickDown = false;
				return { KeyUp(SC_MOUSERIGHT) };
			}

			return { g_nullKey };
		}

		if (isKeyDown && !g_isMouseClickDown) {
			g_isMouseClickDown = true;
			return { KeyDown(SC_MOUSELEFT) };
		}

		if (!isKeyDown && g_isMouseClickDown) {
			g_isMouseClickDown = false;
			return { KeyUp(SC_MOUSELEFT) };
		}

		return { g_nullKey };
	}

	return { g_nullKey };
}

Keys handleVimMode(unsigned short keyCode, bool isKeyDown) {
	if (!g_isCapslockKeyDown && keyCode != SC_CAPSLOCK) {
		return {};
	}

	if (keyCode == SC_CAPSLOCK) {
		if (g_isVimShiftKeyDown) {
			g_isVimShiftKeyDown = false;

			return getParsedKeyDownUpKeys(
				isKeyDown,
				TemplateKeys({ g_nullKey }),
				TemplateKeys({ KeyUp(SC_LSHIFT) })
			);
		}

		return {};
	}

	if (keyCode == SC_LSHIFT || keyCode == SC_S) {
		g_isVimShiftKeyDown = isKeyDown;

		return getParsedKeyDownUpKeys(
			isKeyDown,
			TemplateKeys({ KeyDown(SC_LSHIFT) }),
			TemplateKeys({ KeyUp(SC_LSHIFT) })
		);
	}

	if (keyCode == SC_H || keyCode == SC_J || keyCode == SC_L || keyCode == SC_K) {
		if (g_isWinKeyDown) {
			return getParsedKeyDownUpKeys(
				isKeyDown,
				TemplateKeys({ KeyDown(SC_LCTRL), Key(getVimArrowKeyCode(keyCode)), KeyUp(SC_LCTRL) }),
				TemplateKeys({ g_nullKey })
			);
		}

		if (g_isAltKeyDown && g_isAltAsCtrl && (keyCode == SC_H || keyCode == SC_L)) {
			return getParsedKeyDownUpKeys(
				isKeyDown,
				TemplateKeys(_keyUpLAlt, { Key(getVimHomeEndKeyCode(keyCode)) }, _keyDownLAltAsLCtrl),
				TemplateKeys({ g_nullKey })
			);
		}

		if (g_isAltKeyDown && g_isAltAsCtrl) {
			return getParsedKeyDownUpKeys(
				isKeyDown,
				TemplateKeys({ Key(getVimHomeEndKeyCode(keyCode)) }),
				TemplateKeys({ g_nullKey })
			);
		}

		return getParsedKeyDownUpKeys(
			isKeyDown,
			TemplateKeys({ Key(getVimArrowKeyCode(keyCode)) }),
			TemplateKeys({ g_nullKey })
		);
	}

	return {};
}

Keys handleWinAltKeys(unsigned short keyCode, bool isKeyDown) {
	if (
		!(g_isWinKeyDown && g_isAltKeyDown) &&
		!(g_isWinKeyDown && keyCode == SC_LALT) &&
		!(g_isAltKeyDown && keyCode == SC_LWIN)

		// NOTE: For triple keys
		/*!(g_isWinKeyDown && g_isAltKeyDown && g_isShiftKeyDown) &&
		!(g_isWinKeyDown && g_isAltKeyDown && key.code == SC_LSHIFT) &&
		!(g_isWinKeyDown && g_isShiftKeyDown && key.code == SC_LALT) &&
		!(g_isAltKeyDown && g_isShiftKeyDown && key.code == SC_LWIN)*/
	) {
		return {};
	}

	return handleHotKeys(keyCode, isKeyDown, g_winAltHotKeys);
}

Keys handleCtrlShiftKeys(unsigned short keyCode, bool isKeyDown) {
	if (
		!(g_isCtrlKeyDown && g_isShiftKeyDown) &&
		!(g_isCtrlKeyDown && keyCode == SC_LSHIFT) &&
		!(g_isShiftKeyDown && keyCode == SC_LCTRL)
	) {
		return {};
	}

	return handleHotKeys(keyCode, isKeyDown, g_ctrlShiftHotKeys);
}

Keys handleCapslockAltKeys(unsigned short keyCode, bool isKeyDown) {
	if (
		!((g_isCapslockKeyDown && g_isAltKeyDown) ||
		(g_isCapslockKeyDown && keyCode == SC_LALT) ||
		(g_isAltKeyDown && keyCode == SC_CAPSLOCK))
	) {
		return {};
	}

	return handleHotKeys(keyCode, isKeyDown, g_capsAltHotKeys);
}

Keys handleCapslockKey(unsigned short keyCode, bool isKeyDown) {
	if (!g_isCapslockKeyDown && keyCode != SC_CAPSLOCK) {
		return {};
	}

	return handleHotKeys(keyCode, isKeyDown, g_capsHotKeys);
}

Keys handleCtrlKey(unsigned short keyCode, bool isKeyDown) {
	if (!g_isCtrlKeyDown && keyCode != SC_LCTRL) {
		return {};
	}

	return handleHotKeys(keyCode, isKeyDown, g_ctrlHotKeys);
}

Keys handleWinShiftKeys(unsigned short keyCode, bool isKeyDown) {
	if (
		!(g_isWinKeyDown && g_isShiftKeyDown) &&
		!(g_isWinKeyDown && keyCode == SC_LSHIFT) &&
		!(g_isShiftKeyDown && keyCode == SC_LCTRL)
	) {
		return {};
	}

	return handleHotKeys(keyCode, isKeyDown, g_winShiftHotKeys);
}

Keys handleWinKey(unsigned short keyCode, bool isKeyDown) {
	if (!(g_isWinKeyDown || keyCode == SC_LWIN)) {
		return {};
	}

	return handleHotKeys(keyCode, isKeyDown, g_winHotKeys);
}

Keys handleAltShiftKeys(unsigned short keyCode, bool isKeyDown) {
	if (!(
		(g_isAltKeyDown && g_isShiftKeyDown) ||
		(g_isAltKeyDown && keyCode == SC_LSHIFT) ||
		(g_isShiftKeyDown && keyCode == SC_LALT)
	)) {
		return {};
	}

	// Omit keys when in alt-tab mode
	if (!g_isAltAsCtrl && keyCode != SC_LALT && keyCode != SC_TAB && keyCode != SC_LSHIFT) {
		return { g_nullKey };
	}

	return handleHotKeys(keyCode, isKeyDown, g_altShiftHotKeys);
}

Keys handleAltKey(unsigned short keyCode, bool isKeyDown) {
	if (!(g_isAltKeyDown || keyCode == SC_LALT)) {
		return {};
	}

	// alt + esc
	if (keyCode == SC_ESC) {
		return getParsedKeyDownUpKeys(
			isKeyDown,
			getTemplateKeysForEsc(),
			TemplateKeys({ g_nullKey })
		);
	}

	// alt + q
	if (keyCode == SC_Q) {
		if (!g_isAltAsCtrl) { // alt + tabbed + q
			return getParsedKeyDownUpKeys(
				isKeyDown,
				TemplateKeys({ Key(SC_DELETE) }),
				TemplateKeys({ g_nullKey })
			);
		}

		return getParsedKeyDownUpKeys(
			isKeyDown,
			TemplateKeys(_keyDownLAltAsLAlt, { Key(SC_F4) }, _keyDownLAltAsLCtrl),
			TemplateKeys({ g_nullKey })
		);
	}

	// Omit keys when in alt-tab mode
	if (!g_isAltAsCtrl && keyCode != SC_LALT && keyCode != SC_TAB) {
		return { g_nullKey };
	}

	return handleHotKeys(keyCode, isKeyDown, g_altHotKeys);
}

Keys handleKey(unsigned short keyCode, bool isKeyDown) {
	return handleHotKeys(keyCode, isKeyDown, g_keyHotKeys);
}

void resetHotKeys() {
	g_capsAltHotKeys = {};
	g_capsHotKeys = {};
	g_ctrlShiftHotKeys = {};
	g_ctrlHotKeys = {};
	g_winAltHotKeys = {};
	g_winShiftHotKeys = {};
	g_winHotKeys = {};
	g_altShiftHotKeys = {};
	g_altHotKeys = {};
	g_keyHotKeys = {};
}

namespace KeyEvent {
	bool isKeyDown(Key key) {
		return key.state == 0 || key.state == 2;
	}

	Keys getParsedKeysForEsc() {
		return getTemplateKeysForEsc().getParsedKeys();
	}

	Keys getKeyEvents(Keys keys) {
		Keys allKeys;
		int size = keys.size();

		for (int i = 0; i < size; i++) {
			Key key = keys[i];
			bool _isKeyDown = isKeyDown(key);
			Keys keys;
			int size = 0;
			auto keyCode = key.code;

			if (keyCode == SC_CAPSLOCK) {
				g_isCapslockKeyDown = _isKeyDown;
			} else if (keyCode == SC_LSHIFT) {
				g_isShiftKeyDown = _isKeyDown;
			} else if (keyCode == SC_LCTRL) {
				g_isCtrlKeyDown = _isKeyDown;
			} else if (keyCode == SC_LWIN) {
				g_isWinKeyDown = _isKeyDown;
			} else if (keyCode == SC_LALT) {
				g_isAltKeyDown = _isKeyDown;
			} else if (keyCode == SC_ESC) {
				g_isEscKeyDown = _isKeyDown;
			}

			if (keys = handleMouseClick(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleVimMode(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleCapslockAltKeys(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleCapslockKey(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleCtrlShiftKeys(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleCtrlKey(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleWinAltKeys(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleWinShiftKeys(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleWinKey(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleAltShiftKeys(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleAltKey(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleEscKey(keyCode, _isKeyDown), size = keys.size(), size != 0) {}
			else if (keys = handleKey(keyCode, _isKeyDown), size = keys.size(), size != 0) {}

			allKeys = concatKeyVectors(allKeys, keys);
		}

		return allKeys;
	}

	void setActiveProcessName(std::string _activeProcessName) {
		g_activeProcessName = _activeProcessName;

		OutputDebugStringA(String(g_activeProcessName + "\n").c_str());
	}

	void setCustomHotKeysFromFile(String customHotKeysFilePath, String coreHotKeysFilepath) {
		initialize();
		resetHotKeys();
		setHotKeysFromFile(customHotKeysFilePath);
		setHotKeysFromFile(coreHotKeysFilepath);
	}

	void initialize(String coreHotKeysFilepath) {
		g_isCapslockKeyDown = false;
		g_isShiftKeyDown = false;
		g_isCtrlKeyDown = false;
		g_isWinKeyDown = false;
		g_isAltKeyDown = false;
		g_isEscKeyDown = false;

		g_isCtrlAsAlt = true;
		g_isAltAsCtrl = true;

		g_isMouseClickDown = false;
		g_isVimShiftKeyDown = false;

		resetHotKeys();
		setHotKeysFromFile(coreHotKeysFilepath);
	}
}
