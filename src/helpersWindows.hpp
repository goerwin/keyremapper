#pragma once

#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <regex>
#include "helpers.hpp"
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

    std::vector<std::vector<String>> importKeys = {
        {"dotdotdotObject", "\"%dotdotdotObject\".*\".*\""},
        {"object", "\"%object.*\""},
        {"array", "\"%array.*\""},
        {"dotdotdotArray", "\"%dotdotdotArray.*\""}};

    for (auto i = 0; i < importKeys.size(); i++)
    {
      auto importKey = importKeys[i];
      auto name = importKey[0];
      auto regex = std::regex(importKey[1]);

      std::regex_token_iterator<std::string::iterator> rend;
      std::regex_token_iterator<std::string::iterator> a(fileStr.begin(), fileStr.end(), regex);

      std::vector<std::string> res;
      while (a != rend)
      {
        res.push_back(*a);
        a++;
      };

      for (auto j = 0; j < res.size(); j++)
      {
        std::smatch matches;
        auto resItem = res[j];
        if (std::regex_search(resItem, matches, std::regex("\\((.*)\\)")))
        {
          String innerFileStr = "";

          if (matches.size() == 2)
          {
            innerFileStr = getJsonFile(matches[1]).dump();

            if (name == "dotdotdotArray" || name == "dotdotdotObject")
            {
              auto openingBracketIdx = name == "dotdotdotArray" ? innerFileStr.find("[") : innerFileStr.find("{");
              auto closingBracketIdx = name == "dotdotdotArray" ? innerFileStr.find_last_of("]") : innerFileStr.find_last_of("}");
              innerFileStr = innerFileStr.substr(openingBracketIdx + 1, closingBracketIdx - 1);
            }
          }

          fileStr = Helpers::replaceAll(fileStr, resItem, innerFileStr);
        }
      }
    }

    return json::parse(fileStr, nullptr, false, true);
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
      DWORD maxPath = MAX_PATH;

      if (QueryFullProcessImageNameA(handle, 0, path, &maxPath))
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
