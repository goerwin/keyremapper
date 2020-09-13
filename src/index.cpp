// #ifndef UNICODE
// #define UNICODE
// #endif

#include <windows.h>
#include <sstream>
#include <chrono>
#include <shellapi.h>
#include <vector>
#include "images/index.h"
#include "helpers/brightness.h"
#include "helpers/KeyEvent.h"
#include "helpers/utils.h"
#include "libraries/Interception/utils2.h"
#include "libraries/Interception/interception.h"

void forceNumLockStatusOn();
void updateNumLockStatusInTrayIcon();

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

HHOOK hHook = 0;
HWND g_hwnd;
HWINEVENTHOOK windowHook = 0;
const auto appTitle = L"Windows KeyRemapper";

bool isKeyForClickCurrentKeyCode;
int EVENT_HANDLED = 70;

bool fileExists(String path) {
	auto dwAttrib = GetFileAttributesA(path.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void deleteActiveModeFiles() {
	remove("./hotKeys/_mode1Active");
	remove("./hotKeys/_mode2Active");
	remove("./hotKeys/_mode3Active");
	remove("./hotKeys/_mode4Active");
}

int getActiveMode() {
	if (fileExists("./hotKeys/_mode1Active")) return SC_MODE1;
	if (fileExists("./hotKeys/_mode2Active")) return SC_MODE2;
	if (fileExists("./hotKeys/_mode3Active")) return SC_MODE3;
	if (fileExists("./hotKeys/_mode4Active")) return SC_MODE4;
	return 0;
}

unsigned short g_activeMode;
LPWSTR getModeMenuText(unsigned short mode) {
	std::wstring modeText;

	switch (mode) {
	case SC_MODE1:
		return g_activeMode == mode ? LPWSTR(L"# Mode 1") : LPWSTR(L"Mode 1");
	case SC_MODE2:
		return g_activeMode == mode ? LPWSTR(L"# Mode 2") : LPWSTR(L"Mode 2");
	case SC_MODE3:
		return g_activeMode == mode ? LPWSTR(L"# Mode 3") : LPWSTR(L"Mode 3");
	case SC_MODE4:
		return g_activeMode == mode ? LPWSTR(L"# Mode 4") : LPWSTR(L"Mode 4");
	}

	return LPWSTR("No mode");
}

void setActiveMode(unsigned short mode) {
	deleteActiveModeFiles();
	g_activeMode = mode;

	switch (mode) {
	case SC_MODE1:
		KeyEvent::setCustomHotKeysFromFile("./hotKeys/mode1.md");
		Utils::writeToFile("./hotKeys/_mode1Active", "");
		break;
	case SC_MODE2:
		KeyEvent::setCustomHotKeysFromFile("./hotKeys/mode2.md");
		Utils::writeToFile("./hotKeys/_mode2Active", "");
		break;
	case SC_MODE3:
		KeyEvent::setCustomHotKeysFromFile("./hotKeys/mode3.md");
		Utils::writeToFile("./hotKeys/_mode3Active", "");
		break;
	case SC_MODE4:
		KeyEvent::setCustomHotKeysFromFile("./hotKeys/mode4.md");
		Utils::writeToFile("./hotKeys/_mode4Active", "");
		break;
	}
}

void defaultKeyRemaps(InterceptionKeyStroke &keyStroke) {
	auto keyCode = keyStroke.code;
	auto keyState = keyStroke.state;

	if (keyCode == SC_LBSLASH || keyCode == SC_RSHIFT) {
		keyStroke.code = SC_LSHIFT;
	} else if (keyCode == SC_RWIN) {
		keyStroke.code = SC_LWIN;
	} else if (keyCode == SC_LCTRL && (keyState == 2 || keyState == 3)) {
		keyStroke.code = SC_LWIN;
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

		if (keyCode == SC_NULL) {
			continue;
		}

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
		} else if (keyCode == SC_MODE1 || keyCode == SC_MODE2 || keyCode == SC_MODE3 || keyCode == SC_MODE4) {
			setActiveMode(keyCode);
		} else {
			unsigned short stateDown = 0;
			unsigned short stateUp = 1;

			switch (keyCode) {
				case SC_MUTE:
				case SC_VOLUMEDOWN:
				case SC_VOLUMEUP:
				case SC_LWIN:
				case SC_LEFT:
				case SC_RIGHT:
				case SC_UP:
				case SC_DOWN:
				case SC_PRIOR:
				case SC_NEXT:
				case SC_HOME:
				case SC_END:
				case SC_INSERT:
				case SC_DELETE:
					stateDown = 2;
					stateUp = 3;

					if (state == 0) {
						state = stateDown;
					} else if (state == 1) {
						state = stateUp;
					}

					break;
				case SC_NUMLOCK:
					if (state == stateUp) {
						updateNumLockStatusInTrayIcon();
					}

					break;
				case SC_NP2:
				case SC_NP4:
				case SC_NP6:
				case SC_NP8:
					if (state == 0 || state == 1) {
						if (keyCode != SC_NP4) {
							if (state == 0) state = 2;
							else state = 3;
							keyCode = SC_LEFT;
						}
					}
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
				if (keyCode == SC_LSHIFT && (state == 0 || state == 1)) {
					keyCode = keyCode;
					//continue;
				}

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
	KeyEvent::setActiveProcessName(Utils::getActiveWindowProcessName(hwnd));
}

void openCustomHotKeysFile() {
	OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	//LPWSTR szFile;       // buffer for file name

	HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hwnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	//ofn.lpstrFilter = "All\0*.*\0MD\0*.md\0";
	ofn.lpstrFilter = "MarkDown Files (*.md)\0*.md\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	// Display the Open dialog box.

	if (GetOpenFileNameA(&ofn) == TRUE) {
		KeyEvent::setCustomHotKeysFromFile(ofn.lpstrFile);
	}
}

bool isAppEnabled = true;
NOTIFYICONDATA nid;
const auto ctxMenuEnabled = L"Disable";
const auto ctxLoadHotKeysFile = L"Load HotKeys file";
const auto ctxMenuNotEnabled = L"Enable";
auto ctxMenuEnabledMsg = ctxMenuEnabled;
auto globalIconImage = HICON(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_DEFAULT), IMAGE_ICON, 0, 0, 0));
//auto globalIconImageDisabled = HICON(LoadImage(NULL, TEXT("images/letter-e-red.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
auto globalIconImageDisabled = HICON(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_RED), IMAGE_ICON, 0, 0, 0));
auto globalIconImageWarning = HICON(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_WARNING), IMAGE_ICON, 0, 0, 0));

void toggleAppEnabled() {
	isAppEnabled = !isAppEnabled;
	ctxMenuEnabledMsg = isAppEnabled ? ctxMenuEnabled : ctxMenuNotEnabled;
	nid.hIcon = isAppEnabled ? globalIconImage : globalIconImageDisabled;
	if (isAppEnabled) forceNumLockStatusOn();
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void updateNumLockStatusInTrayIcon() {
	Sleep(100);
	if ((GetKeyState(VK_NUMLOCK) & 0x0001) == 0) { // NumLock OFF
		nid.hIcon = globalIconImageWarning;
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	} else {
		nid.hIcon = isAppEnabled ? globalIconImage : globalIconImageDisabled;
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}
}

void forceNumLockStatusOn() {
	if ((GetKeyState(VK_NUMLOCK) & 0x0001) == 0) { // NumLock OFF
		sendKeyEvents({ KeyDown(SC_NUMLOCK), KeyUp(SC_NUMLOCK) });
	} else {
		updateNumLockStatusInTrayIcon();
	}
}


#define WM_MYMESSAGE (WM_USER + 1)
const int IDM_EXIT = 5;
const int IDM_ENABLE = 6;
const int IDM_HOTKEYS_FOLDER = 8;
const int IDM_MODE_1 = 11;
const int IDM_MODE_2 = 12;
const int IDM_MODE_3 = 13;
const int IDM_MODE_4 = 14;
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
				InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_4, getModeMenuText(SC_MODE4));
				InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_3, getModeMenuText(SC_MODE3));
				InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_2, getModeMenuText(SC_MODE2));
				InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_1, getModeMenuText(SC_MODE1));
				InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_HOTKEYS_FOLDER, L"Open HotKeys Folder");
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
				case IDM_MODE_1:
					setActiveMode(SC_MODE1);
					return 0;
				case IDM_MODE_2:
					setActiveMode(SC_MODE2);
					return 0;
				case IDM_MODE_3:
					setActiveMode(SC_MODE3);
					return 0;
				case IDM_MODE_4:
					setActiveMode(SC_MODE4);
					return 0;
				case IDM_HOTKEYS_FOLDER:
					char moduleFilepath[MAX_PATH];
					GetModuleFileNameA(NULL, moduleFilepath, MAX_PATH);
					auto strModuleFilepath = String(moduleFilepath);
					auto pos = strModuleFilepath.find_last_of("\\");

					ShellExecuteA(NULL, "open", strModuleFilepath.substr(0, pos).c_str(), NULL, NULL, SW_SHOWDEFAULT);
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
void handleAppRestart() {
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

	forceNumLockStatusOn();
}

DWORD WINAPI keyboardThreadFunc(void* data) {
	raise_process_priority();
	context = interception_create_context();
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

	auto escClick = Utils::KeyClick<void(int)>(SC_ESC, [](int consecutiveClicks) {
		if (consecutiveClicks > 1) {
			toggleAppEnabled();
		}
	});

	auto shiftClick = Utils::KeyClick<void(int)>(SC_LSHIFT, [](int consecutiveClicks) {
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

	auto capslockClick = Utils::KeyClick<void(int)>(SC_CAPSLOCK, [](int consecutiveClicks) {
		sendKeyEvents(KeyEvent::getParsedKeysForEsc());
	});

	auto lAltClick = Utils::KeyClick<void(int)>(SC_LALT, [](int consecutiveClicks) {
		if (consecutiveClicks == 2) {
			handleAppRestart();
		}
	});

	while (interception_receive(
		context,
		device = interception_wait(context),
		(InterceptionStroke *)&keyStroke,
		1
	) > 0) {
		auto key = getKeyFromKeyStroke(keyStroke);
		auto keyCode = key.code;
		bool isKeyDown = KeyEvent::isKeyDown(key);
		escClick.handleKeyStroke(keyCode, isKeyDown);

		if (!isAppEnabled) {
			OutputDebugStringA("\n");
			OutputDebugStringA(std::to_string(key.code).c_str());
			OutputDebugStringA(" ");
			OutputDebugStringA(std::to_string(key.state).c_str());
			interception_send(context, device, (InterceptionStroke *)&keyStroke, 1);
			continue;
		}

		defaultKeyRemaps(keyStroke);
		key = getKeyFromKeyStroke(keyStroke);
		keyCode = key.code;
		shiftClick.handleKeyStroke(keyCode, isKeyDown);
		capslockClick.handleKeyStroke(keyCode, isKeyDown);
		lAltClick.handleKeyStroke(keyCode, isKeyDown);

    Keys newKeys = KeyEvent::getKeyEvents({ key });
		sendKeyEvents(newKeys);
	}

	interception_destroy_context(context);

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
	std::atexit(handleAppExit);

	// SYSTEM TRAY ICON

	g_hwnd = createWindow(hInstance);

	if (!g_hwnd) {
		return 0;
	}

	handleSystemTrayIcon(hInstance, g_hwnd);

	// WINDOW CHANGE EVENT

	windowHook = SetWinEventHook(
		EVENT_SYSTEM_FOREGROUND,
		EVENT_SYSTEM_FOREGROUND,
		NULL,
		handleWindowChange,
		0, 0,
		WINEVENT_OUTOFCONTEXT
	);

	// INITIALIZATIONS

	KeyEvent::initialize();
	setActiveMode(getActiveMode());
	forceNumLockStatusOn(); // Not working properly

	// KEYBOARD

	CreateThread(NULL, 0, keyboardThreadFunc, NULL, 0, NULL);

	MSG messages;
	while (GetMessage(&messages, NULL, 0, 0)) {
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	return messages.wParam;
}
