#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <sstream>
#include <chrono>
#include <shellapi.h>
#include <vector>
#include "resource.h"
#include "interception.h"
#include "brightness.h"
#include "utils.h"
#include "erwinUtils.h"
#include "KeyEvent.h"

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

HHOOK hHook = 0;
HWINEVENTHOOK windowHook = 0;
const auto appTitle = L"Windows KeyRemapper";
DWORD globalDelayMSBetweenKeyEvents = 5;

bool isKeyForClickCurrentKeyCode;
int EVENT_HANDLED = 70;

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

Key getKeyFromKeyStroke(InterceptionKeyStroke keyStroke) {
	return Key(keyStroke.code, keyStroke.state);
}

void sendKeyEvents(std::vector<Key> keys) {
	auto keysSize = keys.size();

	for (int i = 0; i < keysSize; i++) {
		auto keyCode = keys[i].code;
		auto state = keys[i].state;

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
		} else if (keyCode == SC_BRIGHTNESSDOWN) {
			BrightnessHandler::Increment(-10);
		} else if (keyCode == SC_BRIGHTNESSUP) {
			BrightnessHandler::Increment(10);
		} else if (keyCode != SC_NULL) {
			unsigned short stateDown = 0;
			unsigned short stateUp = 1;

			switch (keyCode) {
				case SC_MUTE:
				case SC_VOLUMEDOWN:
				case SC_VOLUMEUP:
				case SC_LWIN:
					stateDown = 2;
					stateUp = 3;
					break;
			}

			if (state == 4) {
				interception_send(
					context,
					device,
					(InterceptionStroke *)&InterceptionKeyStroke({ keyCode, stateDown }),
					1
				);
				interception_send(
					context,
					device,
					(InterceptionStroke *)&InterceptionKeyStroke({ keyCode, stateUp }),
					1
				);

			} else {
				interception_send(
					context,
					device,
					(InterceptionStroke *)&InterceptionKeyStroke({ keyCode, state }),
					1
				);
			}
		}
	}
}

void CALLBACK handleWindowChange(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
	setActiveProcessName(ErwinUtils::getActiveWindowProcessName(hwnd));
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

// You have to phisically press a key to trigger
// the correct clean up
void handleAppCleanUp() {
	sendKeyEvents({
		KeyDown(SC_LALT),
		KeyUp(SC_LALT),
		KeyDown(SC_LCTRL),
		KeyUp(SC_LCTRL),
		KeyDown(SC_LWIN),
		KeyUp(SC_LWIN),
		KeyDown(SC_LSHIFT),
		KeyUp(SC_LSHIFT)
	});
}

DWORD WINAPI keyboardThreadFunc(void* data) {
	raise_process_priority();
	context = interception_create_context();
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

	auto shiftClick = ErwinUtils::KeyClick<void(int)>(SC_LSHIFT, [](int consecutiveClicks) {
		if (consecutiveClicks > 1) {
			if ((GetKeyState(VK_CAPITAL) & 0x0001) == 0) { // Capslock OFF
				sendKeyEvents({ KeyDown(SC_CAPSLOCK), KeyUp(SC_CAPSLOCK) });
			}
		} else {
			if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) { // Capslock ON
				sendKeyEvents({ KeyDown(SC_CAPSLOCK), KeyUp(SC_CAPSLOCK) });
			}
		}
	});

	auto capslockClick = ErwinUtils::KeyClick<void(int)>(SC_CAPSLOCK, [](int consecutiveClicks) {
		sendKeyEvents(getParsedKeysForEsc());
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
		commonKeyRemaps(keyStroke);

		auto key = getKeyFromKeyStroke(keyStroke);
		auto keyCode = key.code;

		bool isCurrentKeyDown = isKeyDown(key);
		shiftClick.handleKeyStroke(keyCode, isCurrentKeyDown);
		capslockClick.handleKeyStroke(keyCode, isCurrentKeyDown);
		lAltClick.handleKeyStroke(keyCode, isCurrentKeyDown);

		sendKeyEvents(getKeyEvents({ key }));
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