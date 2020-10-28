#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-queryfullprocessimagenamea
#define _WIN32_WINNT 0x0600

#include "KeyDispatcher.hpp"
#include "helpers.hpp"
#include "windowsLibraries/Interception/interception.h"
#include "windowsLibraries/Interception/utils.h"

InterceptionContext context;
InterceptionDevice device;
InterceptionKeyStroke keyStroke;

const auto APP_TITLE = L"KeyRemapper";
bool g_isAppEnabled = true;
int g_mode;
int g_nCmdShow;
HINSTANCE g_hInstance;
std::string g_appName;
KeyDispatcher *keyDispatcher;

HWND g_systemTrayIconWindow;
HWND g_eventWindow;
std::vector<std::string> g_remapInfo;

const int IDM_EXIT = 5;
const int IDM_ENABLE = 6;
const int IDM_MODE_1 = 11;
const int IDM_MODE_2 = 12;
const int IDM_MODE_3 = 13;
const int IDM_MODE_4 = 14;
const int IDM_OPEN_EVENT_WINDOW = 15;
const int IDM_RUN_MODE_TESTS = 16;

std::string getAbsPath(std::string filepath) {
  char moduleFilepath[MAX_PATH];
  GetModuleFileNameA(NULL, moduleFilepath, MAX_PATH);
  auto strModuleFilepath = std::string(moduleFilepath);

  auto dir = strModuleFilepath.substr(0, strModuleFilepath.find_last_of("\\"));
  return dir + "\\" + filepath;
}

// https://codingmisadventures.wordpress.com/2009/02/20/creating-a-system-tray-icon-using-visual-c-and-win32/

void handleApplyKeysCb(std::string remapInfo) {
  g_remapInfo.insert(g_remapInfo.begin(), remapInfo);
  RedrawWindow(g_eventWindow, 0, 0, RDW_INVALIDATE | RDW_INTERNALPAINT);
}

void initializeKeyDispatcher(int mode = 0) {
  std::vector<std::string> modes = {"mode1.json", "mode2.json", "mode3.json",
                                    "mode4.json"};

  auto rules = Helpers::getJsonFile(modes[g_mode], getAbsPath);
  auto symbols =
      Helpers::getJsonFile("symbols.json", getAbsPath);

  delete keyDispatcher;
  keyDispatcher = new KeyDispatcher(rules, symbols);
  keyDispatcher->setApplyKeysCb(handleApplyKeysCb);
  keyDispatcher->setAppName(g_appName);
}

void toggleAppEnabled() {
  g_isAppEnabled = !g_isAppEnabled;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpszArgument, int nCmdShow) {
  initializeKeyDispatcher();

  while (true) {
    unsigned short code = 30;
    unsigned short state = 0;
    auto newKeys = keyDispatcher->applyKeys({{ code, state }});
  }

  return 0;

}
