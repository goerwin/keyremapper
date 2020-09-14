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
#include "KeyEvent.h"
#include "utils.h"
#include "libraries/Interception/utils2.h"
#include "libraries/Interception/interception.h"

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

void defaultKeyRemaps(InterceptionKeyStroke &keyStroke)
{
  auto keyCode = keyStroke.code;
  auto keyState = keyStroke.state;

  if (keyCode == SC_LBSLASH || keyCode == SC_RSHIFT)
  {
    keyStroke.code = SC_LSHIFT;
  }
  else if (keyCode == SC_RWIN)
  {
    keyStroke.code = SC_LWIN;
  }
  else if (keyCode == SC_LCTRL && (keyState == 2 || keyState == 3))
  {
    keyStroke.code = SC_LWIN;
  }
}

void sendKeyEvents(std::vector<Key> keys)
{
  auto keysSize = keys.size();

  for (int i = 0; i < keysSize; i++)
  {
    auto keyCode = keys[i].code;
    auto state = keys[i].state;

    if (keyCode == SC_NULL)
    {
      continue;
    }

    if (keyCode == SC_MOUSELEFT)
    {
      if (state == 0)
      {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
      }
      else
      {
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
      }
    }
    else if (keyCode == SC_MOUSERIGHT)
    {
      if (state == 0)
      {
        mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
      }
      else
      {
        mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
      }
    }
    else if (keyCode == SC_BRIGHTNESSDOWN)
    {
      BrightnessHandler::Increment(-10);
    }
    else if (keyCode == SC_BRIGHTNESSUP)
    {
      BrightnessHandler::Increment(10);
    }
    else
    {
      unsigned short stateDown = 0;
      unsigned short stateUp = 1;

      switch (keyCode)
      {
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

        if (state == 0)
        {
          state = stateDown;
        }
        else if (state == 1)
        {
          state = stateUp;
        }

        break;
      case SC_NUMLOCK:
        break;
      case SC_NP2:
      case SC_NP4:
      case SC_NP6:
      case SC_NP8:
        if (state == 0 || state == 1)
        {
          if (keyCode != SC_NP4)
          {
            if (state == 0)
              state = 2;
            else
              state = 3;
            keyCode = SC_LEFT;
          }
        }
        break;
      }

      if (state == 4)
      {
        interception_send(
            context,
            device,
            (InterceptionStroke *)&InterceptionKeyStroke({keyCode, stateDown}),
            1);
        interception_send(
            context,
            device,
            (InterceptionStroke *)&InterceptionKeyStroke({keyCode, stateUp}),
            1);
      }
      else
      {
        if (keyCode == SC_LSHIFT && (state == 0 || state == 1))
        {
          keyCode = keyCode;
          //continue;
        }

        interception_send(
            context,
            device,
            (InterceptionStroke *)&InterceptionKeyStroke({keyCode, state}),
            1);
      }
    }
  }
}

DWORD WINAPI keyboardThreadFunc(void *data)
{
  raise_process_priority();
  context = interception_create_context();
  interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

  while (interception_receive(
             context,
             device = interception_wait(context),
             (InterceptionStroke *)&keyStroke,
             1) > 0)
  {
    defaultKeyRemaps(keyStroke);
    auto key = Key(keyStroke.code, keyStroke.state);
    Keys newKeys = KeyEvent::getKeyEvents({key});
    sendKeyEvents(newKeys);
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
