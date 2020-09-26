#include <windows.h>
// #include <chrono>
// #include <shellapi.h>
#include "images/index.h"
#include "helpers/brightness.h"
#include "KeyDispatcher.hpp"
#include "helpers.hpp"
#include "libraries/Interception/utils.h"
#include "libraries/Interception/interception.h"

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

auto rules = Helpers::getJsonFile("./src/core.json");
auto symbols = Helpers::getJsonFile("./src/symbols.json");
auto keyDispatcher = new KeyDispatcher(rules, symbols);

DWORD WINAPI keyboardThreadFunc(void *data)
{
  raise_process_priority();
  context = interception_create_context();
  interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

  auto testResults = keyDispatcher->runTests();
  Helpers::print(!testResults.is_null() ? testResults["message"] : "NO TESTS RUN");

  while (interception_receive(
             context,
             device = interception_wait(context),
             (InterceptionStroke *)&keyStroke,
             1) > 0)
  {

    auto code = keyStroke.code;
    auto state = keyStroke.state;
    auto newKeys = keyDispatcher->applyKeys({{code, state}});

    for (int i = 0; i < newKeys.size(); i++)
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
      else if (code == 245)
      {

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

HWINEVENTHOOK windowHook = 0;

void CALLBACK handleWindowChange(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
  keyDispatcher->setAppName(Helpers::getActiveWindowProcessName(hwnd));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
  // WINDOW CHANGE EVENT

  windowHook = SetWinEventHook(
      EVENT_SYSTEM_FOREGROUND,
      EVENT_SYSTEM_FOREGROUND,
      NULL,
      handleWindowChange,
      0, 0,
      WINEVENT_OUTOFCONTEXT);

  CreateThread(NULL, 0, keyboardThreadFunc, NULL, 0, NULL);

  MSG messages;
  while (GetMessage(&messages, NULL, 0, 0))
  {
    TranslateMessage(&messages);
    DispatchMessage(&messages);
  }

  return messages.wParam;
}
