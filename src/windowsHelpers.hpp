#pragma once

#include "helpers.hpp"
#include "libraries/json.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

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

String getAbsPath(String filepath) {
  char moduleFilepath[MAX_PATH];
  GetModuleFileNameA(NULL, moduleFilepath, MAX_PATH);
  auto strModuleFilepath = std::string(moduleFilepath);

  auto dir = strModuleFilepath.substr(0, strModuleFilepath.find_last_of("\\"));
  return dir + "\\" + filepath;
}

String getActiveWindowProcessName(HWND hwnd) {
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
} // namespace WindowsHelpers
