#pragma once

#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "libraries/json.hpp"

namespace HelpersWindows
{
  using namespace std;
  using json = nlohmann::json;
  typedef std::string String;
  typedef unsigned short ushort;

  void print(string str, string str2 = "\n")
  {
    auto res = str + str2;
    std::cout << res << "";
    OutputDebugStringA(res.c_str());
  }

  String getAbsPath(String filepath)
  {
    char moduleFilepath[MAX_PATH];
    GetModuleFileNameA(NULL, moduleFilepath, MAX_PATH);
    auto strModuleFilepath = std::string(moduleFilepath);

    auto dir = strModuleFilepath.substr(0, strModuleFilepath.find_last_of("\\"));
    return dir + "\\" + filepath;
  }

  json getJsonFile(String filepath)
  {
    std::ifstream file(getAbsPath(filepath));
    String fileStr((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());

    auto jsonFile = json::parse(fileStr, nullptr, false, true);

    return jsonFile;
  }

  String getActiveWindowProcessName(HWND hwnd)
  {
    if (!hwnd)
    {
      return NULL;
    }

    String processName;
    DWORD dwPID;
    GetWindowThreadProcessId(hwnd, &dwPID);
    HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);

    if (handle)
    {
      CHAR path[MAX_PATH];

      if (GetModuleFileNameExA(handle, 0, path, MAX_PATH))
      {
        processName = path;
        String::size_type idx = processName.rfind("\\");

        if (idx != String::npos)
        {
          processName = processName.substr(idx + 1);
        }
      }

      CloseHandle(handle);
    }

    return processName;
  }
} // namespace HelpersWindows
