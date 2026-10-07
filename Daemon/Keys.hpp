#pragma once

#include <ApplicationServices/ApplicationServices.h>

#include <algorithm>
#include <initializer_list>

// Mac virtual key codes
namespace Keys {
inline bool contains(std::initializer_list<ushort> vkCodes, ushort vkCode) {
  return std::find(vkCodes.begin(), vkCodes.end(), vkCode) != vkCodes.end();
}

inline bool isArrow(ushort vkCode) {
  return contains({123, 124, 125, 126}, vkCode);
}

inline bool isFunction(ushort vkCode) {
  return contains({122, 120, 99, 118, 96, 97, 98, 100, 101, 109, 103, 111},
                  vkCode);
}

inline bool isMedia(ushort vkCode) { return vkCode >= 300 && vkCode <= 311; }

// Like the Mac keys: brightness, keyboard illumination and volume
inline bool isRepeatingMedia(ushort vkCode) {
  return contains({300, 301, 304, 305, 310, 311}, vkCode);
}
}  // namespace Keys

// Modifiers held through the remapped events
struct Modifiers {
  bool cmd = false;
  bool shift = false;
  bool alt = false;
  bool ctrl = false;
  bool fn = false;

  CGEventFlags flags() const {
    CGEventFlags flags = 0;
    if (cmd) flags |= kCGEventFlagMaskCommand;
    if (shift) flags |= kCGEventFlagMaskShift;
    if (alt) flags |= kCGEventFlagMaskAlternate;
    if (ctrl) flags |= kCGEventFlagMaskControl;
    if (fn) flags |= kCGEventFlagMaskSecondaryFn;
    return flags;
  }
};
