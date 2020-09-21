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

DWORD WINAPI keyboardThreadFunc(void *data)
{
  raise_process_priority();
  context = interception_create_context();
  interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

  auto rules = Helpers::getJsonFile("./src/core.json");
  auto symbols = Helpers::getJsonFile("./src/symbols.json");
  auto keyDispatcher = new KeyDispatcher(rules, symbols);
  auto testResults = keyDispatcher->runTests();

  Helpers::print(!testResults.is_null() ? testResults["message"] : "");

  while (interception_receive(
             context,
             device = interception_wait(context),
             (InterceptionStroke *)&keyStroke,
             1) > 0)
  {

    auto code = keyStroke.code;
    auto state = keyStroke.state;
    
    auto newKeys = keyDispatcher->apply({ {code, state} });

    for (int i = 0; i < newKeys.size(); i++)
    {
      auto [keyCode, state] = newKeys[i];
      auto newKeyStroke = InterceptionKeyStroke({ keyCode, state });

      interception_send(
          context,
          device,
          (InterceptionStroke *)&newKeyStroke,
          1);
    }
  }

  interception_destroy_context(context);

  return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
  CreateThread(NULL, 0, keyboardThreadFunc, NULL, 0, NULL);

  MSG messages;
  while (GetMessage(&messages, NULL, 0, 0))
  {
    TranslateMessage(&messages);
    DispatchMessage(&messages);
  }

  return messages.wParam;
}
