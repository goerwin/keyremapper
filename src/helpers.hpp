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
  vector<T> concatArrays(vector<T> arr1, vector<T> arr2, String pos = "end")
  {
    if (pos == "start")
    {
      arr1.insert(arr1.begin(), arr2.begin(), arr2.end());
    }
    else
    {
      arr1.insert(arr1.end(), arr2.begin(), arr2.end());
    }

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

  double getTimeDifference(double time1, double time2)
  {
    return (time1 - time2) / CLOCKS_PER_SEC * 1000;
  }

  struct KeyMultiplePress
  {
  private:
    int keyConsecutiveClicks = 0;
    double keyDownFirstTimeClock = NULL;
    double keyUpClock = NULL;
    ushort localKeyCode;

  public:
    KeyMultiplePress(ushort keyCode)
    {
      localKeyCode = keyCode;
    }

    int getConsecutivePresses(ushort keyCode, bool isKeyDown)
    {
      if (isKeyDown)
      {
        if (keyCode != localKeyCode)
        {
          keyDownFirstTimeClock = NULL;
          keyConsecutiveClicks = 0;
          return NULL;
        }

        if (!keyDownFirstTimeClock)
        {
          keyDownFirstTimeClock = clock();

          if (
              !keyUpClock ||
              getTimeDifference(keyDownFirstTimeClock, keyUpClock) > 200)
          {
            keyConsecutiveClicks = 0;
          }
        }
      }
      else if (keyDownFirstTimeClock)
      {
        keyUpClock = clock();
        double timeDif = getTimeDifference(keyUpClock, keyDownFirstTimeClock);

        if (timeDif < 200)
        {
          keyConsecutiveClicks++;
          keyDownFirstTimeClock = NULL;
          return keyConsecutiveClicks;
        }
        else
        {
          keyConsecutiveClicks = 0;
          keyDownFirstTimeClock = NULL;
        }
      }

      return NULL;
    }
  };
} // namespace Helpers
