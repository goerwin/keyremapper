#pragma once

#include "libraries/json.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

namespace Helpers {
using namespace std;
using json = nlohmann::json;
typedef std::string String;
typedef unsigned short ushort;

String trim(String str) {
  size_t first = str.find_first_not_of(' ');
  if (String::npos == first) {
    return str;
  }
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

String replaceAll(String str, const String &from, const String &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != String::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos +=
        to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}

vector<String> split(String str, char key) {
  std::stringstream test(str);
  String segment;
  std::vector<String> result;

  while (std::getline(test, segment, key)) {
    result.push_back(segment);
  }

  return result;
}

template <typename T>
vector<T> concatArrays(vector<T> arr1, vector<T> arr2, int pos = -1) {
  typename vector<T>::iterator insertPos;

  if (pos == -1)
    insertPos = arr1.end();
  else
    insertPos = arr1.begin() + pos;

  arr1.insert(insertPos, arr2.begin(), arr2.end());
  return arr1;
}

void print(string str, string str2 = "\n") {
  auto res = str + str2;
  std::cout << res << "";
  OutputDebugStringA(res.c_str());
}
} // namespace Helpers
