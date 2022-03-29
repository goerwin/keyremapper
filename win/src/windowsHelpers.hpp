#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include "../../common/vendors/json.hpp"

namespace WindowsHelpers {
using namespace std;
using json = nlohmann::json;
typedef std::string String;
typedef unsigned short ushort;

void print(string str, string str2 = "\n") {
  auto res = str + str2;
  std::cout << res << "";
  OutputDebugStringA(res.c_str());
}

String getAbsDirPath() {
  char moduleFilepath[MAX_PATH];
  GetModuleFileNameA(NULL, moduleFilepath, MAX_PATH);
  auto strModuleFilepath = std::string(moduleFilepath);

  return strModuleFilepath.substr(0, strModuleFilepath.find_last_of("\\"));
}

const char* getEnv(const char* name) {
  const DWORD buffSize = 65535;
  static char buffer[buffSize];
  if (GetEnvironmentVariableA(name, buffer, buffSize)) {
    return buffer;
  } else {
    return 0;
  }
}

String getHomeDirPath() {
  auto envHomePath = getEnv("HOMEPATH");
  std::string homeDirPath(envHomePath);

  print(homeDirPath);
  return homeDirPath;
}

String getMainDirPath() {
  #if defined(_DEBUG)
    return WindowsHelpers::getAbsDirPath();
  #else
    return WindowsHelpers::getHomeDirPath() + "\\" + "keyRemapperWin";
  #endif
}

String getActiveWindowProcessName() {
  auto hwnd = GetForegroundWindow();

  if (!hwnd) {
    return {};
  }

  String processName;
  DWORD dwPID;
  GetWindowThreadProcessId(hwnd, &dwPID);
  HANDLE handle =
      OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);

  if (handle) {
    CHAR path[MAX_PATH];
    DWORD maxPath = MAX_PATH;

    if (QueryFullProcessImageNameA(handle, 0, path, &maxPath)) {
      processName = path;
      String::size_type idx = processName.rfind("\\");

      if (idx != String::npos) {
        processName = processName.substr(idx + 1);
      }
    }

    CloseHandle(handle);
  }

  return processName;
}

bool fileExists(String path) {
  DWORD dwAttrib = GetFileAttributesA(path.c_str());
  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
          !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void sendNotification(std::string title, std::string message) {
  MessageBoxA(NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
}
} // namespace WindowsHelpers
