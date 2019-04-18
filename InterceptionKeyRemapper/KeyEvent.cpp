#ifndef UNICODE
#define UNICODE
#endif

#include <vector>
#include "interception.h"
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

Key g_nullKeyEvent = KeyUp(SC_NULL);

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

std::vector<Key> concatKeyVectors(
	std::vector<Key> keys,
	std::vector<Key> keys2,
	std::vector<Key> keys3,
	std::vector<Key> keys4
) {
	keys.insert(keys.end(), keys2.begin(), keys2.end());
	keys.insert(keys.end(), keys3.begin(), keys3.end());
	keys.insert(keys.end(), keys4.begin(), keys4.end());
	return keys;
}

void commonKeyRemaps(InterceptionKeyStroke &keyStroke) {
	DWORD keyCode = keyStroke.code;
	if (keyCode == SC_LBSLASH || keyCode == SC_RSHIFT) {
		keyStroke.code = SC_LSHIFT;
	} else if (keyCode == SC_RALT) {
		keyStroke.code = SC_LALT;
	} else if (keyCode == SC_RCTRL) {
		keyStroke.code = SC_LCTRL;
	}
}

bool isKeyDown(InterceptionKeyStroke keyStroke) {
	return keyStroke.state == 0 || keyStroke.state == 2;
}

/*std::wstring getStringKeyInfo(InterceptionKeyStroke keyStroke) {
	std::wstring pressedStatus = isKeyDown(keyStroke) ? L"down" : L"up";
	return std::to_wstring(keyStroke.code).append(L" ").append(pressedStatus);
}*/

std::vector<Key> keyDownLCtrlAsLAlt() {
	std::vector<Key> keys;

	if (!g_isLCtrlAsLAlt && g_isLCtrlKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isLCtrlAsLAlt = true;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
std::vector<Key> keyDownLCtrlAsLCtrl() {
	std::vector<Key> keys;

	if (g_isLCtrlAsLAlt && g_isLCtrlKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	}

	g_isLCtrlAsLAlt = false;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
std::vector<Key> keyUpLCtrl() {
	std::vector<Key> keys;

	if (g_isLCtrlAsLAlt) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	} else {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isLCtrlAsLAlt = true;
	return keys;
}

std::vector<Key> keyDownLAltAsLCtrl() {
	std::vector<Key> keys;

	if (!g_isLAltAsLCtrl && g_isLAltKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	}

	g_isLAltAsLCtrl = true;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
std::vector<Key> keyDownLAltAsLAlt() {
	std::vector<Key> keys;

	if (g_isLAltAsLCtrl && g_isLAltKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	g_isLAltAsLCtrl = false;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
std::vector<Key> keyUpLAlt() {
	std::vector<Key> keys;

	if (g_isLAltAsLCtrl) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	} else {
		keys.insert(keys.end(), { KeyUp(SC_LALT) });
	}

	g_isLAltAsLCtrl = true;
	return keys;
}

std::vector<Key> handleSimulateMouseClick(InterceptionKeyStroke keyStroke) {
	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

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
			if (isCurrentKeyDown) {
				if (!g_isMouseClickDown) {
					g_isMouseClickDown = true;
					return { KeyDown(SC_MOUSERIGHT) };
				}
				return { g_nullKeyEvent };
			}
			g_isMouseClickDown = false;
			return { KeyUp(SC_MOUSERIGHT) };
		}

		if (isCurrentKeyDown) {
			if (!g_isMouseClickDown) {
				g_isMouseClickDown = true;
				return { KeyDown(SC_MOUSELEFT) };
			}
			return { g_nullKeyEvent };
		}
		g_isMouseClickDown = false;
		return { KeyUp(SC_MOUSELEFT) };
	}

	return {};
}

std::vector<Key> handleLWinLAltKeys(InterceptionKeyStroke keyStroke) {
	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (!(g_isLWinKeyDown && g_isLAltKeyDown)) {
		return {};
	}

	if (keyCode == SC_K) { // win + alt + k
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), {
				KeyDown(SC_LWIN, 2),
				KeyDown(SC_UP),
				KeyUp(SC_UP),
				KeyUp(SC_LWIN, 3)
				});
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_J) { // win + alt + j
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), {
				KeyDown(SC_LWIN, 2),
				KeyDown(SC_DOWN),
				KeyUp(SC_DOWN),
				KeyUp(SC_LWIN, 3)
				});
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_L) { // win + alt + l
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LWIN, 2), KeyDown(SC_RIGHT), KeyUp(SC_RIGHT), KeyUp(SC_LWIN, 3) });
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_H) { // win + alt + h
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LWIN, 2), KeyDown(SC_LEFT), KeyUp(SC_LEFT), KeyUp(SC_LWIN, 3) });
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_LALT) {
		if (isCurrentKeyDown) {
			return { g_nullKeyEvent };
		}
		return keyUpLAlt();
	}

	if (keyCode == SC_LCTRL) {
		if (isCurrentKeyDown) {
			return { g_nullKeyEvent };
		}
		return keyDownLAltAsLCtrl();
	}

	return { g_nullKeyEvent };
}

std::vector<Key> handleCapslockKey(InterceptionKeyStroke keyStroke) {
	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD currentKeyCode = keyStroke.code;

	if (!g_isCapslockKeyDown) {
		return {};
	}

	if (isGitBashActiveProcess()) {
		if (currentKeyCode == SC_C && !g_isLAltKeyDown) { // capslock + c : ctrl + c : kill process
			if (isCurrentKeyDown) {
				return concatKeyVectors(
					keyDownLAltAsLCtrl(),
					{ KeyDown(SC_C), KeyUp(SC_C) },
					keyUpLAlt()
				);
			}
			return { g_nullKeyEvent };
		}
	}

	if (currentKeyCode == SC_CAPSLOCK || currentKeyCode == SC_LSHIFT || currentKeyCode == SC_S) {
		if (isCurrentKeyDown) {
			if (currentKeyCode == SC_LSHIFT || currentKeyCode == SC_S) {
				g_isVimShiftKeyDown = true;
				return { KeyDown(SC_LSHIFT) };
			}
			return { g_nullKeyEvent };
		}

		if (g_isVimShiftKeyDown) {
			g_isVimShiftKeyDown = false;
			return { KeyUp(SC_LSHIFT) };
		}
		return { g_nullKeyEvent };
	}

	if (currentKeyCode == SC_LALT) {
		if (isCurrentKeyDown) {
			return { g_nullKeyEvent };
		}
		return keyUpLAlt();
	}

	if (currentKeyCode == SC_TAB) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LCTRL), KeyDown(SC_TAB), KeyUp(SC_TAB), KeyUp(SC_LCTRL) };
		}
		return { g_nullKeyEvent };
	}

	if (currentKeyCode == SC_SPACE) {
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLAltAsLCtrl(), { KeyDown(currentKeyCode), KeyUp(currentKeyCode) }, keyUpLAlt());
		}
		return { g_nullKeyEvent };
	}

	if (currentKeyCode == SC_H) {
		if (g_isLWinKeyDown) {
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), KeyDown(SC_LEFT), KeyUp(SC_LEFT), KeyUp(SC_LCTRL) };
			}
			return { g_nullKeyEvent };
		}

		if (g_isLAltKeyDown && g_isLAltAsLCtrl) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_HOME), KeyUp(SC_HOME) }, keyDownLAltAsLCtrl());
			}
			return { g_nullKeyEvent };
		}

		if (isCurrentKeyDown) {
			return { KeyDown(SC_LEFT), KeyUp(SC_LEFT) };
		}
		return { g_nullKeyEvent };
	}

	if (currentKeyCode == SC_L) {
		if (g_isLWinKeyDown) {
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), KeyDown(SC_RIGHT), KeyUp(SC_RIGHT), KeyUp(SC_LCTRL) };
			}
			return { g_nullKeyEvent };
		}

		if (g_isLAltKeyDown && g_isLAltAsLCtrl) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_END), KeyUp(SC_END) }, keyDownLAltAsLCtrl());
			}
			return { g_nullKeyEvent };
		}

		if (isCurrentKeyDown) {
			return { KeyDown(SC_RIGHT), KeyUp(SC_RIGHT) };
		}
		return { g_nullKeyEvent };
	}

	if (currentKeyCode == SC_J) {
		if (g_isLAltKeyDown && g_isLAltAsLCtrl) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LCTRL), KeyDown(SC_END), KeyUp(SC_END), KeyUp(SC_LCTRL) }, keyDownLAltAsLCtrl());
			}
			return { g_nullKeyEvent };
		}

		if (isCurrentKeyDown) {
			// Here you MAY in the Alt+Tab switcher
			return { KeyDown(SC_DOWN), KeyUp(SC_DOWN) };
		}
		return { g_nullKeyEvent };
	}

	if (currentKeyCode == SC_K) {
		if (g_isLAltKeyDown && g_isLAltAsLCtrl) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LCTRL), KeyDown(SC_HOME), KeyUp(SC_HOME), KeyUp(SC_LCTRL) }, keyDownLAltAsLCtrl());
			}
			return { g_nullKeyEvent };
		}

		if (isCurrentKeyDown) {
			// Here you MAY in the Alt+Tab switcher
			return { KeyDown(SC_UP), KeyUp(SC_UP) };
		}
		return { g_nullKeyEvent };
	}

	if (currentKeyCode == SC_V) {
		if (g_isLAltKeyDown && g_isLAltAsLCtrl) {
			if (isCurrentKeyDown) { // caps + lalt + V
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LWIN, 2), KeyDown(SC_V), KeyUp(SC_V), KeyUp(SC_LWIN, 3) }, keyDownLAltAsLCtrl());
			}
			return { g_nullKeyEvent };
		}
		return { g_nullKeyEvent };
	}

	return { g_nullKeyEvent };
}

std::vector<Key> handleLCtrlKey(InterceptionKeyStroke keyStroke) {
	if (!g_isLCtrlKeyDown) {
		return {};
	}

	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (keyCode == SC_LSHIFT) { // LCtrl + Shift
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}

	if (keyCode == SC_TAB) { // LCtrl + Tab
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLCtrlAsLCtrl(), { KeyDown(SC_TAB), KeyUp(SC_TAB) });
		}
		return { g_nullKeyEvent };
	}

	if (keyCode != SC_LCTRL) { // LCtrl + letter
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLCtrlAsLAlt(), { KeyDown(keyCode), KeyUp(keyCode) });
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_LCTRL) { // LCtrl
		if (isCurrentKeyDown) {
			return keyDownLCtrlAsLAlt();
		}
		return keyUpLCtrl();
	}

	return {};
}

std::vector<Key> handleLWinKey(InterceptionKeyStroke keyStroke) {
	if (!g_isLWinKeyDown) {
		return {};
	}

	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (isStarcraft2ActiveProcess()) {
		if (keyCode == SC_X) { // lwin + x to alt + x
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), KeyDown(SC_X), KeyUp(SC_X), KeyUp(SC_LALT) };
			}
			return { g_nullKeyEvent };
		}

		if (keyCode == SC_1 || keyCode == SC_2 || keyCode == SC_3 || keyCode == SC_4) { // lwin + 1/2/3/4
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), KeyDown(keyCode), KeyUp(keyCode), KeyUp(SC_LALT) };
			}
			return { g_nullKeyEvent };
		}
	}

	if (isGitBashActiveProcess()) {
		if (keyCode == SC_BACK) { // lwin + backspace : delete from cursor to beginning of word
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyDownLAltAsLCtrl(), { KeyDown(SC_W), KeyUp(SC_W) }, keyUpLAlt());
			}
			return { g_nullKeyEvent };
		}
	}

	if (keyCode == SC_H) { // lwin + h
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LALT), KeyDown(SC_LEFT), KeyUp(SC_LEFT), KeyUp(SC_LALT) };
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_L) { // lwin + l
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LALT), KeyDown(SC_RIGHT), KeyUp(SC_RIGHT), KeyUp(SC_LALT) };
		}
		return { g_nullKeyEvent };
	}

	// TODO: Probably to do a combo to for example, get accented keys
	if (keyCode == SC_E) { // LWin + E
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_BACK) { // LWin + Backspace
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LCTRL), KeyDown(keyCode), KeyUp(keyCode), KeyUp(SC_LCTRL) };
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_D) { // LWin + D
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LWIN, 2), KeyDown(keyCode), KeyUp(keyCode), KeyUp(SC_LWIN, 3) };
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_SPACE) { // LWin + Space
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LWIN, 2), KeyUp(SC_LWIN, 3) };
		}
		return { g_nullKeyEvent };
	}

	return { g_nullKeyEvent };
}

std::vector<Key> handleLAltKey(InterceptionKeyStroke keyStroke) {
	if (!g_isLAltKeyDown) {
		return {};
	}

	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (isChromeActiveProcess()) {
		if (keyCode == SC_RETURN) { // lalt + enter to alt + enter
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyDownLAltAsLAlt(), { KeyDown(SC_RETURN), KeyUp(SC_RETURN) }, keyDownLAltAsLCtrl());
			}
			return { g_nullKeyEvent };
		}

		if (keyCode == SC_H) { // alt + h
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LSHIFT), KeyDown(SC_TAB), KeyUp(SC_TAB) };
			}
			return { KeyUp(SC_LSHIFT) };
		}

		if (keyCode == SC_L) { // alt + l
			if (isCurrentKeyDown) {
				return { KeyDown(SC_TAB), KeyUp(SC_TAB) };
			}
			return { g_nullKeyEvent };
		}

		if (keyCode == SC_SEMI) { // alt + ;
			if (isCurrentKeyDown) {
				return { KeyDown(SC_L), KeyUp(SC_L) };
			}
			return { g_nullKeyEvent };
		}
	}

	if (isStarcraft2ActiveProcess()) {
		if (
			keyCode == SC_F1 ||
			keyCode == SC_F2 ||
			keyCode == SC_F3 ||
			keyCode == SC_F4
			) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyDownLAltAsLCtrl(), { KeyDown(keyCode), KeyUp(keyCode) });
			}
			return { g_nullKeyEvent };
		}
	}

	if (isSlackActiveProcess()) {
		if (keyCode == SC_P) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyDownLAltAsLCtrl(), { KeyDown(SC_K), KeyUp(SC_K) });
			}
			return { g_nullKeyEvent };
		}
	}

	if (isGitBashActiveProcess()) {
		// TODO: lalt + a : select all
		if (keyCode == SC_C) { // lalt + c : copy
			if (isCurrentKeyDown) {
				return { KeyDown(SC_NP0), KeyUp(SC_NP0) };
			}
			return { g_nullKeyEvent };
		}

		if (keyCode == SC_V) { // lalt + v : paste
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LSHIFT), KeyDown(SC_NP0), KeyUp(SC_NP0), KeyUp(SC_LSHIFT) }, keyDownLAltAsLCtrl());
			}
			return { g_nullKeyEvent };
		}

		if (keyCode == SC_Z) { // lalt + z : undo
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LSHIFT), KeyDown(SC_MINUS), KeyDown(SC_MINUS), KeyUp(SC_LSHIFT) };
			}
			return { g_nullKeyEvent };
		}

		if (keyCode == SC_BACK) { // lalt + backspace : delete from cursor to beginning of line
			if (isCurrentKeyDown) {
				return { KeyDown(SC_U), KeyUp(SC_U) };
			}
			return { g_nullKeyEvent };
		}

		if (keyCode == SC_I) { // lalt + i : Clear screen
			if (isCurrentKeyDown) {
				return { KeyDown(SC_L), KeyUp(SC_L) };
			}
			return { g_nullKeyEvent };
		}
	}

	if (keyCode == SC_LSHIFT) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		}
		return { KeyUp(SC_LSHIFT) };
	}

	if (keyCode == SC_GRAVE) { // lalt + ` to alt + enter
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLAltAsLAlt(), { KeyDown(SC_GRAVE), KeyUp(SC_GRAVE) }, keyDownLAltAsLCtrl());
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_ESC && !g_isLAltAsLCtrl) { // alttabbed + esc
		if (isCurrentKeyDown) {
			return { KeyDown(SC_ESC), KeyUp(SC_ESC) };
		}
		return keyDownLAltAsLCtrl();
	}

	if (keyCode == SC_ESC) { // alt + esc
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_Q && !g_isLAltAsLCtrl) { // alt + tab + q
		if (isCurrentKeyDown) {
			return { KeyDown(SC_SUPR), KeyUp(SC_SUPR) };
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_Q) { // alt + q
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLAltAsLAlt(), { KeyDown(SC_F4), KeyUp(SC_F4) });
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_TAB) { // alt + tab
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLAltAsLAlt(), { KeyDown(SC_TAB), KeyUp(SC_TAB) });
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_BACK) { // alt + backspace
		if (isCurrentKeyDown) {
			return concatKeyVectors(
				keyUpLAlt(),
				{ KeyDown(SC_LSHIFT), KeyDown(SC_HOME), KeyUp(SC_HOME), KeyUp(SC_LSHIFT), KeyDown(SC_BACK), KeyUp(SC_BACK) },
				keyDownLAltAsLCtrl()
			);
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_J) { // alt + j
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_NEXT), KeyUp(SC_NEXT) }, keyDownLAltAsLCtrl());
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_K) { // alt + k
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_PRIOR), KeyUp(SC_PRIOR) }, keyDownLAltAsLCtrl());
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_SPACE) { // alt + espace
		if (isCurrentKeyDown) {
			return { KeyDown(SC_F12), KeyUp(SC_F12) };
		}
		return { g_nullKeyEvent };
	}

	if (
		keyCode == SC_F1 ||
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
		keyCode == SC_F12
		) { // alt + f{n} - NOTE: it's not possible to send ctrl + (shift) + f{n}
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(keyCode), KeyUp(keyCode) });
		}
		return { g_nullKeyEvent };
	}

	if (keyCode != SC_LALT && !g_isLAltAsLCtrl) { // alttabbed + letter
		return { g_nullKeyEvent };
	}

	if (keyCode != SC_LALT) { // alt + letter
		if (isCurrentKeyDown) {
			return { KeyDown(keyCode), KeyUp(keyCode) };
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_LALT) { // lalt
		if (isCurrentKeyDown) {
			return keyDownLAltAsLCtrl();
		}
		return keyUpLAlt();
	}

	return {};
}

std::vector<Key> handleShiftKey(InterceptionKeyStroke keyStroke) {
	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (!g_isShiftKeyDown) {
		return {};
	}

	if (keyCode == SC_LSHIFT) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LSHIFT) };
		} else {
			return { KeyUp(SC_LSHIFT) };
		}
	}

	return {};
}

std::vector<Key> handleKey(InterceptionKeyStroke keyStroke) {
	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (isChromeActiveProcess()) {
		if (keyCode == SC_F3) { // f3
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), KeyDown(SC_LSHIFT), KeyDown(SC_TAB) };
			}
			return { KeyUp(SC_TAB), KeyUp(SC_LSHIFT), KeyUp(SC_LCTRL) };
		}

		if (keyCode == SC_F4) { // f4
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), KeyDown(SC_TAB) };
			}
			return { KeyUp(SC_TAB), KeyUp(SC_LCTRL) };
		}

		if (keyCode == SC_F5) { // F5
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), KeyDown(SC_M) };
			}
			return { KeyUp(SC_M), KeyUp(SC_LALT) };
		}

		if (keyCode == SC_F6) { // f6
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), KeyDown(SC_T) };
			}
			return { KeyUp(SC_T), KeyUp(SC_LALT) };
		}
	}

	if (keyCode == SC_F1) { // f1
		if (isCurrentKeyDown) {
			return { KeyDown(SC_BRIGHTNESSDOWN) };
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_F2) { // f2
		if (isCurrentKeyDown) {
			return { KeyDown(SC_BRIGHTNESSUP) };
		}
		return { g_nullKeyEvent };
	}

	if (keyCode == SC_F10) { // f10
		if (isCurrentKeyDown) {
			return { KeyDown(SC_MUTE, 2) };
		}
		return { KeyUp(SC_MUTE, 3) };
	}

	if (keyCode == SC_F11) { // f11
		if (isCurrentKeyDown) {
			return {
				KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3),
				KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3)
			};
		}
		return {
			KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3),
			KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3)
		};
	}

	if (keyCode == SC_F12) { // f2
		if (isCurrentKeyDown) {
			return {
				KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3),
				KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3)
			};
		}
		return {
			KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3),
			KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3)
		};
	}

	if (isCurrentKeyDown) {
		return { KeyDown(keyCode) };
	}
	return { KeyUp(keyCode) };
}

std::vector<Key> getKeyEvents(std::vector<InterceptionKeyStroke> keyStrokes) {
	int keyStrokesSize = keyStrokes.size();
	std::vector<Key> allKeyEvents;

	for (int i = 0; i < keyStrokesSize; i++) {
		InterceptionKeyStroke keyStroke = keyStrokes[i];
		bool isCurrentKeyDown = isKeyDown(keyStroke);
		std::vector<Key> keyEvents;
		int keyEventsSize = 0;
		DWORD keyCode = keyStroke.code;

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

		if (keyEvents = handleSimulateMouseClick(keyStroke), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleLWinLAltKeys(keyStroke), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleCapslockKey(keyStroke), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleLCtrlKey(keyStroke), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleLWinKey(keyStroke), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleLAltKey(keyStroke), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleShiftKey(keyStroke), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}
		else if (keyEvents = handleKey(keyStroke), keyEventsSize = keyEvents.size(), keyEventsSize != 0) {}

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
