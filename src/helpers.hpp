#pragma once

#include <windows.h>
#include <iostream>
#include <string>

namespace Helpers
{
  using namespace std;

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
    std::string segment;
    std::vector<std::string> result;

    while (std::getline(test, segment, key))
    {
      result.push_back(segment);
    }

    return result;
  }

  void print(string str, string str2 = "\n")
  {
    auto res = str + str2;
    OutputDebugStringA(res.c_str());
  }
} // namespace Helpers
