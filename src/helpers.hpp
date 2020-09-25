#pragma once

#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "libraries/json.hpp"

namespace Helpers
{
  using namespace std;
  using json = nlohmann::json;
  typedef std::string String;
  typedef unsigned short ushort;

  string trim(string str)
  {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
      return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
  }

  vector<string> split(string str, char key)
  {
    std::stringstream test(str);
    String segment;
    std::vector<String> result;

    while (std::getline(test, segment, key))
    {
      result.push_back(segment);
    }

    return result;
  }

  template <typename T>
  vector<T> concatArrays(vector<T> arr1, vector<T> arr2, int pos = -1)
  {
    typename vector<T>::iterator insertPos;

    if (pos == -1)
      insertPos = arr1.end();
    else
      insertPos = arr1.begin() + pos;
  
    arr1.insert(insertPos, arr2.begin(), arr2.end());
    return arr1;
  }

  void print(string str, string str2 = "\n")
  {
    auto res = str + str2;
    OutputDebugStringA(res.c_str());
  }

  json getJsonFile(String filepath)
  {
    std::ifstream file(filepath);
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
} // namespace Helpers
