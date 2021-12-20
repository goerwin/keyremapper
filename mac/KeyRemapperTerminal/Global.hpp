#pragma once

#include "../../common/KeyDispatcher.hpp"

namespace Global {
  KeyDispatcher *keyDispatcher;

  bool isCmdDown = false;
  bool isShiftDown = false;
  bool isAltDown = false;
  bool isCtrlDown = false;
  bool isFnDown= false;
  auto fnKeyVkCodes = {122, 120, 99, 118, 96, 97, 98, 100, 101, 109, 103, 111};
  auto arrowKeyVkCodes = {123,124,125,126};

  int delayUntilRepeat = 250;
  int keyRepeatInterval = 25;
  bool shouldKeyRepeat = false;
  int keyRepeatThreadCount = 0;
  CGKeyCode repeatedKey;

  std::string activeApp;
  bool isAppEnabled = false;
  nlohmann::json symbols;
  std::string resourcesParentDirPath;

  void reset() {
    isCmdDown = false;
    isShiftDown = false;
    isAltDown = false;
    isCtrlDown = false;
    isFnDown= false;
  }
}
