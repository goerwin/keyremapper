#pragma once
#import <AppKit/AppKit.h>
#include "../../common/KeyRemapper.hpp"

namespace Global {
  KeyRemapper *keyRemapper;

  bool isCmdDown = false;
  bool isShiftDown = false;
  bool isAltDown = false;
  bool isCtrlDown = false;
  bool isFnDown= false;
  auto fnKeyVkCodes = {122, 120, 99, 118, 96, 97, 98, 100, 101, 109, 103, 111};

  // cmd, cmdR, shift, shiftR, alt, altR, ctrl, ctrlR, fn
  auto modifierVkCodes = {55, 54, 56, 60, 58, 61, 59, 62, 63};
  auto arrowKeyVkCodes = {123, 124, 125, 126};
  auto mediaVkCodes = {300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311};

  int delayUntilRepeat = 250;
  int keyRepeatInterval = 25;
  bool shouldKeyRepeat = false;
  int keyRepeatThreadCount = 0;
  CGKeyCode repeatedKey;

  std::string activeApp;
  bool isAppEnabled = false;
  nlohmann::json symbols;

  bool isArrowKeyVkCode(ushort vkCode) {
    return std::find(arrowKeyVkCodes.begin(), arrowKeyVkCodes.end(), vkCode) != arrowKeyVkCodes.end();
  }

  bool isMediaVkKeyCode(ushort vkCode) {
    return std::find(mediaVkCodes.begin(), mediaVkCodes.end(), vkCode) != mediaVkCodes.end();
  }

  bool isFunctionKeyVkCode(ushort vkCode) {
    return std::find(fnKeyVkCodes.begin(), fnKeyVkCodes.end(), vkCode) != fnKeyVkCodes.end();
  }

  bool isModifierKeyVkCode(ushort vkCode) {
    return std::find(modifierVkCodes.begin(), modifierVkCodes.end(), vkCode) != modifierVkCodes.end();
  }

  void reset() {
    isCmdDown = false;
    isShiftDown = false;
    isAltDown = false;
    isCtrlDown = false;
    isFnDown= false;
    shouldKeyRepeat = false;
    isAppEnabled = false;
    symbols = NULL;
    activeApp = {};
    repeatedKey = NULL;
    keyRepeatThreadCount = 0;

    if (Global::keyRemapper) {
      delete Global::keyRemapper;
      Global::keyRemapper = nil;
    }
  }
}
