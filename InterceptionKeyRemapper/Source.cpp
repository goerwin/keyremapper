#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <sstream>
#include <chrono>
#include <shellapi.h>
#include <vector>
#include "resource.h"
#include "brightness.h"
#include "interception.h"
#include "utils.h"
#include "erwinUtils.h"

enum ScanCodes {
	SC_C = 0x2E,
	SC_D = 0x20,
	SC_E = 0x12,
	SC_H = 0x23,
	SC_I = 0x17,
	SC_J = 0x24,
	SC_K = 0x25,
	SC_L = 0x26,
	SC_M = 0x32,
	SC_P = 0x19,
	SC_Q = 0x10,
	SC_S = 0x1F,
	SC_T = 0x14,
	SC_U = 0x16,
	SC_V = 0x2F,
	SC_W = 0x11,
	SC_X = 0x2D,
	SC_Y = 0x15,
	SC_Z = 0x2C,
	SC_1 = 0x02,
	SC_2 = 0x03,
	SC_3 = 0x04,
	SC_4 = 0x05,
	SC_5 = 0x06,
	SC_6 = 0x07,
	SC_7 = 0x08,
	SC_8 = 0x09,
	SC_9 = 0x0A,
	SC_0 = 0x0B,
	SC_MUTE = 0xA0,
	SC_VOLUMEDOWN = 0xAE,
	SC_VOLUMEUP = 0xB0,
	SC_ESC = 0x01,
	SC_CAPSLOCK = 0x3A,
	SC_LEFT = 0xCB, // 0x4B
	SC_RIGHT = 0xCD, // 0x4D
	SC_UP = 0xC8, // 0x48
	SC_DOWN = 0xD0, // 0x50
	SC_SPACE = 0x39,
	SC_LWIN = 0x5B,
	SC_LALT = 0x38,
	SC_RALT = 0xB8,
	SC_LCTRL = 0x1D,
	SC_RCTRL = 0x9D,
	SC_LSHIFT = 0x2A,
	SC_RSHIFT = 0x36,
	SC_LBSLASH = 0x56,
	SC_RETURN = 0x1C,
	SC_SUPR = 0x53,
	SC_BACK = 0x0E,
	SC_TAB = 0x0F,
	SC_HOME = 0x47,
	SC_END = 0x4F,
	SC_PRIOR = 0x49,
	SC_NEXT = 0x51,
	SC_SEMI = 0x27,
	SC_NP0 = 0x52, // Insert
	SC_MINUS = 0x0C,
	SC_GRAVE = 0x29, // accent `
	SC_F1 = 0x3B,
	SC_F2 = 0x3C,
	SC_F3 = 0x3D,
	SC_F4 = 0x3E,
	SC_F5 = 0x3F,
	SC_F6 = 0x40,
	SC_F7 = 0x41,
	SC_F8 = 0x42,
	SC_F9 = 0x43,
	SC_F10 = 0x44,
	SC_F11 = 0x57,
	SC_F12 = 0x58,
	// Custom ScanCodes. Hope no keyboard ever sends this.
	SC_NULL = 0xF0,
	SC_MOUSELEFT = 0xF1,
	SC_MOUSERIGHT = 0xF2
};

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

HHOOK hHook = 0;
HWINEVENTHOOK windowHook = 0;
std::string activeProcessName;
const auto appTitle = L"Windows KeyRemapper";
DWORD globalDelayMSBetweenKeyEvents = 5;

bool isKeyForClickCurrentKeyCode;
int EVENT_HANDLED = 70;

bool isCapslockKeyDown;
bool isShiftKeyDown;
bool isLCtrlKeyDown;
bool isLWinKeyDown;
bool isLAltKeyDown;

struct Key {
	DWORD keyCode;
	int state;

	Key() {}

	Key(DWORD cKeyCode, int cState) {
		keyCode = cKeyCode;
		state = cState;
	}
};
struct KeyDown : Key {
	KeyDown(DWORD cKeyCode, int cState = 0) : Key(cKeyCode, cState) {}
};
struct KeyUp : Key {
	KeyUp(DWORD cKeyCode, int cState = 1) : Key(cKeyCode, cState) {}
};

bool isChromeActiveProcess() {
	return activeProcessName == "chrome.exe";
}
bool isStarcraft2ActiveProcess() {
	return activeProcessName == "SC2_x64.exe";
}
bool isSlackActiveProcess() {
	return activeProcessName == "slack.exe";
}
bool isGitBashActiveProcess() {
	return activeProcessName == "mintty.exe";
}

void setKeyToKeyRemaps(InterceptionKeyStroke &keyStroke) {
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

std::wstring getStringKeyInfo(InterceptionKeyStroke keyStroke) {
	std::wstring pressedStatus = isKeyDown(keyStroke) ? L"down" : L"up";
	return std::to_wstring(keyStroke.code).append(L" ").append(pressedStatus);
}

bool isLCtrlAsLAlt = true;
std::vector<Key> keyDownLCtrlAsLAlt() {
	std::vector<Key> keys;

	if (!isLCtrlAsLAlt && isLCtrlKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	isLCtrlAsLAlt = true;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
std::vector<Key> keyDownLCtrlAsLCtrl() {
	std::vector<Key> keys;

	if (isLCtrlAsLAlt && isLCtrlKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	}

	isLCtrlAsLAlt = false;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
std::vector<Key> keyUpLCtrl() {
	std::vector<Key> keys;

	if (isLCtrlAsLAlt) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });
	} else {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	isLCtrlAsLAlt = true;
	return keys;
}

bool isLAltAsLCtrl = true;
std::vector<Key> keyDownLAltAsLCtrl() {
	std::vector<Key> keys;

	if (!isLAltAsLCtrl && isLAltKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LALT) });	
	}

	isLAltAsLCtrl = true;
	keys.insert(keys.end(), { KeyDown(SC_LCTRL) });
	return keys;
}
std::vector<Key> keyDownLAltAsLAlt() {
	std::vector<Key> keys;

	if (isLAltAsLCtrl && isLAltKeyDown) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	}

	isLAltAsLCtrl = false;
	keys.insert(keys.end(), { KeyDown(SC_LALT) });
	return keys;
}
std::vector<Key> keyUpLAlt() {
	std::vector<Key> keys;

	if (isLAltAsLCtrl) {
		keys.insert(keys.begin(), { KeyUp(SC_LCTRL) });
	} else {
		keys.insert(keys.end(), { KeyUp(SC_LALT) });
	}

	isLAltAsLCtrl = true;
	return keys;
}

std::vector<Key> nullKeyEvent = { KeyUp(SC_NULL) };

std::vector<Key> concatKeyVectors(std::vector<Key> keys, std::vector<Key> keys2, std::vector<Key> keys3 = {}, std::vector<Key> keys4 = {}) {
	keys.insert(keys.end(), keys2.begin(), keys2.end());
	keys.insert(keys.end(), keys3.begin(), keys3.end());
	keys.insert(keys.end(), keys4.begin(), keys4.end());
	return keys;
}

void sendKeyEvents(std::pair<std::vector<Key>, int> keyEventsAndSize) {
	auto keyEvents = keyEventsAndSize.first;
	auto keyEventsSize = keyEventsAndSize.second;

	for (int i = 0; i < keyEventsSize; i++) {
		DWORD keyCode = keyEvents[i].keyCode;
		int state = keyEvents[i].state;
		InterceptionKeyStroke newKeyStroke;
		newKeyStroke.code = keyCode;
		newKeyStroke.state = state;

		if (keyCode == SC_MOUSELEFT) {
			if (state == 0) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			} else {
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}
		} else if (keyCode == SC_MOUSERIGHT) {
			if (state == 0) {
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			} else {
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			}
		} else if (keyCode != SC_NULL) {
			interception_send(context, device, (InterceptionStroke *)&newKeyStroke, 1);
		}
	}
}

// You have to phisically press a key to trigger
// the correct clean up
void handleAppCleanUp() {
	sendKeyEvents({
		{
			KeyDown(SC_LALT),
			KeyUp(SC_LALT),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LWIN),
			KeyUp(SC_LWIN),
			KeyDown(SC_LSHIFT),
			KeyUp(SC_LSHIFT)
		},
		8 
	});
}

bool isMouseClickDown;
std::vector<Key> handleSimulateMouseClick(InterceptionKeyStroke keyStroke) {
	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (
		!(isLWinKeyDown && keyCode == SC_C) && 
		!(isLWinKeyDown && keyCode == SC_LSHIFT) && 
		!(isLWinKeyDown && keyCode == SC_LALT) && 
		!isMouseClickDown
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
		if (isCapslockKeyDown) {
			if (isCurrentKeyDown) {
				if (!isMouseClickDown) {
					isMouseClickDown = true;
					return { KeyDown(SC_MOUSERIGHT) };
				}
				return nullKeyEvent;
			}
			isMouseClickDown = false;
			return { KeyUp(SC_MOUSERIGHT) };
		}
		
		if (isCurrentKeyDown) {
			if (!isMouseClickDown) {
				isMouseClickDown = true;
				return { KeyDown(SC_MOUSELEFT) };
			}
			return nullKeyEvent;
		}
		isMouseClickDown = false;
		return { KeyUp(SC_MOUSELEFT) };
	}

	return {};
}

std::vector<Key> handleLWinLAltKeys(InterceptionKeyStroke keyStroke) {
	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (!(isLWinKeyDown && isLAltKeyDown)) {
		return {};
	}

	// NOTE: For this snapping windows function you have to disable
	// the option "When I Snap a window, show what I can snap next to it"
	// under "Multitasking" settings in control panel
	if (keyCode == SC_K) { // win + alt + k
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), {
				KeyDown(SC_LWIN, 2),
				KeyDown(SC_UP),
				KeyUp(SC_UP),
				KeyUp(SC_LWIN, 3)
			});
		}
		return nullKeyEvent;
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
		return nullKeyEvent;
	}

	if (keyCode == SC_L) { // win + alt + l
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LWIN, 2), KeyDown(SC_RIGHT), KeyUp(SC_RIGHT), KeyUp(SC_LWIN, 3) });
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_H) { // win + alt + h
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LWIN, 2), KeyDown(SC_LEFT), KeyUp(SC_LEFT), KeyUp(SC_LWIN, 3) });
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_LALT) {
		if (isCurrentKeyDown) {
			return nullKeyEvent;
		}
		return keyUpLAlt();
	}

	if (keyCode == SC_LCTRL) {
		if (isCurrentKeyDown) {
			return nullKeyEvent;
		}
		return keyDownLAltAsLCtrl();
	}

	return nullKeyEvent;
}

bool isVimShiftKeyDown = false;
std::vector<Key> handleCapslockKey(InterceptionKeyStroke keyStroke) {
	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD currentKeyCode = keyStroke.code;

	if (!isCapslockKeyDown) {
		return {};
	}

	if (isGitBashActiveProcess()) {
		if (currentKeyCode == SC_C && !isLAltKeyDown) { // capslock + c : ctrl + c : kill process
			if (isCurrentKeyDown) {
				return concatKeyVectors(
					keyDownLAltAsLCtrl(),
					{ KeyDown(SC_C), KeyUp(SC_C) },
					keyUpLAlt()
				);
			}
			return nullKeyEvent;
		}
	}

	if (currentKeyCode == SC_CAPSLOCK || currentKeyCode == SC_LSHIFT || currentKeyCode == SC_S) {
		if (isCurrentKeyDown) {
			if (currentKeyCode == SC_LSHIFT || currentKeyCode == SC_S) {
				isVimShiftKeyDown = true;
				return { KeyDown(SC_LSHIFT) };
			}
			return nullKeyEvent;
		}
		
		if (isVimShiftKeyDown) {
			isVimShiftKeyDown = false;
			return { KeyUp(SC_LSHIFT) };
		}
		return nullKeyEvent;
	}

	if (currentKeyCode == SC_LALT) {
		if (isCurrentKeyDown) {
			return nullKeyEvent;
		}
		return keyUpLAlt();
	}

	if (currentKeyCode == SC_TAB) {
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LCTRL), KeyDown(SC_TAB), KeyUp(SC_TAB), KeyUp(SC_LCTRL) };
		}
		return nullKeyEvent;
	}

	if (currentKeyCode == SC_SPACE) {
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLAltAsLCtrl(), { KeyDown(currentKeyCode), KeyUp(currentKeyCode) }, keyUpLAlt());
		}
		return nullKeyEvent;
	}

	if (currentKeyCode == SC_H) {
		if (isLWinKeyDown) {
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), KeyDown(SC_LEFT), KeyUp(SC_LEFT), KeyUp(SC_LCTRL) };
			}
			return nullKeyEvent;
		}

		if (isLAltKeyDown && isLAltAsLCtrl) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_HOME), KeyUp(SC_HOME) }, keyDownLAltAsLCtrl());
			}
			return nullKeyEvent;
		}
			
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LEFT), KeyUp(SC_LEFT) };
		}
		return nullKeyEvent;
	} 
	
	if (currentKeyCode == SC_L) {
		if (isLWinKeyDown) {
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), KeyDown(SC_RIGHT), KeyUp(SC_RIGHT), KeyUp(SC_LCTRL) };
			}
			return nullKeyEvent;
		}

		if (isLAltKeyDown && isLAltAsLCtrl) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_END), KeyUp(SC_END) }, keyDownLAltAsLCtrl());
			}
			return nullKeyEvent;
		}

		if (isCurrentKeyDown) {
			return { KeyDown(SC_RIGHT), KeyUp(SC_RIGHT) };
		}
		return nullKeyEvent;
	}
	
	if (currentKeyCode == SC_J) {
		if (isLAltKeyDown && isLAltAsLCtrl) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LCTRL), KeyDown(SC_END), KeyUp(SC_END), KeyUp(SC_LCTRL) }, keyDownLAltAsLCtrl());
			}
			return nullKeyEvent;
		}

		if (isCurrentKeyDown) {
			// Here you MAY in the Alt+Tab switcher
			return { KeyDown(SC_DOWN), KeyUp(SC_DOWN) };
		}
		return nullKeyEvent;
	}
	
	if (currentKeyCode == SC_K) {
		if (isLAltKeyDown && isLAltAsLCtrl) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LCTRL), KeyDown(SC_HOME), KeyUp(SC_HOME), KeyUp(SC_LCTRL) }, keyDownLAltAsLCtrl());
			}
			return nullKeyEvent;
		}

		if (isCurrentKeyDown) {
			// Here you MAY in the Alt+Tab switcher
			return { KeyDown(SC_UP), KeyUp(SC_UP) };
		}
		return nullKeyEvent;
	}

	if (currentKeyCode == SC_V) {
		if (isLAltKeyDown && isLAltAsLCtrl) {
			if (isCurrentKeyDown) { // caps + lalt + V
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LWIN, 2), KeyDown(SC_V), KeyUp(SC_V), KeyUp(SC_LWIN, 3) }, keyDownLAltAsLCtrl());
			}
			return nullKeyEvent;
		}
		return nullKeyEvent;
	}

	return nullKeyEvent;
}

std::vector<Key> handleLCtrlKey(InterceptionKeyStroke keyStroke) {
	if (!isLCtrlKeyDown) {
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
		return nullKeyEvent;
	}
	
	if (keyCode != SC_LCTRL) { // LCtrl + letter
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLCtrlAsLAlt(), { KeyDown(keyCode), KeyUp(keyCode) });
		}
		return nullKeyEvent;
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
	if (!isLWinKeyDown) {
		return {};
	}

	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (isStarcraft2ActiveProcess()) {
		if (keyCode == SC_X) { // lwin + x to alt + x
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), KeyDown(SC_X), KeyUp(SC_X), KeyUp(SC_LALT) };
			} 
			return nullKeyEvent;
		}

		if (keyCode == SC_1 || keyCode == SC_2 || keyCode == SC_3 || keyCode == SC_4) { // lwin + 1/2/3/4
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), KeyDown(keyCode), KeyUp(keyCode), KeyUp(SC_LALT) };
			} 
			return nullKeyEvent;
		}
	}

	if (isGitBashActiveProcess()) {
		if (keyCode == SC_BACK) { // lwin + backspace : delete from cursor to beginning of word
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyDownLAltAsLCtrl(), { KeyDown(SC_W), KeyUp(SC_W) }, keyUpLAlt());
			}
			return nullKeyEvent;
		}
	}

	if (keyCode == SC_H) { // lwin + h
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LALT), KeyDown(SC_LEFT), KeyUp(SC_LEFT), KeyUp(SC_LALT) };
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_L) { // lwin + l
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LALT), KeyDown(SC_RIGHT), KeyUp(SC_RIGHT), KeyUp(SC_LALT) };
		}
		return nullKeyEvent;
	}

	// TODO: Probably to do a combo to for example, get accented keys
	if (keyCode == SC_E) { // LWin + E
		return nullKeyEvent;
	}

	if (keyCode == SC_BACK) { // LWin + Backspace
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LCTRL), KeyDown(keyCode), KeyUp(keyCode), KeyUp(SC_LCTRL) };
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_D) { // LWin + D
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LWIN, 2), KeyDown(keyCode), KeyUp(keyCode), KeyUp(SC_LWIN, 3) };
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_SPACE) { // LWin + Space
		if (isCurrentKeyDown) {
			return { KeyDown(SC_LWIN, 2), KeyUp(SC_LWIN, 3) };
		}
		return nullKeyEvent;
	}

	return nullKeyEvent;
}

std::vector<Key> handleLAltKey(InterceptionKeyStroke keyStroke) {
	if (!isLAltKeyDown) {
		return {};
	}

	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	if (isChromeActiveProcess()) {
		if (keyCode == SC_RETURN) { // lalt + enter to alt + enter
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyDownLAltAsLAlt(), { KeyDown(SC_RETURN), KeyUp(SC_RETURN) }, keyDownLAltAsLCtrl());
			} 
			return nullKeyEvent;
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
			return nullKeyEvent;
		}

		if (keyCode == SC_SEMI) { // alt + ;
			if (isCurrentKeyDown) {
				return { KeyDown(SC_L), KeyUp(SC_L) };
			}
			return nullKeyEvent;
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
			return nullKeyEvent;
		}
	}

	if (isSlackActiveProcess()) {
		if (keyCode == SC_P) {
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyDownLAltAsLCtrl(), { KeyDown(SC_K), KeyUp(SC_K) });
			}
			return nullKeyEvent;
		}
	}

	if (isGitBashActiveProcess()) {
		// TODO: lalt + a : select all
		if (keyCode == SC_C) { // lalt + c : copy
			if (isCurrentKeyDown) {
				return { KeyDown(SC_NP0), KeyUp(SC_NP0) };
			}
			return nullKeyEvent;
		}

		if (keyCode == SC_V) { // lalt + v : paste
			if (isCurrentKeyDown) {
				return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LSHIFT), KeyDown(SC_NP0), KeyUp(SC_NP0), KeyUp(SC_LSHIFT) }, keyDownLAltAsLCtrl());
			}
			return nullKeyEvent;
		}

		if (keyCode == SC_Z) { // lalt + z : undo
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LSHIFT), KeyDown(SC_MINUS), KeyDown(SC_MINUS), KeyUp(SC_LSHIFT) };
			}
			return nullKeyEvent;
		}

		if (keyCode == SC_BACK) { // lalt + backspace : delete from cursor to beginning of line
			if (isCurrentKeyDown) {
				return { KeyDown(SC_U), KeyUp(SC_U) };
			}
			return nullKeyEvent;
		}

		if (keyCode == SC_I) { // lalt + i : Clear screen
			if (isCurrentKeyDown) {
				return { KeyDown(SC_L), KeyUp(SC_L) };
			}
			return nullKeyEvent;
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
		return nullKeyEvent;
	}

	if (keyCode == SC_ESC && !isLAltAsLCtrl) { // alttabbed + esc
		if (isCurrentKeyDown) {
			return { KeyDown(SC_ESC), KeyUp(SC_ESC) };
		}
		return keyDownLAltAsLCtrl();
	}

	if (keyCode == SC_ESC) { // alt + esc
		return nullKeyEvent;
	}

	if (keyCode == SC_Q && !isLAltAsLCtrl) { // alt + tab + q
		if (isCurrentKeyDown) {
			return { KeyDown(SC_SUPR), KeyUp(SC_SUPR) };
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_Q) { // alt + q
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), keyDownLAltAsLAlt(), { KeyDown(SC_F4), KeyUp(SC_F4) });
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_TAB) { // alt + tab
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLAltAsLAlt(), { KeyDown(SC_TAB), KeyUp(SC_TAB) });
		}
		return { KeyUp(SC_TAB) };
	}

	if (keyCode == SC_BACK) { // alt + backspace
		if (isCurrentKeyDown) {
			return concatKeyVectors(
				keyUpLAlt(),
				{ KeyDown(SC_LSHIFT), KeyDown(SC_HOME), KeyUp(SC_HOME), KeyUp(SC_LSHIFT), KeyDown(SC_BACK), KeyUp(SC_BACK) }, 
				keyDownLAltAsLCtrl()
			);
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_J && isShiftKeyDown) { // alt + shift + j
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LSHIFT), KeyDown(SC_NEXT), KeyUp(SC_NEXT) }, keyDownLAltAsLCtrl());
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_K && isShiftKeyDown) { // alt + shift + k
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_LSHIFT), KeyDown(SC_PRIOR), KeyUp(SC_PRIOR) }, keyDownLAltAsLCtrl());
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_J) { // alt + j
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_NEXT), KeyUp(SC_NEXT) }, keyDownLAltAsLCtrl());
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_K) { // alt + k
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyUpLAlt(), { KeyDown(SC_PRIOR), KeyUp(SC_PRIOR) }, keyDownLAltAsLCtrl());
		}
		return nullKeyEvent;
	}

	if (keyCode == SC_SPACE) { // alt + espace
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLAltAsLCtrl(), { KeyDown(SC_F12), KeyUp(SC_F12) });
		}
		return nullKeyEvent;
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
		return nullKeyEvent;
	}

	if (keyCode != SC_LALT && !isLAltAsLCtrl) { // alttabbed + letter
		return nullKeyEvent;
	}

	if (keyCode != SC_LALT) { // alt + letter
		if (isCurrentKeyDown) {
			return concatKeyVectors(keyDownLAltAsLCtrl(), { KeyDown(keyCode), KeyUp(keyCode) });
		}
		return nullKeyEvent;
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

	if (!isShiftKeyDown) {
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
			} else {
				return { KeyUp(SC_LCTRL), KeyUp(SC_LSHIFT), KeyUp(SC_TAB) };
			}
		}

		if (keyCode == SC_F4) { // f4
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LCTRL), KeyDown(SC_TAB) };
			} else {
				return { KeyUp(SC_LCTRL), KeyUp(SC_TAB) };
			}
		}

		if (keyCode == SC_F5) { // F5
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), KeyDown(SC_M) };
			} else {
				return { KeyUp(SC_LALT), KeyUp(SC_M) };
			}
		}

		if (keyCode == SC_F6) { // f6
			if (isCurrentKeyDown) {
				return { KeyDown(SC_LALT), KeyDown(SC_T) };
			} else {
				return { KeyUp(SC_LALT), KeyUp(SC_T) };
			}
		}
	}

	if (keyCode == SC_F1) { // f1
		if (isCurrentKeyDown) {
			BrightnessHandler::Increment(-10);
			return { KeyDown(SC_NULL) };
		} else {
			return { KeyUp(SC_NULL) };
		}
	}

	if (keyCode == SC_F2) { // f2
		if (isCurrentKeyDown) {
			BrightnessHandler::Increment(10);
			return { KeyDown(SC_NULL) };
		} else {
			return { KeyUp(SC_NULL) };
		}
	}

	if (keyCode == SC_F10) { // f10
		if (isCurrentKeyDown) {
			return { KeyDown(SC_MUTE, 2) };
		} else {
			return { KeyUp(SC_MUTE, 3) };
		}
	}

	if (keyCode == SC_F11) { // f11
		if (isCurrentKeyDown) {
			return {
				KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3),
				KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3)
			};
		} else {
			return {
				KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3),
				KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3)
			};
		}
	}

	if (keyCode == SC_F12) { // f2
		if (isCurrentKeyDown) {
			return { 
				KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3),
				KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3) 
			};
		} else {
			return {
				KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3),
				KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3)
			};
		}
	}

	if (isCurrentKeyDown) {
		return { KeyDown(keyCode) };
	} else {
		return { KeyUp(keyCode) };
	}
}

void CALLBACK handleWindowChange(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
	activeProcessName = ErwinUtils::getActiveWindowProcessName(hwnd);
	OutputDebugStringA(activeProcessName.c_str());
}

bool isAppEnabled = true;
NOTIFYICONDATA nid;
const auto ctxMenuEnabled = L"Disable";
const auto ctxMenuNotEnabled = L"Enable";
auto ctxMenuEnabledMsg = ctxMenuEnabled;
auto globalIconImage = HICON(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, 0));
//auto globalIconImageDisabled = HICON(LoadImage(NULL, TEXT("images/letter-e-red.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
auto globalIconImageDisabled = HICON(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 0, 0, 0));

void toggleAppEnabled() {
	isAppEnabled = !isAppEnabled;
	ctxMenuEnabledMsg = isAppEnabled ? ctxMenuEnabled : ctxMenuNotEnabled;
	nid.hIcon = isAppEnabled ? globalIconImage : globalIconImageDisabled;
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

#define WM_MYMESSAGE (WM_USER + 1)
const int IDM_EXIT = 5;
const int IDM_ENABLE = 6;
const auto ctxMenuExitMsg = L"Exit";
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_MYMESSAGE:
			switch (lParam) {
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
				// MessageBox(NULL, L"Clicked!", L"Title", MB_OK);
				POINT pt;
				GetCursorPos(&pt);
				HMENU hPopMenu = CreatePopupMenu();
				SetForegroundWindow(hWnd);

				InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT, ctxMenuExitMsg);
				InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_ENABLE, ctxMenuEnabledMsg);
				TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);
				return 0;
			};
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDM_EXIT:
					exit(0);
					return 0;
				case IDM_ENABLE:
					toggleAppEnabled();
					return 0;
			}
			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void handleAppExit() {
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

HWND createWindow(HINSTANCE hInstance) {
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"WinClass";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);
	HWND hWnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		appTitle,						// Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,       // Parent window
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	return hWnd;
}

// https://codingmisadventures.wordpress.com/2009/02/20/creating-a-system-tray-icon-using-visual-c-and-win32/
void handleSystemTrayIcon(HINSTANCE hInstance, HWND hWnd) {
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_MYMESSAGE;
	nid.hIcon = globalIconImage;
	wcscpy_s(nid.szTip, appTitle);
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &nid);
}

std::pair<std::vector<Key>, int> getKeyEvents(InterceptionKeyStroke keyStroke) {
	bool isCurrentKeyDown = isKeyDown(keyStroke);
	DWORD keyCode = keyStroke.code;

	std::vector<Key> keyEvents;
	int keyEventsSize = 0;

	if (isCurrentKeyDown) {
		if (keyCode == SC_CAPSLOCK) {
			isCapslockKeyDown = true;
		} else if (keyCode == SC_LSHIFT) {
			isShiftKeyDown = true;
		} else if (keyCode == SC_LCTRL) {
			isLCtrlKeyDown = true;
		} else if (keyCode == SC_LWIN) {
			isLWinKeyDown = true;
		} else if (keyCode == SC_LALT) {
			isLAltKeyDown = true;
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
			isCapslockKeyDown = false;
		} else if (keyCode == SC_LSHIFT) {
			isShiftKeyDown = false;
		} else if (keyCode == SC_LCTRL) {
			isLCtrlKeyDown = false;
		} else if (keyCode == SC_LWIN) {
			isLWinKeyDown = false;
		} else if (keyCode == SC_LALT) {
			isLAltKeyDown = false;
		}
	}

	return { keyEvents, keyEventsSize };
}

DWORD WINAPI keyboardThreadFunc(void* data) {
	raise_process_priority();
	context = interception_create_context();
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

	auto shiftClick = ErwinUtils::KeyClick<void(int)>(SC_LSHIFT, [](int consecutiveClicks) {
		if (consecutiveClicks > 1) {
			if ((GetKeyState(VK_CAPITAL) & 0x0001) == 0) { // Capslock OFF
				sendKeyEvents({ { KeyDown(SC_CAPSLOCK), KeyUp(SC_CAPSLOCK) }, 2 });
			}
		} else {
			if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) { // Capslock ON
				sendKeyEvents({ { KeyDown(SC_CAPSLOCK), KeyUp(SC_CAPSLOCK) }, 2 });
			}
		}
	});

	auto capslockClick = ErwinUtils::KeyClick<void(int)>(SC_CAPSLOCK, [](int consecutiveClicks) {
		if (consecutiveClicks == 1) {
			if (isLAltKeyDown && !isLAltAsLCtrl) { // altTabbed + esc
				auto keyEvents = concatKeyVectors(
					{ KeyDown(SC_ESC), KeyUp(SC_ESC) },
					keyDownLAltAsLCtrl()
				);

				sendKeyEvents({ keyEvents, keyEvents.size() });
			} else if (isLAltKeyDown) { // alt + esc
				// DO NOTHING
			} else {
				sendKeyEvents({ { KeyDown(SC_ESC), KeyUp(SC_ESC) }, 2 });
			}
		}
	});

	auto lAltClick = ErwinUtils::KeyClick<void(int)>(SC_LALT, [](int consecutiveClicks) {
		if (consecutiveClicks == 2) {
			handleAppCleanUp();
		}
	});

	while (interception_receive(
		context,
		device = interception_wait(context),
		(InterceptionStroke *)&keyStroke,
		1
	) > 0) {
		if (!isAppEnabled) {
			interception_send(context, device, (InterceptionStroke *)&keyStroke, 1);
			continue;
		}

		/*
		OutputDebugString(getStringKeyInfo(keyStroke).append(L"\n").c_str());
		interception_send(context, device, (InterceptionStroke *)&keyStroke, 1);
		continue;
		*/
		setKeyToKeyRemaps(keyStroke);
		
		DWORD keyCode = keyStroke.code;
		bool isCurrentKeyDown = isKeyDown(keyStroke);
		shiftClick.handleKeyStroke(keyCode, isCurrentKeyDown);
		capslockClick.handleKeyStroke(keyCode, isCurrentKeyDown);
		lAltClick.handleKeyStroke(keyCode, isCurrentKeyDown);

		sendKeyEvents(getKeyEvents(keyStroke));
	}

	interception_destroy_context(context);

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
	std::atexit(handleAppExit);

	// SYSTEM TRAY ICON

	HWND hWnd = createWindow(hInstance);

	if (!hWnd) {
		return 0;
	}

	handleSystemTrayIcon(hInstance, hWnd);

	// WINDOW CHANGE EVENT

	windowHook = SetWinEventHook(
		EVENT_SYSTEM_FOREGROUND,
		EVENT_SYSTEM_FOREGROUND,
		NULL,
		handleWindowChange,
		0, 0,
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
	);

	// KEYBOARD

	CreateThread(NULL, 0, keyboardThreadFunc, NULL, 0, NULL);

	MSG messages;
	while (GetMessage(&messages, NULL, 0, 0)) {
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	return messages.wParam;
}