// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-queryfullprocessimagenamea
#define _WIN32_WINNT 0x0600

#include <windows.h>
#include "images/index.h"
#include "images/index.h"
#include "helpers/brightness.h"
#include "KeyDispatcher.hpp"
#include "helpersWindows.hpp"
#include "libraries/Interception/utils.h"
#include "libraries/Interception/interception.h"

namespace
{
  InterceptionContext context;
  InterceptionDevice device;
  InterceptionKeyStroke keyStroke;

  const auto APP_TITLE = L"KeyRemapper";
  bool isAppEnabled = true;

  KeyDispatcher* keyDispatcher;

  HWND g_hwnd;
  const int IDM_EXIT = 5;
  const int IDM_ENABLE = 6;
  const int IDM_MODE_1 = 11;
  const int IDM_MODE_2 = 12;
  const int IDM_MODE_3 = 13;
  const int IDM_MODE_4 = 14;

  // https://codingmisadventures.wordpress.com/2009/02/20/creating-a-system-tray-icon-using-visual-c-and-win32/
  NOTIFYICONDATA nid;
#define WM_MYMESSAGE (WM_USER + 1)
  auto globalIconImage = HICON(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_DEFAULT), IMAGE_ICON, 0, 0, 0));
  auto globalIconImageDisabled = HICON(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_RED), IMAGE_ICON, 0, 0, 0));

  void createSystemTrayIcon(HINSTANCE hInstance, HWND hWnd)
  {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 100;
    nid.uVersion = NOTIFYICON_VERSION;
    nid.uCallbackMessage = WM_MYMESSAGE;
    nid.hIcon = globalIconImage;
    wcscpy_s(nid.szTip, APP_TITLE);
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    Shell_NotifyIcon(NIM_ADD, &nid);
  }

  void initializeKeyDispatcher()
  {
    auto rules = HelpersWindows::getJsonFile("mode1.json");
    auto symbols = HelpersWindows::getJsonFile("symbols.json");
    keyDispatcher = new KeyDispatcher(rules, symbols);
  }

  void toggleAppEnabled()
  {
    isAppEnabled = !isAppEnabled;

    if (isAppEnabled) {
      nid.hIcon = globalIconImage;
      initializeKeyDispatcher();
    } else {
      nid.hIcon = globalIconImageDisabled;
    }

    Shell_NotifyIcon(NIM_MODIFY, &nid);
  }

  void handleAppExit()
  {
    Shell_NotifyIcon(NIM_DELETE, &nid);
  }

  DWORD WINAPI keyboardThreadFunc(void *data)
  {
    raise_process_priority();
    context = interception_create_context();
    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

    initializeKeyDispatcher();

    auto testResults = keyDispatcher->runTests();
    HelpersWindows::print(!testResults.is_null() ? testResults["message"] : "NO TESTS RUN");

    while (interception_receive(
               context,
               device = interception_wait(context),
               (InterceptionStroke *)&keyStroke,
               1) > 0)
    {
      auto code = keyStroke.code;
      auto state = keyStroke.state;
      auto newKeys = keyDispatcher->applyKeys({{code, state}});
      auto newKeysSize = newKeys.size();

      if (newKeysSize == 3 && newKeys[2].code == 245)
      {
        toggleAppEnabled();
        continue;
      }

      if (!isAppEnabled)
      {
        interception_send(context, device, (InterceptionStroke *)&keyStroke, 1);
        continue;
      }

      for (size_t i = 0; i < newKeysSize; i++)
      {
        auto [code, state] = newKeys[i];
        auto newKeyStroke = InterceptionKeyStroke({code, state});

        if (code == 241)
        {
          if (state == 0)
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
          else
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
        else if (code == 242)
        {
          if (state == 0)
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
          else
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
        }
        else if (code == 243 && state == 0)
        {
          BrightnessHandler::Increment(-10);
        }
        else if (code == 244 && state == 0)
        {
          BrightnessHandler::Increment(10);
        }
        else
        {
          interception_send(
              context,
              device,
              (InterceptionStroke *)&newKeyStroke,
              1);
        }
      }
    }

    interception_destroy_context(context);

    return 0;
  }

  void CALLBACK handleWindowChange(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
  {
    keyDispatcher->setAppName(HelpersWindows::getActiveWindowProcessName(hwnd));
  }

  LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    switch (uMsg)
    {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_MYMESSAGE:
      switch (lParam)
      {
      case WM_LBUTTONUP:
      case WM_RBUTTONUP:
        POINT pt;
        GetCursorPos(&pt);
        HMENU hPopMenu = CreatePopupMenu();
        SetForegroundWindow(hWnd);

        InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit");
        InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_4, L"Mode 4");
        InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_3, L"Mode 3");
        InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_2, L"Mode 2");
        InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MODE_1, L"Mode 1");
        InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_ENABLE, L"On/Off");
        TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);
        return 0;
      };
    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDM_EXIT:
        exit(0);
        return 0;
      case IDM_ENABLE:
        toggleAppEnabled();
        return 0;
      case IDM_MODE_1:
      case IDM_MODE_2:
      case IDM_MODE_3:
      case IDM_MODE_4:
        return 0;
      }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  HWND createWindow(HINSTANCE hInstance)
  {
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"WinClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    HWND hWnd = CreateWindowEx(
        0,                   // Optional window styles.
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
} // namespace

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
  std::atexit(handleAppExit);

  g_hwnd = createWindow(hInstance);

  if (!g_hwnd)
    return 0;

  SetWinEventHook(
      EVENT_SYSTEM_FOREGROUND,
      EVENT_SYSTEM_FOREGROUND,
      NULL,
      handleWindowChange,
      0, 0,
      WINEVENT_OUTOFCONTEXT);

  createSystemTrayIcon(hInstance, g_hwnd);

  CreateThread(NULL, 0, keyboardThreadFunc, NULL, 0, NULL);

  MSG messages;
  while (GetMessage(&messages, NULL, 0, 0))
  {
    TranslateMessage(&messages);
    DispatchMessage(&messages);
  }

  return messages.wParam;
}
