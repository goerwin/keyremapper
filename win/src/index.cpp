// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-queryfullprocessimagenamea
#define _WIN32_WINNT 0x0600

#include <windows.h>

#include "../../common/Helpers.hpp"
#include "../../common/KeyRemapper.hpp"
#include "../../common/TestHelpers.hpp"
#include "../../common/vendors/json.hpp"
#include "./resources.h"
#include "./windowsBrightness.h"
#include "./windowsHelpers.hpp"
#include "./windowsLibraries/Interception/interception.h"
#include "./windowsLibraries/Interception/utils.h"

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

const auto APP_TITLE = L"KeyRemapper";
bool g_isAppEnabled = true;
int g_nCmdShow;
HINSTANCE g_hInstance;
auto g_mainDirPath = WindowsHelpers::getMainDirPath();
json g_symbolsJson;
json g_activeProfile;
json g_activeProfileIdx;
json g_profiles;
KeyRemapper *g_keyRemapper = NULL;
std::vector<std::tuple<ushort, ushort, ushort, ushort>>
    g_interceptionRemapCodesStates;
size_t g_interceptionRemapCodesStatesSize;

HWND g_systemTrayIconWindow;
HWND g_eventWindow;
Helpers::circular_buffer<std::string> g_EventsInfo(60);

const int IDM_EXIT = 5;
const int IDM_ENABLE = 6;
const int IDM_PROFILE_SELECTED = 20;  // 20 to 29 reserved for profiles
const int IDM_OPEN_EVENT_WINDOW = 15;
const int IDM_RUN_PROFILE_TESTS = 16;
const int IDM_COPY_LAST_5_INPUTS_TO_CLIPBOARD = 17;
const int IDM_OPEN_MAIN_FOLDER = 18;

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

void handleApplyKeysCb(std::string appName, std::string keyboard,
                       std::string keyboardDesc, std::string keys) {
  g_EventsInfo.push_back("");
  g_EventsInfo.push_back("Keys: " + keys);
  g_EventsInfo.push_back("Keyboard: " + keyboard);
  g_EventsInfo.push_back("App: " + appName);
  RedrawWindow(g_eventWindow, 0, 0, RDW_INVALIDATE | RDW_INTERNALPAINT);
}

void exitAppWithMessage(std::string title, std::string message) {
  WindowsHelpers::sendNotification(title, message);
  exit(0);
}

void initKeyRemapper(int profileIdx = 0) {
  try {
    auto configJsonPath = g_mainDirPath + "/config.json";
    auto symbolsJsonPath = g_mainDirPath + "/symbols.json";

    if (!WindowsHelpers::fileExists(configJsonPath) ||
        !WindowsHelpers::fileExists(configJsonPath))
      throw std::runtime_error(configJsonPath + " and " + symbolsJsonPath +
                               " required");

    auto configJson = Helpers::getJsonFile(configJsonPath);
    g_symbolsJson = Helpers::getJsonFile(symbolsJsonPath);

    if (configJson.is_null() || g_symbolsJson.is_null())
      throw std::runtime_error(configJsonPath + " or" + symbolsJsonPath +
                               " invalid");

    g_interceptionRemapCodesStates =
        g_symbolsJson["_interceptionRemapCodesStates"]
            .get<std::vector<std::tuple<ushort, ushort, ushort, ushort>>>();
    g_interceptionRemapCodesStatesSize = g_interceptionRemapCodesStates.size();

    g_profiles = configJson["profiles"].get<std::vector<json>>();
    g_activeProfile = g_profiles[profileIdx];
    g_activeProfileIdx = profileIdx;

    if (g_keyRemapper) delete g_keyRemapper;

    g_keyRemapper = new KeyRemapper(g_activeProfile, g_symbolsJson);
    g_keyRemapper->setApplyKeysCb(handleApplyKeysCb);
    g_keyRemapper->setAppName(WindowsHelpers::getActiveWindowProcessName());
  } catch (const std::runtime_error &err) {
    exitAppWithMessage("RuntimeError", err.what());
  } catch (const std::exception &err) {
    exitAppWithMessage("GeneralError", err.what());
  } catch (...) {
    exitAppWithMessage("GeneralError", "Unknown Error");
  }
}

void toggleAppEnabled() {
  g_isAppEnabled = !g_isAppEnabled;

  if (g_isAppEnabled) {
    nid.hIcon = iconImage;
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

  initKeyRemapper();

  while (interception_receive(context, device = interception_wait(context),
                              (InterceptionStroke *)&keyStroke, 1) > 0) {
    try {
      if (!g_isAppEnabled) {
        interception_send(context, device, (InterceptionStroke *)&keyStroke, 1);
        continue;
      }

      wchar_t hardwareId[500];
      size_t length = interception_get_hardware_id(context, device, hardwareId,
                                                   sizeof(hardwareId));
      if (length > 0 && length < sizeof(hardwareId)) {
        std::wstring ws(hardwareId);
        std::string hardwareIdStr(ws.begin(), ws.end());
        g_keyRemapper->setKeyboard(hardwareIdStr, "Not available");
      }

      ushort code = keyStroke.code;
      ushort state = keyStroke.state;

      // some keys send different scancode whether ctrl is pressed
      // eg. prtScr sends [55,2,3] but if ctrl is down, it will send [84,0,1]
      for (size_t i = 0; i < g_interceptionRemapCodesStatesSize; i++) {
        auto &remapCodeState = g_interceptionRemapCodesStates[i];
        auto secondCode = std::get<0>(remapCodeState);
        auto secondState = std::get<1>(remapCodeState);
        if (secondCode != code || secondState != state) continue;
        code = std::get<2>(remapCodeState);
        state = std::get<3>(remapCodeState);
        break;
      }

      if (code == 0) continue;

      auto keyEvents = g_keyRemapper->applyKeys({{"", code, state, false}});
      auto keyEventsSize = keyEvents.size();

      for (size_t i = 0; i < keyEventsSize; i++) {
        auto keyEvent = keyEvents[i];
        auto name = keyEvent.name;
        auto state = keyEvent.state;

        if (name == "SK:Delay") {
          std::this_thread::sleep_for(std::chrono::milliseconds(state));
          continue;
        }

        auto code = keyEvent.code;
        auto isKeyDown = keyEvent.isKeyDown;

        if (code == 241) {
          if (isKeyDown)
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
          else
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        } else if (code == 242) {
          if (isKeyDown)
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
          else
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
        } else if (code == 243 && isKeyDown)
          BrightnessHandler::Increment(-10);
        else if (code == 244 && isKeyDown)
          BrightnessHandler::Increment(10);
        else {
          auto newKeyStroke = InterceptionKeyStroke({code, state});
          interception_send(context, device,
                            (InterceptionStroke *)&newKeyStroke, 1);
        }
      }
    } catch (const std::runtime_error &err) {
      exitAppWithMessage("RuntimeError", err.what());
    } catch (const std::exception &err) {
      exitAppWithMessage("GeneralError", err.what());
    } catch (...) {
      exitAppWithMessage("GeneralError", "Unknown Error");
    }
  }

  interception_destroy_context(context);

  return 0;
}

void CALLBACK handleWindowChange(HWINEVENTHOOK hWinEventHook, DWORD dwEvent,
                                 HWND hwnd, LONG idObject, LONG idChild,
                                 DWORD dwEventThread, DWORD dwmsEventTime) {
  if (g_keyRemapper)
    g_keyRemapper->setAppName(WindowsHelpers::getActiveWindowProcessName());
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

      auto title =
          "inputCode:inputState -> remappedCode:remappedState -> "
          "parsedKeyEvent -> remappedKeyEvent -> keyEventsSent";
      auto remapInfoSize = g_EventsInfo.size();

      TextOutA(hdc, 0, 10, title, strlen(title));

      for (auto i = 0; i < remapInfoSize; i++) {
        auto size = g_EventsInfo[remapInfoSize - 1 - i].size();
        TextOutA(hdc, 0, i * 20 + 60,
                 g_EventsInfo[remapInfoSize - 1 - i].c_str(), size);
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
      CreateWindowEx(0,                    // Optional window styles.
                     CLASS_NAME,           // Window class
                     APP_TITLE,            // Window text
                     WS_OVERLAPPEDWINDOW,  // Window style

                     // position and size
                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                     NULL,       // Parent window
                     NULL,       // Menu
                     hInstance,  // Instance handle
                     NULL        // Additional application data
      );

  return hwnd;
}

void copyToClipboard(HWND hwnd, const std::string &s) {
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
          InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING,
                     IDM_RUN_PROFILE_TESTS, L"Run profile tests");
          InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING,
                     IDM_COPY_LAST_5_INPUTS_TO_CLIPBOARD,
                     L"Copy latest Events to Clipboard");
          InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING,
                     IDM_OPEN_EVENT_WINDOW, L"Open Events Window");
          InsertMenu(hPopMenu, 0, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);

          try {
            auto profiles = g_profiles.get<std::vector<json>>();
            auto profilesSize = profiles.size();

            // Max 10 profiles
            for (int i = 0; i < profilesSize && i <= 9; i++) {
              std::string newProfileName = "profile " + std::to_string(i + 1);
              auto profileName = profiles[i]["name"];

              if (profileName.is_string()) newProfileName = profileName;
              if (g_activeProfileIdx == i)
                newProfileName = "# " + newProfileName;
              std::wstring newProfileNameW =
                  std::wstring(newProfileName.begin(), newProfileName.end());
              LPCWSTR profileNameW = newProfileNameW.c_str();

              InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING,
                         IDM_PROFILE_SELECTED + i, profileNameW);
            }
          } catch (...) {
            exitAppWithMessage("Error",
                               "Error reading the config.json profiles");
          }

          InsertMenu(hPopMenu, 0, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
          InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING,
                     IDM_OPEN_MAIN_FOLDER, L"Open Config Folder");
          InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_ENABLE,
                     L"On/Off");
          TrackPopupMenu(hPopMenu,
                         TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pt.x,
                         pt.y, 0, hWnd, NULL);
          return 0;
      };
    case WM_COMMAND:
      auto command = LOWORD(wParam);

      if (command >= IDM_PROFILE_SELECTED &&
          command < IDM_PROFILE_SELECTED + 10) {
        initKeyRemapper(command - IDM_PROFILE_SELECTED);
        return 0;
      }
      switch (command) {
        case IDM_EXIT:
          exit(0);
          return 0;
        case IDM_ENABLE:
          toggleAppEnabled();
          return 0;
        case IDM_RUN_PROFILE_TESTS: {
          try {
            auto a = g_profiles;
            auto b = g_symbolsJson;
            auto tests = g_activeProfile["tests"];
            auto testResults = TestHelpers::runTests(
                g_activeProfile["tests"], g_activeProfile, g_symbolsJson);
            std::string testResultsStr =
                testResults.is_null() ? "NO TESTS RUN" : testResults["message"];

            WindowsHelpers::sendNotification("Tests Results", testResultsStr);
          } catch (...) {
            WindowsHelpers::sendNotification(
                "Error", "There were some errors running the tests");
          }
          return 0;
        }
        case IDM_COPY_LAST_5_INPUTS_TO_CLIPBOARD: {
          auto remapInfoSize = g_EventsInfo.size();

          std::string str = "";

          for (auto i = 0, max = 0; i < remapInfoSize && i < 20; i++)
            str = str + g_EventsInfo[remapInfoSize - 1 - i] + "\n";

          copyToClipboard(g_systemTrayIconWindow, str);
          WindowsHelpers::sendNotification("Clipboard",
                                           "Events copied to clipboard!");

          return 0;
        }
        case IDM_OPEN_EVENT_WINDOW:
          SwitchToThisWindow(g_eventWindow, false);
          g_eventWindow = createEventWindow(g_hInstance);
          ShowWindow(g_eventWindow, g_nCmdShow);
          return 0;
        case IDM_OPEN_MAIN_FOLDER:
          ShellExecuteA(NULL, "open", g_mainDirPath.c_str(), NULL, NULL,
                        SW_SHOWDEFAULT);
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
      CreateWindowEx(0,                    // Optional window styles.
                     CLASS_NAME,           // Window class
                     APP_TITLE,            // Window text
                     WS_OVERLAPPEDWINDOW,  // Window style
                     // Size and position
                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                     NULL,       // Parent window
                     NULL,       // Menu
                     hInstance,  // Instance handle
                     NULL        // Additional application data
      );

  return hWnd;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszArgument, int nCmdShow) {
  std::atexit(handleAppExit);
  g_nCmdShow = nCmdShow;
  g_hInstance = hInstance;

  g_systemTrayIconWindow = createSystemTrayIconWindow(hInstance);

  if (!g_systemTrayIconWindow) return 0;

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
