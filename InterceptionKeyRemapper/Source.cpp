#include <windows.h>
#include <sstream>
#include "interception.h"
#include "utils.h"
#include "erwinUtils.h"
#include <chrono>

enum ScanCodes {
	SC_C = 0x2E,
	SC_D = 0x20, 
	SC_E = 0x12,
	SC_H = 0x23,
	SC_J = 0x24,
	SC_K = 0x25,
	SC_L = 0x26,
	SC_Q = 0x10,
	SC_S = 0x1F,
	SC_X = 0x2D,
	SC_Y = 0x15,
	SC_ESC = 0x01,
	SC_CAPSLOCK = 0x3A,
	SC_LEFT = 0x4B,
	SC_RIGHT = 0x4D,
	SC_UP = 0x48,
	SC_DOWN = 0x50,
	SC_SPACE = 0x39,
	SC_LWIN = 0x5B,
	SC_LALT = 0x38,
	SC_RALT = 0xB8,
	SC_LCTRL = 0x1D,
	SC_RCTRL = 0x9D,
	SC_LSHIFT = 0x2A,
	SC_RSHIFT = 0x36,
	SC_LBSLASH = 0x56,
	SC_SUPR = 0x53,
	SC_BACK = 0x0E,
	SC_TAB = 0x0F,
	SC_HOME = 0x47,
	SC_END = 0x4F,
	SC_PRIOR = 0x49,
	SC_NEXT = 0x51,
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

bool isKeyForClickCurrentKeyCode;
int EVENT_HANDLED = 70;

bool isCapslockKeyDown;
bool isShiftKeyDown;
bool isLCtrlKeyDown;
bool isLWinKeyDown;
bool isLAltKeyDown;

void setKeyToKeyRemaps(InterceptionKeyStroke &keyStroke) {
	DWORD keyCode = keyStroke.code;
	if (keyCode == SC_LBSLASH || keyCode == SC_RSHIFT) {
		keyCode = SC_LSHIFT;
		keyStroke.code = keyCode;
	} else if (keyCode == SC_RALT) {
		keyCode = SC_LALT;
		keyStroke.code = keyCode;
	} else if (keyCode == SC_RCTRL) {
		keyCode = SC_RCTRL;
		keyStroke.code = keyCode;
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

int handleLWinLAltKeyDown(InterceptionKeyStroke keyStroke) {
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
			sendCustomKeyUpEvent(keyStroke.code);
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

			if (isVimShiftKeyDown) {
				sendCustomKeyDownEvent(SC_LSHIFT);
			}

			if (isHCurrentKeyCode) {
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

		OutputDebugString("\nhandledCapslockKeyDown");
		return EVENT_HANDLED;
	} else {
		if (
			isCapslockCurrentKeyCode(keyStroke) || 
			currentKeyCode == shiftLetterForVimMode ||
			(isCapslockKeyDown && (isShiftCurrentKeyCode(keyStroke)))
		) {
			if (isVimShiftKeyDown) {
				isVimShiftKeyDown = false;
				isShiftLetterForVimModeKeyDown = false;
				sendCustomKeyUpEvent(SC_LSHIFT);
			}

			OutputDebugString("\nhandledCapslockKeyUp");
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

		OutputDebugString("\nhandledLCtrlKeyDown");
		return EVENT_HANDLED;

	} else {
		if (isLCTrlCurrentKeyCode(keyStroke)) {
			pressUpLCtrl();
			OutputDebugString("\nhandledLCtrlKeyUp");
			return EVENT_HANDLED;
		} else if (isLCtrlKeyDown) {
			OutputDebugString("\nhandledLCtrlKeyUp");
			sendCustomKeyUpEvent(keyStroke.code);
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

		/*if (activeProcessName == "SC2_x64.exe") {
			if (keyCode == 0x58) { // win + x
				sendCustomKeyDownEvent(SC_LALT);
				sendCustomKeyEvent(0x58);
				sendCustomKeyUpEvent(SC_LALT);
				return EVENT_HANDLED;
			}

			if (keyCode == 0x31 || keyCode == 0x32 || keyCode == 0x33 || keyCode == 0x34) { // win + 1/2/3/4
				sendCustomKeyDownEvent(SC_LALT);
				sendCustomKeyEvent(keyCode);
				sendCustomKeyUpEvent(SC_LALT);
				return EVENT_HANDLED;
			}
		}*/

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

		OutputDebugString("\nhandledLWinKeyDown");
		return EVENT_HANDLED;
	} else {
		if (isLWinCurrentKeyCode(keyStroke)) {
			OutputDebugString("\nhandledLWinKeyUp");
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

		/*if (activeProcessName == "chrome.exe") {
			if (keyCode == VK_RETURN) { // alt + enter
				pressDownLAltAsLAlt();
				sendCustomKeyDownEvent(keyCode);
				pressDownLAltAsLCtrl();
				return EVENT_HANDLED;
			}

			if (keyCode == 0x48) { // alt + h
				sendCustomKeyDownEvent(SC_LSHIFT);
				sendCustomKeyEvent(SC_TAB);
				sendCustomKeyUpEvent(SC_LSHIFT);
				return EVENT_HANDLED;
			}

			if (keyCode == 0x4C) { // alt + l
				sendCustomKeyEvent(SC_TAB);
				return EVENT_HANDLED;
			}

			if (keyCode == VK_OEM_1) { // alt + ;
				sendCustomKeyEvent(0x4C);
				return EVENT_HANDLED;
			}
		}*/

		if (keyCode == SC_ESC && !isLAltAsLCtrl) { // alttabbed + esc
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
		} else if (keyCode == SC_SPACE) {
			pressDownLAltAsLCtrl();
			sendCustomKeyEvent(SC_F12);
		} else if (keyCode != SC_LALT && !isLAltAsLCtrl) { // alttabbed + letter
			// DO NOTHING
		} else if (keyCode != SC_LALT) { // alt + letter
			pressDownLAltAsLCtrl();
			sendCustomKeyDownEvent(keyCode);
		} else {
			pressDownLAltAsLCtrl(); // alt
		}

		OutputDebugString("\nhandledLAltKeyDown");
		return EVENT_HANDLED;
	} else {
		if (isLAltCurrentKeyCode(keyStroke)) {
			pressUpLAlt();
			OutputDebugString("\nhandledLAltKeyUp");
			return EVENT_HANDLED;
		} else if (isLAltKeyDown) {
			sendCustomKeyUpEvent(keyStroke.code);
			OutputDebugString("\nhandledLAltKeyUp");
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

		OutputDebugString("\nhandledLShiftKeyDown");
		return EVENT_HANDLED;
	} else {
		if (isShiftCurrentKeyCode(keyStroke)) {
			sendCustomKeyUpEvent(SC_LSHIFT);
			OutputDebugString("\nhandledLShiftKeyUp");
			return EVENT_HANDLED;
		} else if (isShiftKeyDown) {
			sendCustomKeyUpEvent(keyStroke.code);
			OutputDebugString("\nhandledLShiftKeyUp");
			return EVENT_HANDLED;
		}
	}

	return 0;
}

int handleKey(InterceptionKeyStroke keyStroke) {
	if (isKeyDown(keyStroke)) {
		DWORD keyCode = keyStroke.code;

		/*if (activeProcessName == "chrome.exe") {
			if (keyCode == VK_F3) { // f3
				sendCustomKeyDownEvent(SC_LCTRL);
				sendCustomKeyDownEvent(SC_LSHIFT);
				sendCustomKeyEvent(SC_TAB);
				sendCustomKeyUpEvent(SC_LSHIFT);
				sendCustomKeyUpEvent(SC_LCTRL);
				return EVENT_HANDLED;
			}
			if (keyCode == VK_F4) { // f4
				sendCustomKeyDownEvent(SC_LCTRL);
				sendCustomKeyEvent(SC_TAB);
				sendCustomKeyUpEvent(SC_LCTRL);
		Shifturn EVENT_HANDLED;
			}
			if (keyCode == VK_F5) { // F5
				sendCustomKeyDownEvent(SC_LALT);
				sendCustomKeyEvent(0x4D);
				sendCustomKeyUpEvent(SC_LALT);
				return EVENT_HANDLED;
			}
			if (keyCode == VK_F6) { // f6
				sendCustomKeyDownEvent(SC_LALT);
				sendCustomKeyEvent(0x54);
				sendCustomKeyUpEvent(SC_LALT);
				return EVENT_HANDLED;
			}
		}*/

		sendCustomKeyDownEvent(keyCode);
		OutputDebugString("\nhandledKeyDown");
		return EVENT_HANDLED;
	} else {
		sendCustomKeyUpEvent(keyStroke.code);
		OutputDebugString("\nhandledKeyUp");
		return EVENT_HANDLED;
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
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

	while (interception_receive(context, device = interception_wait(context), (InterceptionStroke *)&keyStroke, 1) > 0) {				
		bool isCurrentKeyDown = isKeyDown(keyStroke);
		setKeyToKeyRemaps(keyStroke);

		DWORD keyCode = keyStroke.code;

		shiftClick.handleKeyStroke(keyCode, isCurrentKeyDown);
		capslockClick.handleKeyStroke(keyCode, isCurrentKeyDown);

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
		else if (handleLWinLAltKeyDown(keyStroke) == EVENT_HANDLED) {}
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