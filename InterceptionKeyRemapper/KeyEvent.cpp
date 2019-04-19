#ifndef UNICODE
#define UNICODE
#endif

#include <vector>
#include "KeyEvent.h"

std::string g_activeProcessName;

bool g_isCapslockKeyDown;
bool g_isShiftKeyDown;
bool g_isLCtrlKeyDown;
bool g_isLWinKeyDown;
bool g_isLAltKeyDown;

bool g_isLCtrlAsLAlt;
bool g_isLAltAsLCtrl;

bool g_isMouseClickDown;
bool g_isVimShiftKeyDown;

Key g_nullKey = KeyUp(SC_NULL);

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

Keys concatKeyVectors(Keys keys, Keys keys2, Keys keys3 = {}, Keys keys4 = {}) {
	keys.insert(keys.end(), keys2.begin(), keys2.end());
	keys.insert(keys.end(), keys3.begin(), keys3.end());
	keys.insert(keys.end(), keys4.begin(), keys4.end());
	return keys;
}

bool isKeyDown(Key key) {
	return key.state == 0 || key.state == 2;
}

/*std::wstring getStringKeyInfo(InterceptionKeyStroke keyStroke) {
	std::wstring pressedStatus = isKeyDown(keyStroke) ? L"down" : L"up";
	return std::to_wstring(keyStroke.code).append(L" ").append(pressedStatus);
}*/

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

enum LAltLCtrl {
	_keyDownLAltAsLCtrl = 1,
	_keyDownLAltAsLAlt = 2,
	_keyUpLAlt = 3,
	_keyDownLCtrlAsLAlt = 4,
	_keyDownLCtrlAsLCtrl = 5,
	_keyUpLCtrl = 6,
	_null = 0
};

struct TemplateKeys {
	LAltLCtrl preLAltLCtrl = _null;
	LAltLCtrl posLAltLCtrl = _null;
	Keys keys;

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

Keys handleSimulateMouseClick(Key key) {
	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (
		!(g_isLWinKeyDown && keyCode == SC_C) &&
		//!(g_isLWinKeyDown && keyCode == SC_LSHIFT) &&
		!(g_isLWinKeyDown && keyCode == SC_LALT) &&
		!g_isMouseClickDown
		) {
		return {};
	}
	/*
	if (keyCode == SC_LSHIFT) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}*/

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
				{ KeyDown(SC_LWIN, 2), Key(arrowKeyCode), KeyUp(SC_LWIN, 3) }
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

	if (keyCode == SC_LCTRL) { // LCtrl
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(_keyDownLCtrlAsLAlt),
			TemplateKeys(_keyUpLCtrl)
		);
	}

	/*
	if (keyCode == SC_LSHIFT) { // LCtrl + Shift
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}
	*/

	if (keyCode == SC_TAB) { // LCtrl + Tab
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(_keyDownLCtrlAsLCtrl, { Key(SC_TAB) }),
			TemplateKeys({ g_nullKey })
		);
	}

	return {};
	/*
	if (isCurrentKeyDown) {
		return { KeyDown(keyCode) };
	}
	return { KeyUp(keyCode) };
	*/
}

Keys handleLWinKey(Key key) {
	if (!g_isLWinKeyDown) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (isStarcraft2ActiveProcess()) {
		if (keyCode == SC_1 || keyCode == SC_2 || keyCode == SC_3 || keyCode == SC_4) { // lwin + 1/2/3/4
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ KeyDown(SC_LALT), Key(keyCode), KeyUp(SC_LALT) }),
				TemplateKeys({ g_nullKey })
			);
		}
	}

	if (isGitBashActiveProcess()) {
		if (keyCode == SC_BACK) { // lwin + back : delete from cursor to beginning of word
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ KeyDown(SC_LCTRL), Key(SC_W), KeyUp(SC_LCTRL) }),
				TemplateKeys({ g_nullKey })
			);
		}
	}

	if (keyCode == SC_H || keyCode == SC_L) { // lwin + h/l
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ KeyDown(SC_LALT), Key(getVimArrowKeyCode(keyCode)), KeyUp(SC_LALT) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_BACK) { // LWin + back
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ KeyDown(SC_LCTRL), Key(SC_BACK), KeyUp(SC_LCTRL) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_D) { // LWin + D
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ KeyDown(SC_LWIN, 2), Key(SC_D), KeyUp(SC_LWIN, 3) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_SPACE) { // LWin + Space
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ Key(SC_LWIN, 5) }),
			TemplateKeys({ g_nullKey })
		);
	}

	return { g_nullKey };
}

Keys handleLAltKey(Key key) {
	if (!g_isLAltKeyDown) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (isChromeActiveProcess()) {
		if (keyCode == SC_RETURN) { // lalt + enter to alt + enter
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys(_keyDownLAltAsLAlt, { Key(SC_RETURN) }, _keyDownLAltAsLCtrl),
				TemplateKeys({ g_nullKey })
			);
		}

		if (keyCode == SC_H) { // alt + h
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ KeyDown(SC_LSHIFT), Key(SC_TAB), KeyUp(SC_LSHIFT) }),
				TemplateKeys({ g_nullKey })
			);
		}

		if (keyCode == SC_L) { // alt + l
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ Key(SC_TAB) }),
				TemplateKeys({ g_nullKey })
			);
		}

		if (keyCode == SC_SEMI) { // alt + ;
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ Key(SC_L) }),
				TemplateKeys({ g_nullKey })
			);
		}
	}

	if (isStarcraft2ActiveProcess()) {
		if (keyCode == SC_F1 || keyCode == SC_F2 || keyCode == SC_F3 || keyCode == SC_F4) {
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys(_keyDownLAltAsLCtrl, { Key(keyCode) }),
				TemplateKeys({ g_nullKey })
			);
		}
	}

	if (isSlackActiveProcess()) {
		if (keyCode == SC_P) {
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ Key(SC_K) }),
				TemplateKeys({ g_nullKey })
			);
		}
	}

	if (isGitBashActiveProcess()) {
		// TODO: lalt + a : select all

		if (keyCode == SC_V) { // lalt + v : paste
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys(
					_keyUpLAlt,
					{ KeyDown(SC_LSHIFT), Key(SC_NP0), KeyUp(SC_LSHIFT) },
					_keyDownLAltAsLCtrl
				),
				TemplateKeys({ g_nullKey })
			);
		}

		if (keyCode == SC_Z) { // lalt + z : undo
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ KeyDown(SC_LSHIFT), Key(SC_MINUS), KeyUp(SC_LSHIFT) }),
				TemplateKeys({ g_nullKey })
			);
		}

		if (keyCode == SC_C) { // lalt + c : copy
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ Key(SC_NP0) }),
				TemplateKeys({ g_nullKey })
			);
		}

		if (keyCode == SC_BACK) { // lalt + back : delete from cursor to beginning of line
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ Key(SC_U) }),
				TemplateKeys({ g_nullKey })
			);
		}

		if (keyCode == SC_I) { // lalt + i : Clear screen
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ Key(SC_L) }),
				TemplateKeys({ g_nullKey })
			);
		}
	}

	if (keyCode == SC_LALT) {
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(_keyDownLAltAsLCtrl),
			TemplateKeys(_keyUpLAlt)
		);
	}

	/*
	if (keyCode == SC_LSHIFT) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}*/

	if (keyCode == SC_GRAVE) { // lalt + ` to alt + `
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(_keyDownLAltAsLAlt, { Key(SC_GRAVE) }, _keyDownLAltAsLCtrl),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_ESC) { // alt + esc
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			getTemplateKeysForEsc(),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_Q) { // alt + q
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

	if (keyCode == SC_TAB) { // alt + tab
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(_keyDownLAltAsLAlt, { Key(SC_TAB) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_BACK) { // alt + back
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(
				_keyUpLAlt,
				{ KeyDown(SC_LSHIFT), Key(SC_HOME), KeyUp(SC_LSHIFT), Key(SC_BACK) },
				_keyDownLAltAsLCtrl
			),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_J || keyCode == SC_K) { // alt + j/k
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(
				_keyUpLAlt,
				{ Key(keyCode == SC_J ? SC_NEXT : SC_PRIOR) },
				_keyDownLAltAsLCtrl
			),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_SPACE) { // alt + espace
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ Key(SC_F12) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (isAFnKeyCode(keyCode)) { // alt + f{n} - NOTE: it's not possible to send ctrl + (shift) + f{n}
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys(_keyUpLAlt, { Key(keyCode) }, _keyDownLAltAsLCtrl),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode != SC_LSHIFT && !g_isLAltAsLCtrl) { // alttabbed + letter
		return { g_nullKey };
	}

	return {};
	/*if (isCurrentKeyDown) {
		return { KeyDown(keyCode) };
	}
	return { KeyUp(keyCode) };*/
}

/*Keys handleShiftKey(Key key) {
	if (!g_isShiftKeyDown) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (keyCode == SC_LSHIFT) {
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ KeyDown(SC_LSHIFT) }),
			TemplateKeys({ KeyUp(SC_LSHIFT) })
		);
	}

	return getParsedKeyDownUpKeys(
		isCurrentKeyDown,
		TemplateKeys({ KeyDown(keyCode) }),
		TemplateKeys({ KeyUp(keyCode) })
	);
}*/

Keys handleKey(Key key) {
	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (isChromeActiveProcess()) {
		if (keyCode == SC_F3) { // f3
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ KeyDown(SC_LCTRL), KeyDown(SC_LSHIFT), Key(SC_TAB), KeyUp(SC_LSHIFT), KeyUp(SC_LCTRL) }),
				TemplateKeys({ g_nullKey })
			);
		}

		if (keyCode == SC_F4) { // f4
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ KeyDown(SC_LCTRL), Key(SC_TAB), KeyUp(SC_LCTRL) }),
				TemplateKeys({ g_nullKey })
			);
		}

		if (keyCode == SC_F5 || keyCode == SC_F6) { // f5/f6
			return getParsedKeyDownUpKeys(
				isCurrentKeyDown,
				TemplateKeys({ KeyDown(SC_LALT), Key(keyCode == SC_F5 ? SC_M : SC_T), KeyUp(SC_LALT) }),
				TemplateKeys({ g_nullKey })
			);
		}
	}

	if (keyCode == SC_F1 || keyCode == SC_F2) { // f1/f2
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ KeyDown(keyCode == SC_F1 ? SC_BRIGHTNESSDOWN : SC_BRIGHTNESSUP) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_F10) { // f10
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ Key(SC_MUTE, 5) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_F11) { // f11
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ Key(SC_VOLUMEDOWN, 5), Key(SC_VOLUMEDOWN, 5), Key(SC_VOLUMEDOWN, 5), Key(SC_VOLUMEDOWN, 5) }),
			TemplateKeys({ g_nullKey })
		);
	}

	if (keyCode == SC_F12) { // f12
		return getParsedKeyDownUpKeys(
			isCurrentKeyDown,
			TemplateKeys({ Key(SC_VOLUMEUP, 5), Key(SC_VOLUMEUP, 5), Key(SC_VOLUMEUP, 5), Key(SC_VOLUMEUP, 5) }),
			TemplateKeys({ g_nullKey })
		);
	}

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
		// else if (keyEvents = handleShiftKey(key), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
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
