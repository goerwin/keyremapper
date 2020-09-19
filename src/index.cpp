// #ifndef UNICODE
// #define UNICODE
// #endif

#include <windows.h>
#include <sstream>
#include <chrono>
#include <shellapi.h>
#include <vector>
#include <fstream>
#include "images/index.h"
#include "helpers/brightness.h"
#include "libraries/json.hpp"
#include "KeyDispatcher.hpp"
#include "helpers.hpp"
#include "utils.h"
#include "libraries/Interception/utils2.h"
#include "libraries/Interception/interception.h"

using json = nlohmann::json;

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

DWORD WINAPI keyboardThreadFunc(void *data)
{
  raise_process_priority();
  context = interception_create_context();
  interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

  std::ifstream coreFile("./src/core.json");
  std::string coreStr((std::istreambuf_iterator<char>(coreFile)),
  std::istreambuf_iterator<char>());

  auto jsonSchema = json::parse(coreStr, nullptr, false, true);
  auto keyDispatcher = new KeyDispatcher(jsonSchema);
  auto testResults = keyDispatcher->runTests();

  Helpers::print(!testResults.is_null() ? testResults["message"] : "");

  while (interception_receive(
             context,
             device = interception_wait(context),
             (InterceptionStroke *)&keyStroke,
             1) > 0)
  {
    auto key = KeyDispatcher::Key(keyStroke.code, keyStroke.state);
    auto newKeys = keyDispatcher->getKeyEvents({key});

    auto keysSize = newKeys.size();

    for (int i = 0; i < keysSize; i++)
    {
      auto keyCode = newKeys[i].code;
      auto state = newKeys[i].state;
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
