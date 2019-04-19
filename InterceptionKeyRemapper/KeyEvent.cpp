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

Keys concatKeyVectors(
	Keys keys,
	Keys keys2,
	Keys keys3,
	Keys keys4
) {
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

Keys concatKeysWithLAltLCtrl(
	std::string pre,
	Keys keys,
	std::string pos = ""
) {
	if (pre == "keyDownLAltAsLCtrl") {
		keys = concatKeyVectors(keyDownLAltAsLCtrl(), keys);
	} else if (pre == "keyDownLAltAsLAlt") {
		keys = concatKeyVectors(keyDownLAltAsLAlt(), keys);
	} else if (pre == "keyUpLAlt") {
		keys = concatKeyVectors(keyUpLAlt() , keys);
	} else if (pre == "keyDownLCtrlAsLAlt") {
		keys = concatKeyVectors(keyDownLCtrlAsLAlt(), keys);
	} else if (pre == "keyDownLCtrlAsLCtrl") {
		keys = concatKeyVectors(keyDownLCtrlAsLCtrl(), keys);
	} else if (pre == "keyUpLCtrl") {
		keys = concatKeyVectors(keyUpLCtrl() , keys);
	}

	if (pos == "keyDownLAltAsLCtrl") {
		keys = concatKeyVectors(keys, keyDownLAltAsLCtrl());
	} else if (pos == "keyDownLAltAsLAlt") {
		keys = concatKeyVectors(keys, keyDownLAltAsLAlt());
	} else if (pos == "keyUpLAlt") {
		keys = concatKeyVectors(keys, keyUpLAlt());
	} else if (pos == "keyDownLCtrlAsLAlt") {
		keys = concatKeyVectors(keys, keyDownLCtrlAsLAlt());
	} else if (pos == "keyDownLCtrlAsLCtrl") {
		keys = concatKeyVectors(keys, keyDownLCtrlAsLCtrl());
	} else if (pos == "keyUpLCtrl") {
		keys = concatKeyVectors(keys, keyUpLCtrl());
	}

	return keys;
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

Keys handleSimulateMouseClick(Key key) {
	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (
		!(g_isLWinKeyDown && keyCode == SC_C) &&
		!(g_isLWinKeyDown && keyCode == SC_LSHIFT) &&
		!(g_isLWinKeyDown && keyCode == SC_LALT) &&
		!g_isMouseClickDown
		) {
		return {};
	}

	if (keyCode == SC_LSHIFT) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}

	if (keyCode == SC_LALT) {
		if (isCurrentKeyDown) {
			return keyDownLAltAsLCtrl();
		}
		return keyUpLAlt();
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
		return { g_nullKey };
	}

	if (keyCode == SC_LALT) {
		if (isCurrentKeyDown) {
			return { g_nullKey };
		}
		return keyUpLAlt();
	}

	if (keyCode == SC_K || keyCode == SC_J || keyCode == SC_L || keyCode == SC_H) {
		auto arrowKeyCode = getVimArrowKeyCode(keyCode);

		if (isCurrentKeyDown) {
			return concatKeysWithLAltLCtrl(
				"keyUpLAlt",
				{ KeyDown(SC_LWIN, 2), Key(arrowKeyCode), KeyUp(SC_LWIN, 3) }
			);
		}
		return { g_nullKey };
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
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), Key(SC_C), KeyUp(SC_LCTRL) };
			}
			return { g_nullKey };
		}
	}

	if (keyCode == SC_CAPSLOCK) {
		if (g_isVimShiftKeyDown) {
			g_isVimShiftKeyDown = false;

			if (isCurrentKeyDown) {
				return { g_nullKey };
			}
			return { KeyUp(SC_LSHIFT) };
		}

		return { g_nullKey };
	}

	if (keyCode == SC_LSHIFT || keyCode == SC_S) {
		g_isVimShiftKeyDown = isCurrentKeyDown;

		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}

	if (keyCode == SC_LALT) {
		if (isCurrentKeyDown) {
			return keyDownLAltAsLCtrl();
		}
		return keyUpLAlt();
	}

	if (keyCode == SC_TAB || keyCode == SC_SPACE) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LCTRL), { Key(keyCode) }, KeyUp(SC_LCTRL) };
		}
		return { g_nullKey };
	}

	if (keyCode == SC_H || keyCode == SC_L || keyCode == SC_J || keyCode == SC_K) {
		auto arrowKeyCode = getVimArrowKeyCode(keyCode);
		auto homeEndKeyCode = getVimHomeEndKeyCode(keyCode);

		if (g_isLWinKeyDown) {
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), Key(arrowKeyCode), KeyUp(SC_LCTRL) };
			}
			return { g_nullKey };
		}

		if (g_isLAltKeyDown && g_isLAltAsLCtrl && (keyCode == SC_H || keyCode == SC_L)) {
			if (isCurrentKeyDown) {
				return concatKeysWithLAltLCtrl("keyUpLAlt", { Key(homeEndKeyCode) }, "keyDownLAltAsLCtrl");
			}
			return { g_nullKey };
		}

		if (g_isLAltKeyDown && g_isLAltAsLCtrl) {
			if (isCurrentKeyDown) {
				return { Key(homeEndKeyCode) };
			}
			return { g_nullKey };
		}

		if (isCurrentKeyDown) {
			return { Key(arrowKeyCode) };
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
		if (isCurrentKeyDown) {
			return keyDownLCtrlAsLAlt();
		}
		return keyUpLCtrl();
	}

	if (keyCode == SC_LSHIFT) { // LCtrl + Shift
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}

	if (keyCode == SC_TAB) { // LCtrl + Tab
		if (isCurrentKeyDown) {
			return concatKeysWithLAltLCtrl("keyDownLCtrlAsLCtrl", { Key(SC_TAB) });
		}
		return { g_nullKey };
	}

	if (isCurrentKeyDown) {
		return { KeyDown(keyCode) };
	}
	return { KeyUp(keyCode) };
}

Keys handleLWinKey(Key key) {
	if (!g_isLWinKeyDown) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (isStarcraft2ActiveProcess()) {
		if (keyCode == SC_1 || keyCode == SC_2 || keyCode == SC_3 || keyCode == SC_4) { // lwin + 1/2/3/4
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), Key(keyCode), KeyUp(SC_LALT) };
			}
			return { g_nullKey };
		}
	}

	if (isGitBashActiveProcess()) {
		if (keyCode == SC_BACK) { // lwin + back : delete from cursor to beginning of word
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), Key(SC_W), KeyUp(SC_LCTRL) };
			}
			return { g_nullKey };
		}
	}

	if (keyCode == SC_H || keyCode == SC_L) { // lwin + h/l
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LALT), Key(getVimArrowKeyCode(keyCode)), KeyUp(SC_LALT) };
		}
		return { g_nullKey };
	}

	if (keyCode == SC_BACK) { // LWin + back
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LCTRL), Key(SC_BACK), KeyUp(SC_LCTRL) };
		}
		return { g_nullKey };
	}

	if (keyCode == SC_D) { // LWin + D
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LWIN, 2), Key(SC_D), KeyUp(SC_LWIN, 3) };
		}
		return { g_nullKey };
	}

	if (keyCode == SC_SPACE) { // LWin + Space
		if (isCurrentKeyDown) {
			return { Key(SC_LWIN, 5) };
		}
		return { g_nullKey };
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
			if (isCurrentKeyDown) {
				return concatKeysWithLAltLCtrl("keyDownLAltAsLAlt", { Key(SC_RETURN) }, "keyDownLAltAsLCtrl");
			}
			return { g_nullKey };
		}

		if (keyCode == SC_H) { // alt + h
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LSHIFT), Key(SC_TAB), KeyUp(SC_LSHIFT) };
			}
			return { g_nullKey };
		}

		if (keyCode == SC_L) { // alt + l
			if (isCurrentKeyDown) {
				return { Key(SC_TAB) };
			}
			return { g_nullKey };
		}

		if (keyCode == SC_SEMI) { // alt + ;
			if (isCurrentKeyDown) {
				return { Key(SC_L) };
			}
			return { g_nullKey };
		}
	}

	if (isStarcraft2ActiveProcess()) {
		if (keyCode == SC_F1 || keyCode == SC_F2 || keyCode == SC_F3 || keyCode == SC_F4) {
			if (isCurrentKeyDown) {
				return concatKeysWithLAltLCtrl("keyDownLAltAsLCtrl", { Key(keyCode) });
			}
			return { g_nullKey };
		}
	}

	if (isSlackActiveProcess()) {
		if (keyCode == SC_P) {
			if (isCurrentKeyDown) {
				return { Key(SC_K) };
			}
			return { g_nullKey };
		}
	}

	if (isGitBashActiveProcess()) {
		// TODO: lalt + a : select all
		if (keyCode == SC_C) { // lalt + c : copy
			if (isCurrentKeyDown) {
				return { Key(SC_NP0) };
			}
			return { g_nullKey };
		}

		if (keyCode == SC_V) { // lalt + v : paste
			if (isCurrentKeyDown) {
				return concatKeysWithLAltLCtrl(
					"keyUpLAlt",
					{ KeyDown(SC_LSHIFT), Key(SC_NP0), KeyUp(SC_LSHIFT) },
					"keyDownLAltAsLCtrl"
				);
			}
			return { g_nullKey };
		}

		if (keyCode == SC_Z) { // lalt + z : undo
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LSHIFT), Key(SC_MINUS), KeyUp(SC_LSHIFT) };
			}
			return { g_nullKey };
		}

		if (keyCode == SC_BACK) { // lalt + back : delete from cursor to beginning of line
			if (isCurrentKeyDown) {
				return { Key(SC_U) };
			}
			return { g_nullKey };
		}

		if (keyCode == SC_I) { // lalt + i : Clear screen
			if (isCurrentKeyDown) {
				return { Key(SC_L) };
			}
			return { g_nullKey };
		}
	}

	if (keyCode == SC_LALT) { // lalt
		if (isCurrentKeyDown) {
			return keyDownLAltAsLCtrl();
		}
		return keyUpLAlt();
	}

	if (keyCode == SC_LSHIFT) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}

	if (keyCode == SC_GRAVE) { // lalt + ` to alt + `
		if (isCurrentKeyDown) {
			return concatKeysWithLAltLCtrl("keyDownLAltAsLAlt", { Key(SC_GRAVE) }, "keyDownLAltAsLCtrl");
		}
		return { g_nullKey };
	}

	if (keyCode == SC_ESC) { // alt + esc
		if (!g_isLAltAsLCtrl) { // alt + tabbed + esc
			if (isCurrentKeyDown) {
				return { Key(SC_ESC) };
			}
			return keyDownLAltAsLCtrl();
		}

		return { g_nullKey };
	}

	if (keyCode == SC_Q) { // alt + q
		if (!g_isLAltAsLCtrl) { // alt + tabbed + q
			if (isCurrentKeyDown) {
				return { Key(SC_SUPR) };
			}
			return { g_nullKey };
		}

		if (isCurrentKeyDown) {
			return concatKeysWithLAltLCtrl("keyDownLAltAsLAlt", { Key(SC_F4) }, "keyDownLAltAsLCtrl");
		}
		return { g_nullKey };
	}

	if (keyCode == SC_TAB) { // alt + tab
		if (isCurrentKeyDown) {
			return concatKeysWithLAltLCtrl("keyDownLAltAsLAlt", { Key(SC_TAB) });
		}
		return { g_nullKey };
	}

	if (keyCode == SC_BACK) { // alt + back
		if (isCurrentKeyDown) {
				return concatKeysWithLAltLCtrl(
					"keyUpLAlt",
					{ KeyDown(SC_LSHIFT), Key(SC_HOME), KeyUp(SC_LSHIFT), Key(SC_BACK) },
					"keyDownLAltAsLCtrl"
				);
		}
		return { g_nullKey };
	}

	if (keyCode == SC_J || keyCode == SC_K) { // alt + j/k
		if (isCurrentKeyDown) {
				return concatKeysWithLAltLCtrl(
					"keyUpLAlt",
					{ Key(getVimPriorNextKeyCode(keyCode)) },
					"keyDownLAltAsLCtrl"
				);
		}
		return { g_nullKey };
	}

	if (keyCode == SC_SPACE) { // alt + espace
		if (isCurrentKeyDown) {
				return { Key(SC_F12) };
		}
		return { g_nullKey };
	}

	if (isAFnKeyCode(keyCode)) { // alt + f{n} - NOTE: it's not possible to send ctrl + (shift) + f{n}
		if (isCurrentKeyDown) {
				return concatKeysWithLAltLCtrl("keyUpLAlt", { Key(keyCode) });
		}
		return { g_nullKey };
	}

	if (!g_isLAltAsLCtrl) { // alttabbed + letter
		return { g_nullKey };
	}

	if (isCurrentKeyDown) {
		return { KeyDown(keyCode) };
	}
	return { KeyUp(keyCode) };
}

Keys handleShiftKey(Key key) {
	if (!g_isShiftKeyDown) {
		return {};
	}

	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (keyCode == SC_LSHIFT) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}

	if (isCurrentKeyDown) {
		return { KeyDown(keyCode) };
	}
	return { KeyUp(keyCode) };
}

Keys handleKey(Key key) {
	auto isCurrentKeyDown = isKeyDown(key);
	auto keyCode = key.code;

	if (isChromeActiveProcess()) {
		if (keyCode == SC_F3) { // f3
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), KeyDown(SC_LSHIFT), Key(SC_TAB), KeyUp(SC_LSHIFT), KeyUp(SC_LCTRL) };
			}
			return{ g_nullKey };
		}

		if (keyCode == SC_F4) { // f4
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), Key(SC_TAB), KeyUp(SC_LCTRL) };
			}
			return{ g_nullKey };
		}

		if (keyCode == SC_F5) { // F5
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), Key(SC_M), KeyUp(SC_LALT) };
			}
			return{ g_nullKey };
		}

		if (keyCode == SC_F6) { // f6
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), Key(SC_T), KeyUp(SC_LALT) };
			}
			return{ g_nullKey };
		}
	}

	if (keyCode == SC_F1) { // f1
		if (isCurrentKeyDown) {
			return { KeyDown(SC_BRIGHTNESSDOWN) };
		}
		return { g_nullKey };
	}

	if (keyCode == SC_F2) { // f2
		if (isCurrentKeyDown) {
			return { KeyDown(SC_BRIGHTNESSUP) };
		}
		return { g_nullKey };
	}

	if (keyCode == SC_F10) { // f10
		if (isCurrentKeyDown) {
			return { Key(SC_MUTE, 5) };
		}
		return { g_nullKey };
	}

	if (keyCode == SC_F11) { // f11
		if (isCurrentKeyDown) {
			return{ Key(SC_VOLUMEDOWN, 5), Key(SC_VOLUMEDOWN, 5), Key(SC_VOLUMEDOWN, 5), Key(SC_VOLUMEDOWN, 5) };
		}
		return { g_nullKey };
	}

	if (keyCode == SC_F12) { // f12
		if (isCurrentKeyDown) {
			return { Key(SC_VOLUMEUP, 5), Key(SC_VOLUMEUP, 5), Key(SC_VOLUMEUP, 5), Key(SC_VOLUMEUP, 5) };
		}
		return { g_nullKey };
	}

	if (isCurrentKeyDown) {
		return { KeyDown(keyCode) };
	}
	return { KeyUp(keyCode) };
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
		else if (keyEvents = handleShiftKey(key), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
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
