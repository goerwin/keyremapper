// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-queryfullprocessimagenamea
#define _WIN32_WINNT 0x0600

#include <windows.h>
#include "./windowsBrightness.h"
#include "./windowsHelpers.hpp"
#include "./windowsImages.h"
#include "./windowsLibraries/Interception/interception.h"
#include "./windowsLibraries/Interception/utils.h"
#include "../../common/vendors/json.hpp"
#include "../../common/Helpers.hpp"
#include "../../common/TestHelpers.hpp"
#include "../../common/KeyDispatcher.hpp"

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

const auto APP_TITLE = L"KeyRemapper";
bool g_isAppEnabled = true;
int g_mode;
int g_nCmdShow;
HINSTANCE g_hInstance;
std::string g_appName;
KeyDispatcher *g_keyDispatcher;
nlohmann::json g_rules;
nlohmann::json g_symbols;

HWND g_systemTrayIconWindow;
HWND g_eventWindow;
Helpers::circular_buffer<std::string> g_EventsInfo(60);

const int IDM_EXIT = 5;
const int IDM_ENABLE = 6;
const int IDM_MODE_1 = 11;
const int IDM_MODE_2 = 12;
const int IDM_MODE_3 = 13;
const int IDM_MODE_4 = 14;
const int IDM_OPEN_EVENT_WINDOW = 15;
const int IDM_RUN_MODE_TESTS = 16;
const int IDM_COPY_LAST_5_INPUTS_TO_CLIPBOARD = 17;

// https://codingmisadventures.wordpress.com/2009/02/20/creating-a-system-tray-icon-using-visual-c-and-win32/
NOTIFYICONDATA nid;
#define WM_MYMESSAGE (WM_USER + 1)
auto iconImage =
    HICON(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_DEFAULT),
                    IMAGE_ICON, 0, 0, 0));
auto iconImageDisabled = HICON(LoadImage(
    GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_RED), IMAGE_ICON, 0, 0, 0));

void createSystemTrayIcon(HINSTANCE hInstance, HWND hWnd) {
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd = hWnd;
  nid.uID = 100;
  nid.uVersion = NOTIFYICON_VERSION;
  nid.uCallbackMessage = WM_MYMESSAGE;
  nid.hIcon = iconImage;
  wcscpy_s(nid.szTip, APP_TITLE);
  nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  Shell_NotifyIcon(NIM_ADD, &nid);
}

void handleApplyKeysCb(std::string appName, std::string keyboard, std::string keyboardDesc, std::string keys) {
  g_EventsInfo.push_back("");
  g_EventsInfo.push_back("Keys: " + keys);
  g_EventsInfo.push_back("Keyboard: " + keyboard);
  g_EventsInfo.push_back("App: " + appName);
  RedrawWindow(g_eventWindow, 0, 0, RDW_INVALIDATE | RDW_INTERNALPAINT);
}

void initializeKeyDispatcher(int mode = 0) {
  std::vector<std::string> modes = {"mode1.json", "mode2.json", "mode3.json", "mode4.json"};

  auto absDirPath = WindowsHelpers::getAbsDirPath();

  if (WindowsHelpers::fileExists(absDirPath + "\\" + modes[mode])) g_mode = mode;
  else g_mode = 0;

  g_rules = Helpers::getJsonFile(absDirPath, modes[g_mode]);
  g_symbols = Helpers::getJsonFile(absDirPath, "symbols.json");

  delete g_keyDispatcher;
  g_keyDispatcher = new KeyDispatcher(g_rules, g_symbols);
  g_keyDispatcher->setApplyKeysCb(handleApplyKeysCb);
  g_keyDispatcher->setAppName(g_appName);
}

void toggleAppEnabled() {
  g_isAppEnabled = !g_isAppEnabled;

  if (g_isAppEnabled) {
    nid.hIcon = iconImage;
    initializeKeyDispatcher();
  } else {
    nid.hIcon = iconImageDisabled;
  }

  Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void handleAppExit() { Shell_NotifyIcon(NIM_DELETE, &nid); }

DWORD WINAPI keyboardThreadFunc(void *data) {
  raise_process_priority();
  context = interception_create_context();
  interception_set_filter(context, interception_is_keyboard,
                          INTERCEPTION_FILTER_KEY_ALL);

  initializeKeyDispatcher();

  while (interception_receive(context, device = interception_wait(context),
                              (InterceptionStroke *)&keyStroke, 1) > 0) {
    wchar_t hardwareId[500];
    size_t length = interception_get_hardware_id(context, device, hardwareId, sizeof(hardwareId));
    if (length > 0 && length < sizeof(hardwareId)) {
      std::wstring ws(hardwareId);
      std::string hardwareIdStr(ws.begin(), ws.end());
      g_keyDispatcher->setKeyboard(hardwareIdStr, "Not available");
    }

    auto code = keyStroke.code;
    auto state = keyStroke.state;
    auto newKeys = g_keyDispatcher->applyKeys({{code, state}});
    auto newKeysSize = newKeys.size();

    if (!g_isAppEnabled) {
      interception_send(context, device, (InterceptionStroke *)&keyStroke, 1);
    }

    // "FakeShiftL": [42, 2, 3],
    // Discard FakeShiftL events
    if (code == 42 && (state == 2 || state == 3)) continue;

    for (size_t i = 0; i < newKeysSize; i++) {
      auto [code, state] = newKeys[i];
      auto newKeyStroke = InterceptionKeyStroke({code, state});

      if (code == 245) {
        toggleAppEnabled();
        break;
      } else if (!g_isAppEnabled) {
        continue;
      } else if (code == 241) {
        if (state == 0)
          mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        else
          mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
      } else if (code == 242) {
        if (state == 0)
          mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
        else
          mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
      } else if (code == 243 && state == 0) {
        BrightnessHandler::Increment(-10);
      } else if (code == 244 && state == 0) {
        BrightnessHandler::Increment(10);
      } else if (code == 246 && state == 1) {
        initializeKeyDispatcher();
      } else if (code == 247 && state == 1) {
        initializeKeyDispatcher(1);
      } else if (code == 248 && state == 1) {
        initializeKeyDispatcher(2);
      } else if (code == 249 && state == 1) {
        initializeKeyDispatcher(3);
      } else {
        interception_send(context, device, (InterceptionStroke *)&newKeyStroke,
                          1);
      }
    }
  }

  interception_destroy_context(context);

  return 0;
}

void CALLBACK handleWindowChange(HWINEVENTHOOK hWinEventHook, DWORD dwEvent,
                                 HWND hwnd, LONG idObject, LONG idChild,
                                 DWORD dwEventThread, DWORD dwmsEventTime) {
    auto foregroundWindow = GetForegroundWindow();
    g_appName = WindowsHelpers::getActiveWindowProcessName(foregroundWindow);
    g_keyDispatcher->setAppName(g_appName);
}

LRESULT CALLBACK eventWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                 LPARAM lParam) {
  switch (uMsg) {
  case WM_PAINT:
    PAINTSTRUCT ps;
    // All painting occurs between BeginPaint and EndPaint.
    HDC hdc = BeginPaint(hWnd, &ps);
    FillRect(hdc, &ps.rcPaint, CreateSolidBrush(0x00000000));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, 0x00DDDDDD);

    auto title = "originalCode:originalState -> keyEvent -> remappedKeyEvent "
                 "-> transformedKeyEvents";
    auto remapInfoSize = g_EventsInfo.size();

    TextOutA(hdc, 0, 10, title, strlen(title));

    for (auto i = 0; i < remapInfoSize; i++) {
      auto size = g_EventsInfo[remapInfoSize - 1 - i].size();
      TextOutA(hdc, 0, i * 20 + 60, g_EventsInfo[remapInfoSize - 1 - i].c_str(),
               size);
    }

    EndPaint(hWnd, &ps);
    return 0;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND createEventWindow(HINSTANCE hInstance) {
  // Register the window class.
  const wchar_t CLASS_NAME[] = L"Event Window Class";

  WNDCLASS wc = {};

  wc.lpfnWndProc = eventWindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  wc.hbrBackground = CreateSolidBrush(0x00000000);

  RegisterClass(&wc);

  // Create the window.

  HWND hwnd =
      CreateWindowEx(0,                   // Optional window styles.
                     CLASS_NAME,          // Window class
                     APP_TITLE,           // Window text
                     WS_OVERLAPPEDWINDOW, // Window style

                     // position and size
                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                     NULL,      // Parent window
                     NULL,      // Menu
                     hInstance, // Instance handle
                     NULL       // Additional application data
      );

  return hwnd;
}

void copyToClipboard(HWND hwnd, const std::string& s) {
  OpenClipboard(hwnd);
  EmptyClipboard();
  HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
  if (!hg) {
    CloseClipboard();
    return;
  }
  memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
  GlobalUnlock(hg);
  SetClipboardData(CF_TEXT, hg);
  CloseClipboard();
  GlobalFree(hg);
}

LRESULT CALLBACK systemTrayWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam) {
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_MYMESSAGE:
    switch (lParam) {
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
      POINT pt;
      GetCursorPos(&pt);
      HMENU hPopMenu = CreatePopupMenu();
      SetForegroundWindow(hWnd);

      InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit");
      InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_RUN_MODE_TESTS,
                 L"Run mode tests");
      InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_COPY_LAST_5_INPUTS_TO_CLIPBOARD,
                 L"Copy latest Events to Clipboard");
      InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_OPEN_EVENT_WINDOW,
                 L"Open Events Window");
      InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_4,
                 g_mode == 3 ? L"# Mode 4" : L"Mode 4");
      InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_3,
                 g_mode == 2 ? L"# Mode 3" : L"Mode 3");
      InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_2,
                 g_mode == 1 ? L"# Mode 2" : L"Mode 2");
      InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_1,
                 g_mode == 0 ? L"# Mode 1" : L"Mode 1");
      InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_ENABLE, L"On/Off");
      TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN,
                     pt.x, pt.y, 0, hWnd, NULL);
      return 0;
    };
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDM_EXIT:
      exit(0);
      return 0;
    case IDM_ENABLE:
      toggleAppEnabled();
      return 0;
    case IDM_MODE_1:
      initializeKeyDispatcher();
      return 0;
    case IDM_MODE_2:
      initializeKeyDispatcher(1);
      return 0;
    case IDM_MODE_3:
      initializeKeyDispatcher(2);
      return 0;
    case IDM_MODE_4:
      initializeKeyDispatcher(3);
      return 0;
    case IDM_RUN_MODE_TESTS: {
      auto testResults = TestHelpers::runTests(g_rules["tests"], g_rules, g_symbols);
      std::string testResultsStr =
          testResults.is_null() ? "NO TESTS RUN" : testResults["message"];

      MessageBoxA(NULL, testResultsStr.c_str(), "Tests Results",
                  MB_OK | MB_ICONINFORMATION);
      return 0;
    }
    case IDM_COPY_LAST_5_INPUTS_TO_CLIPBOARD: {
      auto remapInfoSize = g_EventsInfo.size();
      std::string str = "";
      for (auto i = 0; i < remapInfoSize && i < 20; i++)
        str = str + g_EventsInfo[i] + "\n";

      copyToClipboard(g_systemTrayIconWindow, str);
      MessageBoxA(NULL, "Events copied to clipboard!", "Copy to Clipboard",
        MB_OK | MB_ICONINFORMATION);
      return 0;
    }
    case IDM_OPEN_EVENT_WINDOW:
      SwitchToThisWindow(g_eventWindow, false);
      g_eventWindow = createEventWindow(g_hInstance);
      ShowWindow(g_eventWindow, g_nCmdShow);
      return 0;
    }

    return 0;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND createSystemTrayIconWindow(HINSTANCE hInstance) {
  // Register the window class.
  const wchar_t CLASS_NAME[] = L"WinClass";
  WNDCLASS wc = {};
  wc.lpfnWndProc = systemTrayWindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);
  HWND hWnd =
      CreateWindowEx(0,                   // Optional window styles.
                     CLASS_NAME,          // Window class
                     APP_TITLE,           // Window text
                     WS_OVERLAPPEDWINDOW, // Window style
                     // Size and position
                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                     NULL,      // Parent window
                     NULL,      // Menu
                     hInstance, // Instance handle
                     NULL       // Additional application data
      );

  return hWnd;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszArgument, int nCmdShow) {
  std::atexit(handleAppExit);
  g_nCmdShow = nCmdShow;
  g_hInstance = hInstance;

  g_systemTrayIconWindow = createSystemTrayIconWindow(hInstance);

  if (!g_systemTrayIconWindow)
    return 0;

  SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL,
                  handleWindowChange, 0, 0, WINEVENT_OUTOFCONTEXT);

  createSystemTrayIcon(hInstance, g_systemTrayIconWindow);

  CreateThread(NULL, 0, keyboardThreadFunc, NULL, 0, NULL);

  MSG messages;
  while (GetMessage(&messages, NULL, 0, 0)) {
    TranslateMessage(&messages);
    DispatchMessage(&messages);
  }

  return messages.wParam;
}
