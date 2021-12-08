#pragma once

#include "vendor_json.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <regex>

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
}

json getJsonFile(String dirPath, String filename) {
  std::ifstream file(dirPath + "/" + filename);
  String fileStr((std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>());

  std::vector<std::vector<String>> importKeys = {
      {"dotdotdotObject", "\"%dotdotdotObject\".*\".*\""},
      {"object", "\"%object.*\""},
      {"array", "\"%array.*\""},
      {"dotdotdotArray", "\"%dotdotdotArray.*\""}};

  for (size_t i = 0; i < importKeys.size(); i++) {
    auto importKey = importKeys[i];
    auto name = importKey[0];
    auto regex = std::regex(importKey[1]);

    std::regex_token_iterator<std::string::iterator> rend;
    std::regex_token_iterator<std::string::iterator> a(fileStr.begin(),
                                                       fileStr.end(), regex);

    std::vector<std::string> res;
    while (a != rend) {
      res.push_back(*a);
      a++;
    };

    for (size_t j = 0; j < res.size(); j++) {
      std::smatch matches;
      auto resItem = res[j];
      if (std::regex_search(resItem, matches, std::regex("\\((.*)\\)"))) {
        String innerFileStr = "";

        if (matches.size() == 2) {
          innerFileStr = getJsonFile(dirPath, matches[1]).dump();

          if (name == "dotdotdotArray" || name == "dotdotdotObject") {
            auto openingBracketIdx = name == "dotdotdotArray"
                                         ? innerFileStr.find("[")
                                         : innerFileStr.find("{");
            auto closingBracketIdx = name == "dotdotdotArray"
                                         ? innerFileStr.find_last_of("]")
                                         : innerFileStr.find_last_of("}");
            innerFileStr = innerFileStr.substr(openingBracketIdx + 1,
                                               closingBracketIdx - 1);
          }
        }

        fileStr = Helpers::replaceAll(fileStr, resItem, innerFileStr);
      }
    }
  }

  if (fileStr.size() == 0) return {};
  return json::parse(fileStr, nullptr, false, true);
}
} // namespace Helpers
