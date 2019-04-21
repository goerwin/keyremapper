#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <vector>
#include <tuple>
#include "KeyEvent.h"
#include "erwinUtils.h"

String g_activeProcessName;

bool g_isCapslockKeyDown;
bool g_isShiftKeyDown;
bool g_isLCtrlKeyDown;
bool g_isLWinKeyDown;
bool g_isLAltKeyDown;

bool g_isLCtrlAsLAlt;
bool g_isLAltAsLCtrl;

bool g_isMouseClickDown;
bool g_isVimShiftKeyDown;

std::pair<bool, Keys> g_userHotKeyHandler;

enum LAltLCtrl {
	_keyDownLAltAsLCtrl = 1,
	_keyDownLAltAsLAlt = 2,
	_keyUpLAlt = 3,
	_keyDownLCtrlAsLAlt = 4,
	_keyDownLCtrlAsLCtrl = 5,
	_keyUpLCtrl = 6,
	_null = 0
};

String getScanCodeSymbol(unsigned short code) {
	if (code == SC_C) { return "C"; }
	if (code == SC_D) { return "D"; }
	if (code == SC_E) { return "E"; }
	if (code == SC_H) { return "H"; }
	if (code == SC_I) { return "I"; }
	if (code == SC_J) { return "J"; }
	if (code == SC_K) { return "K"; }
	if (code == SC_L) { return "L"; }
	if (code == SC_M) { return "M"; }
	if (code == SC_P) { return "P"; }
	if (code == SC_Q) { return "Q"; }
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
	if (code == SC_MUTE) { return "MUTE"; }
	if (code == SC_VOLUMEDOWN) { return "VOLUMEDOWN"; }
	if (code == SC_VOLUMEUP) { return "VOLUMEUP"; }
	if (code == SC_ESC) { return "Esc"; }
	if (code == SC_CAPSLOCK) { return "Capslock"; }
	if (code == SC_LEFT) { return "Left"; }
	if (code == SC_RIGHT) { return "Right"; }
	if (code == SC_UP) { return "Up"; }
	if (code == SC_DOWN) { return "Down"; }
	if (code == SC_SPACE) { return "Space"; }
	if (code == SC_LWIN) { return "Win"; }
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
	if (code == SC_NP0) { return "NP0"; }
	if (code == SC_MINUS) { return "-"; }
	if (code == SC_GRAVE) { return "`"; }
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
	if (symbol == L"C") { return SC_C; }
	if (symbol == L"D") { return SC_D; }
	if (symbol == L"E") { return SC_E; }
	if (symbol == L"H") { return SC_H; }
	if (symbol == L"I") { return SC_I; }
	if (symbol == L"J") { return SC_J; }
	if (symbol == L"K") { return SC_K; }
	if (symbol == L"L") { return SC_L; }
	if (symbol == L"M") { return SC_M; }
	if (symbol == L"P") { return SC_P; }
	if (symbol == L"Q") { return SC_Q; }
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
	if (symbol == L"MUTE") { return SC_MUTE; }
	if (symbol == L"VOLUMEDOWN") { return SC_VOLUMEDOWN; }
	if (symbol == L"VOLUMEUP") { return SC_VOLUMEUP; }
	if (symbol == L"Esc") { return SC_ESC; }
	if (symbol == L"Capslock") { return SC_CAPSLOCK; }
	if (symbol == L"Left") { return SC_LEFT; }
	if (symbol == L"Right") { return SC_RIGHT; }
	if (symbol == L"Up") { return SC_UP; }
	if (symbol == L"Down") { return SC_DOWN; }
	if (symbol == L"Space") { return SC_SPACE; }
	if (symbol == L"Win") { return SC_LWIN; }
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
	if (symbol == L"NP0") { return SC_NP0; }
	if (symbol == L"-") { return SC_MINUS; }
	if (symbol == L"`") { return SC_GRAVE; }
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

	if (!g_isLCtrlAsLAlt && g_isLCtrlKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isLCtrlAsLAlt = true;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
Keys keyDownLCtrlAsLCtrl() {
	Keys keys;

	if (g_isLCtrlAsLAlt && g_isLCtrlKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	}

	g_isLCtrlAsLAlt = false;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
Keys keyUpLCtrl() {
	Keys keys;

	if (g_isLCtrlAsLAlt) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	} else {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isLCtrlAsLAlt = true;
	return keys;
}

Keys keyDownLAltAsLCtrl() {
	Keys keys;

	if (!g_isLAltAsLCtrl && g_isLAltKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	}

	g_isLAltAsLCtrl = true;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
Keys keyDownLAltAsLAlt() {
	Keys keys;

	if (g_isLAltAsLCtrl && g_isLAltKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isLAltAsLCtrl = false;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
Keys keyUpLAlt() {
	Keys keys;

	if (g_isLAltAsLCtrl) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	} else {
		keys.insert(keys.end(), { KeyUp(SC_LALT) });
	}

	g_isLAltAsLCtrl = true;
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

struct UserKeyInfo {
	ScanCodes code;
	String program;

	UserKeyInfo() {}

	UserKeyInfo(ScanCodes _code, String _program = "") {
		code = _code;
		program = _program;
	}
};

typedef std::tuple<UserKeyInfo, TemplateKeys, TemplateKeys> UserHotKey;
typedef std::vector<UserHotKey> UserHotKeys;

Key g_nullKey = KeyUp(SC_NULL);

TemplateKeys getTemplateKeys(std::wstring symbolKeys) {
	Keys keys;
	auto stateUpDown = ErwinUtils::utf82ws(String("↕"))[0];
	auto stateDown = ErwinUtils::utf82ws(String("↓"))[0];
	auto stateUp = ErwinUtils::utf82ws(String("↑"))[0];
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

UserHotKey getParsedUserHotkey(std::string userHotkeySymbol) {
	auto wuserHotkeySymbol = ErwinUtils::utf82ws(userHotkeySymbol);
	auto pos = wuserHotkeySymbol.find(L"'");
	auto pos2 = wuserHotkeySymbol.find(L"'", pos + 1);
	auto processName = wuserHotkeySymbol.substr(pos + 1, pos2 - pos - 1);

	pos = wuserHotkeySymbol.find(L" ", pos2);
	pos2 = wuserHotkeySymbol.find(L" ", pos + 1);
	// TODO: NOT DOING ANYTHING WITH THIS
	auto handler = wuserHotkeySymbol.substr(pos + 1, pos2 - pos - 1);

	pos = wuserHotkeySymbol.find(L" ", pos2);
	pos2 = wuserHotkeySymbol.find(L" ", pos + 1);
	auto keyInputSymbol = wuserHotkeySymbol.substr(pos + 1, pos2 - pos - 1);
	auto keyInputCode = getScanCode(keyInputSymbol);

	pos = wuserHotkeySymbol.find(L"Down(", pos2);
	pos2 = wuserHotkeySymbol.find(L")", pos + 1);
	auto keyDownSymbols = wuserHotkeySymbol.substr(pos + 5, pos2 - pos - 5);

	pos = wuserHotkeySymbol.find(L"Up(", pos2);
	pos2 = wuserHotkeySymbol.find(L")", pos + 1);
	auto keyUpSymbols = wuserHotkeySymbol.substr(pos + 3, pos2 - pos - 3);

	auto userKeyInfo = UserKeyInfo(keyInputCode, ErwinUtils::ws2utf8(processName));
	auto templateKeysDown = getTemplateKeys(keyDownSymbols);
	auto templateKeysUp = getTemplateKeys(keyUpSymbols);

	return UserHotKey(userKeyInfo, templateKeysDown, templateKeysUp);
}

UserHotKeys keyUserHotKeys = UserHotKeys({
	getParsedUserHotkey("'chrome.exe' Key F3 => Down(Ctrl↓ Shift↓ Tab↕ Shift↑ Ctrl↑) Up(null)"),
	getParsedUserHotkey("'chrome.exe' Key F4 => Down(Ctrl↓ Tab↕ Ctrl↑) Up(null)"),
	getParsedUserHotkey("'chrome.exe' Key F5 => Down(Alt↓ M↕ Alt↑) Up(null)"),
	getParsedUserHotkey("'chrome.exe' Key F6 => Down(Alt↓ T↕ Alt↑) Up(null)"),
	getParsedUserHotkey("'' Key F1 => Down(BRIGHTNESSDOWN↕) Up(null)"),
	getParsedUserHotkey("'' Key F2 => Down(BRIGHTNESSUP↕) Up(null)"),
	getParsedUserHotkey("'' Key F10 => Down(MUTE↕) Up(null)"),
  getParsedUserHotkey("'' Key F11 => Down(VOLUMEDOWN↕↕↕↕) Up(null)"),
  getParsedUserHotkey("'' Key F12 => Down(VOLUMEUP↕↕↕↕) Up(null)")
});
auto keyUserHotKeysSize = keyUserHotKeys.size();

UserHotKeys laltUserHotKeys = UserHotKeys({
	getParsedUserHotkey("'chrome.exe' LAlt Enter => Down(keyDownLAltAsLAlt Enter↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'chrome.exe' LAlt H => Down(Shift↓ Tab↕ Shift↑) Up(null)"),
	getParsedUserHotkey("'chrome.exe' LAlt L => Down(Tab↕) Up(null)"),
	getParsedUserHotkey("'chrome.exe' LAlt ; => Down(L↕) Up(null)"),
	getParsedUserHotkey("'SC2_x64.exe' LAlt F1 => Down(keyDownLAltAsLCtrl F1↕) Up(null)"),
	getParsedUserHotkey("'SC2_x64.exe' LAlt F2 => Down(keyDownLAltAsLCtrl F2↕) Up(null)"),
	getParsedUserHotkey("'SC2_x64.exe' LAlt F3 => Down(keyDownLAltAsLCtrl F3↕) Up(null)"),
	getParsedUserHotkey("'SC2_x64.exe' LAlt F4 => Down(keyDownLAltAsLCtrl F4↕) Up(null)"),
	getParsedUserHotkey("'slack.exe' LAlt P => Down(K↕) Up(null)"),
	getParsedUserHotkey("'mintty.exe' LAlt Back => Down(U↕) Up(null)"),
	getParsedUserHotkey("'mintty.exe' LAlt Z => Down(Shift↓ -↕ Shift↑) Up(null)"),
	getParsedUserHotkey("'mintty.exe' LAlt I => Down(L↕) Up(null)"),
	getParsedUserHotkey("'mintty.exe' LAlt C => Down(NP0↕) Up(null)"),
	getParsedUserHotkey("'mintty.exe' LAlt V => Down(keyUpLAlt Shift↓ NP0↕ Shift↑ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt Space => Down(F12↕) Up(null)"),
	getParsedUserHotkey("'' LAlt F1 => Down(keyUpLAlt F1↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt F2 => Down(keyUpLAlt F2↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt F1 => Down(keyUpLAlt F1↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt F10 => Down(keyUpLAlt F10↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt F11 => Down(keyUpLAlt F11↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt F12 => Down(keyUpLAlt F12↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt Tab => Down(keyDownLAltAsLAlt Tab↕) Up(null)"),
	getParsedUserHotkey("'' LAlt Back => Down(keyUpLAlt Shift↓ Home↕ Shift↑ Back↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt J => Down(keyUpLAlt Next↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt K => Down(keyUpLAlt Prior↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt ` => Down(keyDownLAltAsLAlt `↕ keyDownLAltAsLCtrl) Up(null)"),
	getParsedUserHotkey("'' LAlt Alt => Down(keyDownLAltAsLCtrl) Up(keyUpLAlt)"),
});
auto laltUserHotkeysSize = laltUserHotKeys.size();

UserHotKeys lwinUserHotKeys = UserHotKeys({
	getParsedUserHotkey("'SC2_x64.exe' Win 1 => Down(Alt↓ 1↕ Alt↑) Up(null)"),
	getParsedUserHotkey("'SC2_x64.exe' Win 2 => Down(Alt↓ 1↕ Alt↑) Up(null)"),
	getParsedUserHotkey("'SC2_x64.exe' Win 3 => Down(Alt↓ 1↕ Alt↑) Up(null)"),
	getParsedUserHotkey("'SC2_x64.exe' Win 4 => Down(Alt↓ 1↕ Alt↑) Up(null)"),
	getParsedUserHotkey("'mintty.exe' Win Back => Down(Ctrl↓ W↕ Ctrl↑) Up(null)"),
	getParsedUserHotkey("'' Win H => Down(Alt↓ Left↕ Alt↑) Up(null)"),
	getParsedUserHotkey("'' Win L => Down(Alt↓ Right↕ Alt↑) Up(null)"),
	getParsedUserHotkey("'' Win Back => Down(Ctrl↓ Back↕ Ctrl↑) Up(null)"),
	getParsedUserHotkey("'' Win D => Down(Win↓ D↕ Win↑) Up(null)"),
	getParsedUserHotkey("'' Win Space => Down(Win↕) Up(null)"),
	getParsedUserHotkey("'' Win Win => Down(null) Up(null)"),
});
auto lwinUserHotkeysSize = lwinUserHotKeys.size();

UserHotKeys ctrlUserHotKeys = UserHotKeys({
	getParsedUserHotkey("'' Ctrl Tab => Down(keyDownLCtrlAsLCtrl Tab↕) Up(null)"),
	getParsedUserHotkey("'' Ctrl Ctrl => Down(keyDownLCtrlAsLAlt) Up(keyUpLCtrl)"),
});
auto ctrlUserHotkeysSize = ctrlUserHotKeys.size();

bool isChromeActiveProcess() {
	return g_activeProcessName == "chrome.exe";
}
bool isStarcraft2ActiveProcess() {
	return g_activeProcessName == "SC2_x64.exe";
}
bool isSlackActiveProcess() {
	return g_activeProcessName == "slack.exe";
}
bool isGitBashActiveProcess() {
	return g_activeProcessName == "mintty.exe";
}

bool isKeyDown(Key key) {
	return key.state == 0 || key.state == 2;
}

/*std::wstring getStringKeyInfo(InterceptionKeyStroke keyStroke) {
	std::wstring pressedStatus = isKeyDown(keyStroke) ? L"down" : L"up";
	return std::to_wstring(keyStroke.code).append(L" ").append(pressedStatus);
}*/

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

bool isAFnKeyCode(unsigned short keyCode) {
	return keyCode == SC_F1 ||
		keyCode == SC_F2 ||
		keyCode == SC_F3 ||
		keyCode == SC_F4 ||
		keyCode == SC_F5 ||
		keyCode == SC_F6 ||
		keyCode == SC_F7 ||
		keyCode == SC_F8 ||
		keyCode == SC_F9 ||
		keyCode == SC_F10 ||
		keyCode == SC_F11 ||
		keyCode == SC_F12;
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

unsigned short getVimPriorNextKeyCode(unsigned short keyCode) {
	return (keyCode == SC_K || keyCode == SC_H) ? SC_PRIOR :
		(keyCode == SC_J || keyCode == SC_L) ? SC_NEXT :
		SC_NULL;
}

TemplateKeys getTemplateKeysForEsc() {
	if (!g_isLAltAsLCtrl) {
		return TemplateKeys({ Key(SC_ESC) }, _keyDownLAltAsLCtrl);
	}

	if (!g_isLAltKeyDown) {
		return TemplateKeys({ Key(SC_ESC) });
	}

	return TemplateKeys({ g_nullKey });
}

Keys getParsedKeysForEsc() {
	return getTemplateKeysForEsc().getParsedKeys();
}

std::pair<bool, Keys> handleUserHotKey(
	unsigned short keyCode,
	bool isKeyDown,
	UserHotKeys userHotKeys,
	int userHotkeysSize
) {
	if (userHotkeysSize == 0) {
		return { false, {} };
	}

	for (int i = 0; i < userHotkeysSize; i++) {
		auto userHotkey = userHotKeys[i];
		auto userKeyInfo = std::get<0>(userHotkey);

		if (keyCode == userKeyInfo.code) {
			auto program = userKeyInfo.program;

			if (program == "" || program == g_activeProcessName) {
				return {
					true,
					getParsedKeyDownUpKeys(
						isKeyDown,
						std::get<1>(userHotkey),
						std::get<2>(userHotkey)
					)
				};
			}
		}
	}

	return { false, {} };
}

Keys handleSimulateMouseClick(Key key) {
	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (
		!(g_isLWinKeyDown && keyCode == SC_C) &&
		!(g_isLWinKeyDown && keyCode == SC_LALT) &&
		!g_isMouseClickDown
		) {
		return {};
	}

	if (keyCode == SC_LALT) {
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(_keyDownLAltAsLCtrl),
			TemplateKeys(_keyUpLAlt)
		);
	}

	if (keyCode == SC_C) {
		if (g_isCapslockKeyDown) {
			if (isCurrentKeyDown && !g_isMouseClickDown) {
				g_isMouseClickDown = true;
				return { KeyDown(SC_MOUSERIGHT) };
			}

			if (!isCurrentKeyDown && g_isMouseClickDown) {
				g_isMouseClickDown = false;
				return { KeyUp(SC_MOUSERIGHT) };
			}

			return { g_nullKey };
		}

		if (isCurrentKeyDown && !g_isMouseClickDown) {
			g_isMouseClickDown = true;
			return { KeyDown(SC_MOUSELEFT) };
		}

		if (!isCurrentKeyDown && g_isMouseClickDown) {
			g_isMouseClickDown = false;
			return { KeyUp(SC_MOUSELEFT) };
		}

		return { g_nullKey };
	}
}

Keys handleLWinLAltKeys(Key key) {
	if (!(g_isLWinKeyDown && g_isLAltKeyDown)) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (keyCode == SC_LWIN) {
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ g_nullKey }),
			TemplateKeys(_keyDownLAltAsLCtrl)
		);
	}

	if (keyCode == SC_LALT) {
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ g_nullKey }),
			TemplateKeys(_keyUpLAlt)
		);
	}

	if (keyCode == SC_K || keyCode == SC_J || keyCode == SC_L || keyCode == SC_H) {
		auto arrowKeyCode = getVimArrowKeyCode(keyCode);

		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(
				_keyUpLAlt,
				{ KeyDown(SC_LWIN), Key(arrowKeyCode), KeyUp(SC_LWIN) }
			),
			TemplateKeys({ g_nullKey })
		);
	}

	return { g_nullKey };
}

Keys handleCapslockKey(Key key) {
	if (!g_isCapslockKeyDown) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (isGitBashActiveProcess()) {
		if (keyCode == SC_C) { // capslock + c : ctrl + c : kill process
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ KeyDown(SC_LCTRL), Key(SC_C), KeyUp(SC_LCTRL) }),
				TemplateKeys({ g_nullKey })
			);
		}
	}

	if (keyCode == SC_CAPSLOCK) {
		if (g_isVimShiftKeyDown) {
			g_isVimShiftKeyDown = false;

			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ g_nullKey }),
				TemplateKeys({ KeyUp(SC_LSHIFT) })
			);
		}

		return { g_nullKey };
	}

	if (keyCode == SC_LSHIFT || keyCode == SC_S) {
		g_isVimShiftKeyDown = isCurrentKeyDown;

		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ KeyDown(SC_LSHIFT) }),
			TemplateKeys({ KeyUp(SC_LSHIFT) })
		);
	}

	if (keyCode == SC_LALT) {
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(_keyDownLAltAsLCtrl),
			TemplateKeys(_keyUpLAlt)
		);
	}

	if (keyCode == SC_TAB || keyCode == SC_SPACE) {
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ KeyDown(SC_LCTRL), { Key(keyCode) }, KeyUp(SC_LCTRL) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_H || keyCode == SC_L || keyCode == SC_J || keyCode == SC_K) {
		if (g_isLWinKeyDown) {
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ KeyDown(SC_LCTRL), Key(getVimArrowKeyCode(keyCode)), KeyUp(SC_LCTRL) }),
				TemplateKeys({ g_nullKey })
			);
		}

		if (g_isLAltKeyDown && g_isLAltAsLCtrl && (keyCode == SC_H || keyCode == SC_L)) {
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys(_keyUpLAlt, { Key(getVimHomeEndKeyCode(keyCode)) }, _keyDownLAltAsLCtrl),
				TemplateKeys({ g_nullKey })
			);
		}

		if (g_isLAltKeyDown && g_isLAltAsLCtrl) {
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ Key(getVimHomeEndKeyCode(keyCode)) }),
				TemplateKeys({ g_nullKey })
			);
		}

		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ Key(getVimArrowKeyCode(keyCode)) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_V) {
		if (g_isLAltKeyDown) {
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys(
					_keyUpLAlt,
					{ KeyDown(SC_LWIN, 2), Key(SC_V), KeyUp(SC_LWIN, 3) },
					_keyDownLAltAsLCtrl
				),
				TemplateKeys({ g_nullKey })
			);
		}
		return { g_nullKey };
	}

	return { g_nullKey };
}

Keys handleLCtrlKey(Key key) {
	if (!g_isLCtrlKeyDown) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (g_userHotKeyHandler = handleUserHotKey(
		keyCode,
		isCurrentKeyDown,
		ctrlUserHotKeys,
		ctrlUserHotkeysSize
	), g_userHotKeyHandler.first) {
		return g_userHotKeyHandler.second;
	};

	return {};
}

Keys handleLWinKey(Key key) {
	if (!g_isLWinKeyDown) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (g_userHotKeyHandler = handleUserHotKey(
		keyCode,
		isCurrentKeyDown,
		lwinUserHotKeys,
		lwinUserHotkeysSize
	), g_userHotKeyHandler.first) {
		return g_userHotKeyHandler.second;
	};

	return {};
}

Keys handleLAltKey(Key key) {
	if (!g_isLAltKeyDown) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	// alt + esc
	if (keyCode == SC_ESC) {
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			getTemplateKeysForEsc(),
			TemplateKeys({ g_nullKey })
		);
	}

	// alt + q
	if (keyCode == SC_Q) {
		if (!g_isLAltAsLCtrl) { // alt + tabbed + q
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ Key(SC_SUPR) }),
				TemplateKeys({ g_nullKey })
			);
		}

		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(_keyDownLAltAsLAlt, { Key(SC_F4) }, _keyDownLAltAsLCtrl),
			TemplateKeys({ g_nullKey })
		);
	}

	// Omit keys when in alt-tab mode except Tab/Shift
	if (
		keyCode != SC_LSHIFT &&
		keyCode != SC_LALT &&
		keyCode != SC_TAB &&
		!g_isLAltAsLCtrl
	) {
		return { g_nullKey };
	}

	if (g_userHotKeyHandler = handleUserHotKey(
		keyCode,
		isCurrentKeyDown,
		laltUserHotKeys,
		laltUserHotkeysSize
	), g_userHotKeyHandler.first) {
		return g_userHotKeyHandler.second;
	};

	return {};
}

Keys handleKey(Key key) {
	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (g_userHotKeyHandler = handleUserHotKey(
		keyCode,
		isCurrentKeyDown,
		keyUserHotKeys,
		keyUserHotKeysSize
	), g_userHotKeyHandler.first) {
		return g_userHotKeyHandler.second;
	};

	return getParsedKeyDownUpKeys(
		isCurrentKeyDown,
		TemplateKeys({ KeyDown(keyCode) }),
		TemplateKeys({ KeyUp(keyCode) })
	);
}

Keys getKeyEvents(Keys keys) {
	int keysSize = keys.size();
	Keys allKeyEvents;

	for (int i = 0; i < keysSize; i++) {
		Key key = keys[i];
		bool isCurrentKeyDown = isKeyDown(key);
		Keys keyEvents;
		int keyEventsSize = 0;
		auto keyCode = key.code;

		if (isCurrentKeyDown) {
			if (keyCode == SC_CAPSLOCK) {
				g_isCapslockKeyDown = true;
			} else if (keyCode == SC_LSHIFT) {
				g_isShiftKeyDown = true;
			} else if (keyCode == SC_LCTRL) {
				g_isLCtrlKeyDown = true;
			} else if (keyCode == SC_LWIN) {
				g_isLWinKeyDown = true;
			} else if (keyCode == SC_LALT) {
				g_isLAltKeyDown = true;
			}
		}

		if (keyEvents = handleSimulateMouseClick(key), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleLWinLAltKeys(key), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleCapslockKey(key), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleLCtrlKey(key), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleLWinKey(key), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleLAltKey(key), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleKey(key), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}

		if (!isCurrentKeyDown) {
			if (keyCode == SC_CAPSLOCK) {
				g_isCapslockKeyDown = false;
			} else if (keyCode == SC_LSHIFT) {
				g_isShiftKeyDown = false;
			} else if (keyCode == SC_LCTRL) {
				g_isLCtrlKeyDown = false;
			} else if (keyCode == SC_LWIN) {
				g_isLWinKeyDown = false;
			} else if (keyCode == SC_LALT) {
				g_isLAltKeyDown = false;
			}
		}

		allKeyEvents = concatKeyVectors(allKeyEvents, keyEvents);
	}

	return allKeyEvents;
}

void setActiveProcessName(std::string _activeProcessName) {
	g_activeProcessName = _activeProcessName;
	OutputDebugStringA(g_activeProcessName.c_str());
}

void setGlobalDefaultValues() {
	g_isCapslockKeyDown = false;
	g_isShiftKeyDown = false;
	g_isLCtrlKeyDown = false;
	g_isLWinKeyDown = false;
	g_isLAltKeyDown = false;

	g_isLCtrlAsLAlt = true;
	g_isLAltAsLCtrl = true;

	g_isMouseClickDown = false;
	g_isVimShiftKeyDown = false;
	g_activeProcessName = "";
}
