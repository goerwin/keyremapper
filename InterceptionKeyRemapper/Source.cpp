#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <sstream>
#include <chrono>
#include <shellapi.h>
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
	SC_J = 0x24,
	SC_K = 0x25,
	SC_L = 0x26,
	SC_M = 0x32,
	SC_Q = 0x10,
	SC_S = 0x1F,
	SC_T = 0x14,
	SC_V = 0x2F,
	SC_X = 0x2D,
	SC_Y = 0x15,
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
};

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

HHOOK hHook = 0;
HWINEVENTHOOK windowHook = 0;
std::string activeProcessName;
const auto appTitle = L"Windows KeyRemapper";

bool isKeyForClickCurrentKeyCode;
int EVENT_HANDLED = 70;

bool isCapslockKeyDown;
bool isShiftKeyDown;
bool isLCtrlKeyDown;
bool isLWinKeyDown;
bool isLAltKeyDown;

bool isChromeActiveProcess() {
	return activeProcessName == "chrome.exe";
}
bool isStarcraft2ActiveProcess() {
	return activeProcessName == "SC2_x64.exe";
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

bool isShiftCurrentKeyCode(InterceptionKeyStroke keyStroke) {
	return keyStroke.code == SC_LSHIFT;
};
bool isLWinCurrentKeyCode(InterceptionKeyStroke keyStroke) {
	return keyStroke.code == SC_LWIN;
};
bool isCapslockCurrentKeyCode(InterceptionKeyStroke keyStroke) {
	return keyStroke.code == SC_CAPSLOCK;
}
bool isLAltCurrentKeyCode(InterceptionKeyStroke keyStroke) {
	return keyStroke.code == SC_LALT;
};
bool isLCTrlCurrentKeyCode(InterceptionKeyStroke keyStroke) {
	return keyStroke.code == SC_LCTRL;
};

bool isKeyDown(InterceptionKeyStroke keyStroke) {
	return keyStroke.state == 0 || keyStroke.state == 2;
}
bool isKeyUp(InterceptionKeyStroke keyStroke) {
	return keyStroke.state == 1 || keyStroke.state == 3;
}

void sendCustomKeyDownEvent(DWORD keyCode, int state = 0) {
	InterceptionKeyStroke newKeyStroke;
	newKeyStroke.code = keyCode;
	newKeyStroke.state = state;
	interception_send(context, device, (InterceptionStroke *)&newKeyStroke, 1);
	OutputDebugStringW(std::wstring(L"\nkeydown: ").append(std::to_wstring(keyCode)).c_str());
}
void sendCustomKeyUpEvent(DWORD keyCode, int state = 1) {
	InterceptionKeyStroke newKeyStroke;
	newKeyStroke.code = keyCode;
	newKeyStroke.state = state;
	interception_send(context, device, (InterceptionStroke *)&newKeyStroke, 1);
	OutputDebugStringW(std::wstring(L"\nkeyup: ").append(std::to_wstring(keyCode)).c_str());
}
void sendCustomKeyEvent(DWORD keyCode, int keyDownState = 0, int keyUpState = 1) {
	sendCustomKeyDownEvent(keyCode, keyDownState);
	sendCustomKeyUpEvent(keyCode, keyUpState);
}

bool isLCtrlAsLAlt = true;
void pressDownLCtrlAsLAlt() {
	if (!isLCtrlAsLAlt && isLCtrlKeyDown) {
		sendCustomKeyUpEvent(SC_LCTRL);
	}
	isLCtrlAsLAlt = true;
	sendCustomKeyDownEvent(SC_LALT);
}
void pressDownLCtrlAsLCtrl() {
	if (isLCtrlAsLAlt && isLCtrlKeyDown) {
		sendCustomKeyUpEvent(SC_LALT);
	}
	isLCtrlAsLAlt = false;
	sendCustomKeyDownEvent(SC_LCTRL);
}
void pressUpLCtrl() {
	if (isLCtrlAsLAlt) { 
		sendCustomKeyUpEvent(SC_LALT); 
	} else { 
		sendCustomKeyUpEvent(SC_LCTRL);
	}
	isLCtrlAsLAlt = true;
}

bool isLAltAsLCtrl = true;
void pressDownLAltAsLCtrl() {
	if (!isLAltAsLCtrl && isLAltKeyDown) {
		sendCustomKeyUpEvent(SC_LALT);
	}
	isLAltAsLCtrl = true;
	sendCustomKeyDownEvent(SC_LCTRL);
}
void pressDownLAltAsLAlt() {
	if (isLAltAsLCtrl && isLAltKeyDown) {
		sendCustomKeyUpEvent(SC_LCTRL);
	}
	isLAltAsLCtrl = false;
	sendCustomKeyDownEvent(SC_LALT);
}
void pressUpLAlt() {
	if (isLAltAsLCtrl) { 
		sendCustomKeyUpEvent(SC_LCTRL); 
	} else { 
		sendCustomKeyUpEvent(SC_LALT); 
	}
	isLAltAsLCtrl = true;
}

// You have to phisically press a key to trigger
// the correct clean up
void handleAppCleanUp() {
	sendCustomKeyEvent(SC_LALT);
	sendCustomKeyEvent(SC_LCTRL);
	sendCustomKeyEvent(SC_LWIN);
	sendCustomKeyEvent(SC_LSHIFT);
}

DWORD keyForClick = SC_C;
bool isMouseClickDown;
int handleSimulateMouseClick(InterceptionKeyStroke keyStroke) {
	if (isKeyDown(keyStroke)) {
		DWORD keyCode = keyStroke.code;

		if (!(isLWinKeyDown && keyCode == keyForClick)) {
			return 0;
		}

		if (isMouseClickDown) {
			return EVENT_HANDLED;
		}

		isMouseClickDown = true;

		if (isCapslockKeyDown) {
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		} else if (isShiftKeyDown) {
			sendCustomKeyDownEvent(SC_LSHIFT);
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		} else if (isLAltKeyDown) {
			pressDownLAltAsLCtrl();
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		} else {
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		}

		return EVENT_HANDLED;
	} else {
		if (!(isMouseClickDown && keyStroke.code == keyForClick)) {
			return 0;
		}

		isMouseClickDown = false;

		if (isCapslockKeyDown) {
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
		} else if (isShiftKeyDown) {
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			//sendCustomKeyUpEvent(SC_LSHIFT);
		} else if (isLAltKeyDown) {
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		} else {
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}

		return EVENT_HANDLED;
	}

	return 0;
}

int handleLWinLAltKeys(InterceptionKeyStroke keyStroke) {
	if (isKeyDown(keyStroke)) {
		if (!(isLWinKeyDown && isLAltKeyDown)) {
			return 0;
		}

		DWORD keyCode = keyStroke.code;

		// NOTE: For this snapping windows function you have to disable
		// the option "When I Snap a window, show what I can snap next to it"
		// under "Multitasking" settings in control panel
		if (keyCode == SC_K) { // win + alt + k
			pressUpLAlt();
			sendCustomKeyDownEvent(SC_LWIN, 2);
			sendCustomKeyEvent(SC_UP);
			sendCustomKeyUpEvent(SC_LWIN, 3);
			return EVENT_HANDLED;
		}

		if (keyCode == SC_J) { // win + alt + j
			pressUpLAlt();
			sendCustomKeyDownEvent(SC_LWIN, 2);
			sendCustomKeyEvent(SC_DOWN);
			sendCustomKeyUpEvent(SC_LWIN, 3);
			return EVENT_HANDLED;
		}

		if (keyCode == SC_L) { // win + alt + l
			pressUpLAlt();
			sendCustomKeyDownEvent(SC_LWIN, 2);
			sendCustomKeyEvent(SC_RIGHT);
			sendCustomKeyUpEvent(SC_LWIN, 3);
			return EVENT_HANDLED;
		}

		if (keyCode == SC_H) { // win + alt + h
			pressUpLAlt();
			sendCustomKeyDownEvent(SC_LWIN, 2);
			sendCustomKeyEvent(SC_LEFT);
			sendCustomKeyUpEvent(SC_LWIN, 3);
			return EVENT_HANDLED;
		}
	} else {
		if (isLWinKeyDown && isLAltCurrentKeyCode(keyStroke)) {
			pressUpLAlt();
			return EVENT_HANDLED;
		} else if (isLAltKeyDown && isLWinCurrentKeyCode(keyStroke)) {
			pressDownLAltAsLCtrl();
			return EVENT_HANDLED;
		} else if (isLWinKeyDown && isLAltKeyDown) {
			return EVENT_HANDLED;
		}
	}
	
	return 0;
}

DWORD shiftLetterForVimMode = SC_S;
bool isVimShiftKeyDown = false;
bool isShiftLetterForVimModeKeyDown = false;
int handleCapslockKey(InterceptionKeyStroke keyStroke) {
	DWORD currentKeyCode = keyStroke.code;

	if (isKeyDown(keyStroke)) {
		if (!isCapslockKeyDown) {
			return 0;
		}

		bool isLCurrentKeyCode = currentKeyCode == SC_L;
		bool isHCurrentKeyCode = currentKeyCode == SC_H;
		bool isJCurrentKeyCode = currentKeyCode == SC_J;
		bool isKCurrentKeyCode = currentKeyCode == SC_K;

		if (currentKeyCode == shiftLetterForVimMode) {
			isShiftLetterForVimModeKeyDown = true;
		}

		isVimShiftKeyDown = isShiftKeyDown || isShiftLetterForVimModeKeyDown;

		if (isLWinKeyDown) {
			// Here you are OUT of the Alt+Tab switcher
			if (isVimShiftKeyDown) {
				sendCustomKeyDownEvent(SC_LSHIFT);
			}

			if (isHCurrentKeyCode) {
				sendCustomKeyDownEvent(SC_LCTRL);
				sendCustomKeyEvent(SC_LEFT);
				sendCustomKeyUpEvent(SC_LCTRL);
			} else if (isLCurrentKeyCode) {
				sendCustomKeyDownEvent(SC_LCTRL);
				sendCustomKeyEvent(SC_RIGHT);
				sendCustomKeyUpEvent(SC_LCTRL);
			}
		} else if (isLAltKeyDown && isLAltAsLCtrl) {
			// Here you are OUT of the Alt+Tab switcher

			if (isVimShiftKeyDown) { // caps + lalt + shift
				sendCustomKeyDownEvent(SC_LSHIFT);
			}

			if (currentKeyCode == SC_V) { // caps + lalt + V
				pressUpLAlt();
				sendCustomKeyDownEvent(SC_LWIN, 2);
				sendCustomKeyEvent(SC_V);
				sendCustomKeyUpEvent(SC_LWIN, 3);
				pressDownLAltAsLCtrl();
			} if (isHCurrentKeyCode) {
				pressUpLAlt();
				sendCustomKeyEvent(SC_HOME);
				pressDownLAltAsLCtrl();
			} else if (isLCurrentKeyCode) {
				pressUpLAlt();
				sendCustomKeyEvent(SC_END);
				pressDownLAltAsLCtrl();
			} else if (isJCurrentKeyCode) {
				pressUpLAlt();
				sendCustomKeyDownEvent(SC_LCTRL);
				sendCustomKeyEvent(SC_END);
				sendCustomKeyUpEvent(SC_LCTRL);
				pressDownLAltAsLCtrl();
			} else if (isKCurrentKeyCode) {
				pressUpLAlt();
				sendCustomKeyDownEvent(SC_LCTRL);
				sendCustomKeyEvent(SC_HOME);
				sendCustomKeyUpEvent(SC_LCTRL);
				pressDownLAltAsLCtrl();
			}
		} else if (isVimShiftKeyDown) {
			// Here you are OUT of the Alt+Tab switcher
			sendCustomKeyDownEvent(SC_LSHIFT);

			if (isHCurrentKeyCode) {
				sendCustomKeyEvent(SC_LEFT);
			} else if (isLCurrentKeyCode) {
				sendCustomKeyEvent(SC_RIGHT);
			} else if (isJCurrentKeyCode) {
				sendCustomKeyEvent(SC_DOWN);
			} else if (isKCurrentKeyCode) {
				sendCustomKeyEvent(SC_UP);
			}
		} else if (isHCurrentKeyCode) {
			// Here you MAY in the Alt+Tab switcher
			sendCustomKeyEvent(SC_LEFT);
		} else if (isLCurrentKeyCode) {
			// Here you MAY in the Alt+Tab switcher
			sendCustomKeyEvent(SC_RIGHT);
		} else if (isJCurrentKeyCode) {
			// Here you MAY in the Alt+Tab switcher
			sendCustomKeyEvent(SC_DOWN);
		} else if (isKCurrentKeyCode) {
			// Here you MAY in the Alt+Tab switcher
			sendCustomKeyEvent(SC_UP);
		} else if (currentKeyCode == SC_SPACE) {
			pressDownLAltAsLCtrl();
			sendCustomKeyEvent(currentKeyCode);
			pressUpLAlt();
		}

		OutputDebugString(L"\nhandledCapslockKeyDown");
		return EVENT_HANDLED;
	} else {
		if (
			isCapslockCurrentKeyCode(keyStroke) || 
			(isCapslockKeyDown && currentKeyCode == shiftLetterForVimMode) ||
			(isCapslockKeyDown && (isShiftCurrentKeyCode(keyStroke)))
		) {
			if (isVimShiftKeyDown) {
				sendCustomKeyUpEvent(SC_LSHIFT);
			}
			isVimShiftKeyDown = false;
			isShiftLetterForVimModeKeyDown = false;

			OutputDebugString(L"\nhandledCapslockKeyUp");
			return EVENT_HANDLED;
		}
	}

	return 0;
}

int handleLCtrlKey(InterceptionKeyStroke keyStroke) {
	if (isKeyDown(keyStroke)) {
		if (!isLCtrlKeyDown) {
			return 0;
		}

		DWORD keyCode = keyStroke.code;

		if (keyCode == SC_LSHIFT) {
			sendCustomKeyDownEvent(SC_LSHIFT);
		} else if (keyCode == SC_TAB) {
			pressDownLCtrlAsLCtrl();
			sendCustomKeyEvent(SC_TAB);
		} else if (keyCode != SC_LCTRL) {
			pressDownLCtrlAsLAlt();
			sendCustomKeyEvent(keyCode);
		} else {
			pressDownLCtrlAsLAlt();
		}

		OutputDebugString(L"\nhandledLCtrlKeyDown");
		return EVENT_HANDLED;

	} else {
		if (isLCTrlCurrentKeyCode(keyStroke)) {
			pressUpLCtrl();
			OutputDebugString(L"\nhandledLCtrlKeyUp");
			return EVENT_HANDLED;
		} else if (isLCtrlKeyDown && isShiftCurrentKeyCode(keyStroke)) {
			OutputDebugString(L"\nhandledLCtrlKeyUp");
			sendCustomKeyUpEvent(SC_LSHIFT);
			return EVENT_HANDLED;
		} else if (isLCtrlKeyDown) {
			OutputDebugString(L"\nhandledLCtrlKeyUp");
			return EVENT_HANDLED;
		}
	}

	return 0;
}

int handleLWinKey(InterceptionKeyStroke keyStroke) {
	if (isKeyDown(keyStroke)) {
		if (!isLWinKeyDown) {
			return 0;
		}

		DWORD keyCode = keyStroke.code;

		if (isStarcraft2ActiveProcess()) {
			if (keyCode == SC_X) {
				sendCustomKeyDownEvent(SC_LALT);
				sendCustomKeyEvent(SC_X);
				sendCustomKeyUpEvent(SC_LALT);
				return EVENT_HANDLED;
			}
			
			if (keyCode == SC_1 || keyCode == SC_2 || keyCode == SC_3 || keyCode == SC_4) { // win + 1/2/3/4
				sendCustomKeyDownEvent(SC_LALT);
				sendCustomKeyEvent(keyCode);
				sendCustomKeyUpEvent(SC_LALT);
				return EVENT_HANDLED;
			}
		}

		if (keyCode == SC_H) { // win + h
			sendCustomKeyDownEvent(SC_LALT);
			sendCustomKeyEvent(SC_LEFT);
			sendCustomKeyUpEvent(SC_LALT);
			return EVENT_HANDLED;
		}
		if (keyCode == SC_L) { // win + l
			sendCustomKeyDownEvent(SC_LALT);
			sendCustomKeyEvent(SC_RIGHT);
			sendCustomKeyUpEvent(SC_LALT);
			return EVENT_HANDLED;
		}

		if (keyCode == SC_E) { // LWin + E
			// TODO:
		} else if (keyCode == SC_BACK) { // LWin + Backspace
			sendCustomKeyDownEvent(SC_LCTRL);
			sendCustomKeyEvent(keyCode);
			sendCustomKeyUpEvent(SC_LCTRL);
		} else if (keyCode == SC_D) { // LWin + D
			sendCustomKeyDownEvent(SC_LWIN, 2);
			sendCustomKeyEvent(keyCode);
			sendCustomKeyUpEvent(SC_LWIN, 3);
		} else if (keyCode == SC_SPACE) { // LWin + Space
			sendCustomKeyEvent(SC_LWIN, 2, 3);
		}

		OutputDebugString(L"\nhandledLWinKeyDown");
		return EVENT_HANDLED;
	} else {
		if (isLWinCurrentKeyCode(keyStroke)) {
			OutputDebugString(L"\nhandledLWinKeyUp");
			return EVENT_HANDLED;
		}
	}
	
	return 0;
}

int handleLAltKey(InterceptionKeyStroke keyStroke) {
	if (isKeyDown(keyStroke)) {
		if (!isLAltKeyDown) {
			return 0;
		}

		DWORD keyCode = keyStroke.code;

		if (isChromeActiveProcess()) {
			if (keyCode == SC_RETURN) { // alt + enter
				pressDownLAltAsLAlt();
				sendCustomKeyDownEvent(keyCode);
				pressDownLAltAsLCtrl();
				return EVENT_HANDLED;
			}

			if (keyCode == SC_H) { // alt + h
				sendCustomKeyDownEvent(SC_LSHIFT);
				sendCustomKeyEvent(SC_TAB);
				sendCustomKeyUpEvent(SC_LSHIFT);
				return EVENT_HANDLED;
			}

			if (keyCode == SC_L) { // alt + l
				sendCustomKeyEvent(SC_TAB);
				return EVENT_HANDLED;
			}

			if (keyCode == SC_SEMI) { // alt + ;
				sendCustomKeyEvent(SC_L);
				return EVENT_HANDLED;
			}
		}

		if (keyCode == SC_LSHIFT) {
			sendCustomKeyDownEvent(SC_LSHIFT);
		} else if (keyCode == SC_ESC && !isLAltAsLCtrl) { // alttabbed + esc
			sendCustomKeyEvent(SC_ESC);
			pressDownLAltAsLCtrl();
		} else if (keyCode == SC_ESC) { // alt + esc
			// DO NOTHING
		} else if (keyCode == SC_Q && !isLAltAsLCtrl) { // alt + tab + q
			sendCustomKeyEvent(SC_SUPR);
		} else if (keyCode == SC_Q) { // alt + q
			pressUpLAlt();
			pressDownLAltAsLAlt();
			sendCustomKeyEvent(SC_F4);
			pressDownLAltAsLCtrl();
		} else if (keyCode == SC_TAB) { // alt + tab
			pressDownLAltAsLAlt();
			sendCustomKeyEvent(SC_TAB);
		} else if (keyCode == SC_BACK) { // alt + backspace
			pressUpLAlt();
			sendCustomKeyDownEvent(SC_LSHIFT);
			sendCustomKeyEvent(SC_HOME);
			sendCustomKeyUpEvent(SC_LSHIFT);
			sendCustomKeyEvent(SC_BACK);
			pressDownLAltAsLCtrl();
		} else if (keyCode == SC_J && isShiftKeyDown) { // alt + shift + j
			pressUpLAlt();
			sendCustomKeyDownEvent(SC_LSHIFT);
			sendCustomKeyEvent(SC_NEXT);
			pressDownLAltAsLCtrl();
		} else if (keyCode == SC_K && isShiftKeyDown) { // alt + shift + k
			pressUpLAlt();
			sendCustomKeyDownEvent(SC_LSHIFT);
			sendCustomKeyEvent(SC_PRIOR);
			pressDownLAltAsLCtrl();
		} else if (keyCode == SC_J) { // alt + j
			pressUpLAlt();
			sendCustomKeyEvent(SC_NEXT);
			pressDownLAltAsLCtrl();
		} else if (keyCode == SC_K) { // alt + k
			pressUpLAlt();
			sendCustomKeyEvent(SC_PRIOR);
			pressDownLAltAsLCtrl();
		} else if (keyCode == SC_SPACE) { // alt + espace
			pressDownLAltAsLCtrl();
			sendCustomKeyEvent(SC_F12);
		} else if (
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
			pressUpLAlt();
			sendCustomKeyEvent(keyCode);
		} else if (keyCode != SC_LALT && !isLAltAsLCtrl) { // alttabbed + letter
			// DO NOTHING
		} else if (keyCode != SC_LALT) { // alt + letter
			pressDownLAltAsLCtrl();
			sendCustomKeyEvent(keyCode);
		} else {
			pressDownLAltAsLCtrl(); // alt
		}

		OutputDebugString(L"\nhandledLAltKeyDown");
		return EVENT_HANDLED;
	} else {
		if (isLAltCurrentKeyCode(keyStroke)) {
			pressUpLAlt();
			OutputDebugString(L"\nhandledLAltKeyUp");
			return EVENT_HANDLED;
		} else if (isLAltKeyDown && isShiftCurrentKeyCode(keyStroke)) {
			OutputDebugString(L"\nhandledLAltKeyUp");
			sendCustomKeyUpEvent(SC_LSHIFT);
			return EVENT_HANDLED;
		} else if (isLAltKeyDown) {
			OutputDebugString(L"\nhandledLAltKeyUp");
			return EVENT_HANDLED;
		}
	}

	return 0;
}

int handleShiftKey(InterceptionKeyStroke keyStroke) {
	if (isKeyDown(keyStroke)) {
		if (!isShiftKeyDown) {
			return 0;
		}

		DWORD keyCode = keyStroke.code;

		if (keyCode == SC_LSHIFT) {
			sendCustomKeyDownEvent(SC_LSHIFT);
		} else {
			sendCustomKeyDownEvent(SC_LSHIFT);
			sendCustomKeyDownEvent(keyCode);
		}

		OutputDebugString(L"\nhandledLShiftKeyDown");
		return EVENT_HANDLED;
	} else {
		if (isShiftCurrentKeyCode(keyStroke)) {
			// A shift keyup event is fired with a state 3 before the actual key released
			// is released. That's why I'm making sure that the state is different of 3 
			// in order to trigger the lshift properly
			if (keyStroke.state != 3) {
				sendCustomKeyUpEvent(SC_LSHIFT);
			}
			OutputDebugString(L"\nhandledLShiftKeyUp");
			return EVENT_HANDLED;
		} else if (isShiftKeyDown) {
			if (keyStroke.state != 3) {
				sendCustomKeyUpEvent(keyStroke.code);
			}
			OutputDebugString(L"\nhandledLShiftKeyUp");
			return EVENT_HANDLED;
		}
	}

	return 0;
}

int handleKey(InterceptionKeyStroke keyStroke) {
	if (isKeyDown(keyStroke)) {
		DWORD keyCode = keyStroke.code;

		if (isChromeActiveProcess()) {
			if (keyCode == SC_F3) { // f3
				sendCustomKeyDownEvent(SC_LCTRL);
				sendCustomKeyDownEvent(SC_LSHIFT);
				sendCustomKeyEvent(SC_TAB);
				sendCustomKeyUpEvent(SC_LSHIFT);
				sendCustomKeyUpEvent(SC_LCTRL);
				return EVENT_HANDLED;
			}
			if (keyCode == SC_F4) { // f4
				sendCustomKeyDownEvent(SC_LCTRL);
				sendCustomKeyEvent(SC_TAB);
				sendCustomKeyUpEvent(SC_LCTRL);
				EVENT_HANDLED;
			}
			if (keyCode == SC_F5) { // F5
				sendCustomKeyDownEvent(SC_LALT);
				sendCustomKeyEvent(SC_M);
				sendCustomKeyUpEvent(SC_LALT);
				return EVENT_HANDLED;
			}
			if (keyCode == SC_F6) { // f6
				sendCustomKeyDownEvent(SC_LALT);
				sendCustomKeyEvent(SC_T);
				sendCustomKeyUpEvent(SC_LALT);
				return EVENT_HANDLED;
			}
		}

		if (keyCode == SC_F10) {
			sendCustomKeyEvent(SC_MUTE, 2, 3);
			return EVENT_HANDLED;
		}
		if (keyCode == SC_F11) {
			sendCustomKeyEvent(SC_VOLUMEDOWN, 2, 3);
			return EVENT_HANDLED;
		}
		if (keyCode == SC_F12) {
			sendCustomKeyEvent(SC_VOLUMEUP, 2, 3);
			return EVENT_HANDLED;
		}
		if (keyCode == SC_F1) {
			BrightnessHandler::Increment(-10);
			return EVENT_HANDLED;
		}
		if (keyCode == SC_F2) {
			BrightnessHandler::Increment(10);
			return EVENT_HANDLED;
		}
		sendCustomKeyEvent(keyCode);
		OutputDebugString(L"\nhandledKeyDown");
		return EVENT_HANDLED;
	} else {
		//sendCustomKeyUpEvent(keyStroke.code);
		//OutputDebugString(L"\nhandledKeyUp");
		return EVENT_HANDLED;
	}

	return 0;
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

DWORD WINAPI keyboardThreadFunc(void* data) {
	raise_process_priority();
	context = interception_create_context();
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

	auto shiftClick = ErwinUtils::KeyClick<void(int)>(SC_LSHIFT, [](int consecutiveClicks) {
		if (consecutiveClicks > 1) {
			if ((GetKeyState(VK_CAPITAL) & 0x0001) == 0) { // Capslock OFF
				sendCustomKeyEvent(SC_CAPSLOCK);
			}
		} else {
			if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) { // Capslock ON
				sendCustomKeyEvent(SC_CAPSLOCK);
			}
		}
	});

	auto capslockClick = ErwinUtils::KeyClick<void(int)>(SC_CAPSLOCK, [](int consecutiveClicks) {
		if (consecutiveClicks == 1) {
			if (isLAltKeyDown && !isLAltAsLCtrl) { // altTabbed + esc
				sendCustomKeyEvent(SC_ESC);
				pressDownLAltAsLCtrl();
			} else if (isLAltKeyDown) { // alt + esc
				// DO NOTHING
			} else {
				sendCustomKeyEvent(SC_ESC);
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

		bool isCurrentKeyDown = isKeyDown(keyStroke);
		setKeyToKeyRemaps(keyStroke);

		DWORD keyCode = keyStroke.code;

		shiftClick.handleKeyStroke(keyCode, isCurrentKeyDown);
		capslockClick.handleKeyStroke(keyCode, isCurrentKeyDown);
		lAltClick.handleKeyStroke(keyCode, isCurrentKeyDown);

		if (isCurrentKeyDown) {
			if (isCapslockCurrentKeyCode(keyStroke)) {
				isCapslockKeyDown = true;
			} else if (isShiftCurrentKeyCode(keyStroke)) {
				isShiftKeyDown = true;
			} else if (isLCTrlCurrentKeyCode(keyStroke)) {
				isLCtrlKeyDown = true;
			} else if (isLWinCurrentKeyCode(keyStroke)) {
				isLWinKeyDown = true;
			} else if (isLAltCurrentKeyCode(keyStroke)) {
				isLAltKeyDown = true;
			}
		}

		if (handleSimulateMouseClick(keyStroke) == EVENT_HANDLED) {} 
		else if (handleLWinLAltKeys(keyStroke) == EVENT_HANDLED) {} 
		else if (handleCapslockKey(keyStroke) == EVENT_HANDLED) {} 
		else if (handleLCtrlKey(keyStroke) == EVENT_HANDLED) {} 
		else if (handleLWinKey(keyStroke) == EVENT_HANDLED) {} 
		else if (handleLAltKey(keyStroke) == EVENT_HANDLED) {} 
		else if (handleShiftKey(keyStroke) == EVENT_HANDLED) {} 
		else if (handleKey(keyStroke) == EVENT_HANDLED) {}

		if (!isCurrentKeyDown) {
			if (isCapslockCurrentKeyCode(keyStroke)) {
				isCapslockKeyDown = false;
			} else if (isShiftCurrentKeyCode(keyStroke)) {
				isShiftKeyDown = false;
			} else if (isLCTrlCurrentKeyCode(keyStroke)) {
				isLCtrlKeyDown = false;
			} else if (isLWinCurrentKeyCode(keyStroke)) {
				isLWinKeyDown = false;
			} else if (isLAltCurrentKeyCode(keyStroke)) {
				isLAltKeyDown = false;
			}
		}
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