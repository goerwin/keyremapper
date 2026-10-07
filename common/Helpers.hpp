#pragma once

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include "./vendors/json.hpp"

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
    // Handles case where 'to' is a substring of 'from'
    start_pos += to.length();
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

json getJsonFile(String filePath);

// "%import(file)" is replaced by the JSON of file, relative to dirPath. In an
// array, an imported array is spread into it
json resolveImports(json value, String dirPath) {
  static const std::regex importRegex("^%import\\((.+)\\)$");

  auto getImport = [&](json &item, json &result) {
    std::smatch matches;
    if (!item.is_string()) return false;
    auto str = item.get<String>();
    if (!std::regex_match(str, matches, importRegex)) return false;
    result = getJsonFile(dirPath + "/" + String(matches[1]));
    return true;
  };

  if (value.is_object()) {
    for (auto &[key, item] : value.items()) {
      json imported;
      item = getImport(item, imported) ? imported
                                       : resolveImports(item, dirPath);
    }
  } else if (value.is_array()) {
    json result = json::array();
    for (auto &item : value) {
      json imported;
      if (!getImport(item, imported))
        result.push_back(resolveImports(item, dirPath));
      else if (imported.is_array())
        result.insert(result.end(), imported.begin(), imported.end());
      else
        result.push_back(imported);
    }
    return result;
  } else {
    json imported;
    if (getImport(value, imported)) return imported;
  }

  return value;
}

// Comments are allowed. Throws when it or a file it imports isn't valid JSON
json getJsonFile(String filePath) {
  auto slashIdx = filePath.find_last_of('/');
  auto dirPath = slashIdx == String::npos ? "." : filePath.substr(0, slashIdx);

  std::ifstream file(filePath);
  auto fileJson = json::parse(file, nullptr, false, true);
  if (fileJson.is_discarded())
    throw std::runtime_error(filePath + " isn't valid JSON");
  return resolveImports(fileJson, dirPath);
}

template <class T>
class circular_buffer {
 private:
  int capacity;
  T *buf;
  int currSize;

 public:
  circular_buffer(int _capacity)
      : currSize(0), buf(new T[_capacity]), capacity(_capacity) {}

  void push_back(T item) {
    if (currSize < capacity) {
      buf[currSize] = item;
      currSize++;
    } else {
      for (int i = 0; i < capacity - 1; i++) {
        buf[i] = buf[i + 1];
      }

      buf[capacity - 1] = item;
    }
  }
  int size() const { return currSize; }

  T operator[](int idx) const { return buf[idx]; }
};
}  // namespace Helpers
