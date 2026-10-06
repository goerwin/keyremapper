#pragma once

#import "AppKit/AppKit.h"
#import "Foundation/Foundation.h"

#import "IOKit/hidsystem/ev_keymap.h"

#import "../../common/Helpers.hpp"
#import "../../common/KeyRemapper.hpp"

#import "./Global.hpp"
#import "./MouseManager.hpp"
#import "./MyIOHIDManager.hpp"

// Remapping runtime shared by the daemon and the dev runner: listens to the
// keyboards via IOKit, applies the KeyRemapper rules and posts the results
namespace Runtime {
std::function<void(std::string)> onError = [](std::string) {};

// TODO: This is inefficient
ushort getMacVKCode(short scanCode) {
  for (auto& [key, value] : Global::symbols.items()) {
    if (value[0] == scanCode) return value[3];
  }
  return {};
}

void setModifierFlagsToKeyEvent(CGEventRef event, short vkCode,
                                bool isKeyDown) {
  CGEventFlags flags = 0;

  if (Global::isCmdDown) flags = flags | kCGEventFlagMaskCommand;
  if (Global::isShiftDown) flags = flags | kCGEventFlagMaskShift;
  if (Global::isAltDown) flags = flags | kCGEventFlagMaskAlternate;
  if (Global::isCtrlDown) flags = flags | kCGEventFlagMaskControl;
  if (Global::isFnDown) flags = flags | kCGEventFlagMaskSecondaryFn;
  if (MyIOHIDManager::capslockState) flags = flags | kCGEventFlagMaskAlphaShift;

  if (Global::isArrowKeyVkCode(vkCode))
    flags = flags | kCGEventFlagMaskNumericPad | kCGEventFlagMaskSecondaryFn;
  else if (Global::isFunctionKeyVkCode(vkCode))
    flags = flags | kCGEventFlagMaskSecondaryFn;
  else
    flags = flags | kCGEventFlagMaskNonCoalesced;

  CGEventSetFlags(event, flags);
}

CGEventRef cgEventCreateMediaKey(ushort vkCode, bool isDown) {
  ushort newCode = 0;

  if (vkCode == 300)
    newCode = NX_KEYTYPE_BRIGHTNESS_DOWN;
  else if (vkCode == 301)
    newCode = NX_KEYTYPE_BRIGHTNESS_UP;
  else if (vkCode == 304)
    newCode = NX_KEYTYPE_ILLUMINATION_DOWN;
  else if (vkCode == 305)
    newCode = NX_KEYTYPE_ILLUMINATION_UP;

  else if (vkCode == 306)
    newCode = NX_KEYTYPE_REWIND;
  else if (vkCode == 307)
    newCode = NX_KEYTYPE_PLAY;
  else if (vkCode == 308)
    newCode = NX_KEYTYPE_FAST;

  else if (vkCode == 309)
    newCode = NX_KEYTYPE_MUTE;
  else if (vkCode == 310)
    newCode = NX_KEYTYPE_SOUND_DOWN;
  else if (vkCode == 311)
    newCode = NX_KEYTYPE_SOUND_UP;

  NSEvent* nsEvent = [NSEvent
      otherEventWithType:NSEventTypeSystemDefined
                location:NSMakePoint(0, 0)
           modifierFlags:0xa00
               timestamp:0
            windowNumber:0
                 context:0
                 subtype:8
                   data1:(newCode << 16) | (isDown ? (0xa << 8) : (0xb << 8))
                   data2:-1];

  return CGEventCreateCopy([nsEvent CGEvent]);
}

void postKey(ushort vkCode, bool isKeyDown, bool isRepeat = false) {
  auto eventSource = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
  auto event =
      CGEventCreateKeyboardEvent(eventSource, (CGKeyCode)vkCode, isKeyDown);
  setModifierFlagsToKeyEvent(event, vkCode, isKeyDown);

  if (isRepeat)
    CGEventSetIntegerValueField(event, kCGKeyboardEventAutorepeat, 1);
  CGEventPost(kCGHIDEventTap, event);
  CFRelease(eventSource);
  CFRelease(event);
}

void postDownUpMediaKey(ushort vkCode, bool isKeyDown) {
  if (!isKeyDown) return;

  auto cgDownEvent = cgEventCreateMediaKey(vkCode, true);
  auto cgUpEvent = cgEventCreateMediaKey(vkCode, false);

  CGEventPost(kCGHIDEventTap, cgDownEvent);
  CGEventPost(kCGHIDEventTap, cgUpEvent);
  CFRelease(cgDownEvent);
  CFRelease(cgUpEvent);
}

dispatch_source_t keyRepeatTimer = nil;

void stopKeyRepeat() {
  if (!keyRepeatTimer) return;
  dispatch_source_cancel(keyRepeatTimer);
  keyRepeatTimer = nil;
}

void handleKeyRepeat(CGKeyCode vkCode, bool isKeyDown) {
  stopKeyRepeat();
  if (!isKeyDown) return;

  keyRepeatTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0,
                                          dispatch_get_main_queue());
  dispatch_source_set_timer(
      keyRepeatTimer,
      dispatch_time(DISPATCH_TIME_NOW, Global::delayUntilRepeat * NSEC_PER_MSEC),
      Global::keyRepeatInterval * NSEC_PER_MSEC, 0);
  dispatch_source_set_event_handler(keyRepeatTimer, ^{
    if (Global::isMediaVkKeyCode(vkCode))
      postDownUpMediaKey(vkCode, true);
    else
      postKey(vkCode, true, true);
  });
  dispatch_resume(keyRepeatTimer);
}

void startLogging(std::function<void(std::string)> onLog) {
  Global::keyRemapper->setApplyKeysCb(
      [onLog](std::string appName, std::string kbId, std::string kbDesc,
              std::string keys) {
        onLog("AppName: " + appName + "\nKeyboard (productId:vendorId): " +
              kbId + "\nKeyboardDescription: " + kbDesc + "\nKeys: " + keys +
              "\n");
      });
}

void stopLogging() { Global::keyRemapper->setApplyKeysCb(nil); }

void handleIOHIDKeyboardInput(ushort scancode, bool isKeyDown, int vendorId,
                              int productId, std::string manufacturer,
                              std::string product) {
  auto keyboard = std::to_string(productId) + ":" + std::to_string(vendorId);
  Global::keyRemapper->setKeyboard(keyboard, manufacturer + " | " + product);

  try {
    auto keyEvents = Global::keyRemapper->applyKeys(
        {{"", scancode, ushort(isKeyDown ? 0 : 1), false}});
    auto keyEventsSize = keyEvents.size();

    for (size_t i = 0; i < keyEventsSize; i++) {
      auto keyEvent = keyEvents[i];
      auto name = keyEvent.name;

      if (name == "SK:Delay") {
        std::this_thread::sleep_for(std::chrono::milliseconds(keyEvent.state));
        continue;
      }

      auto code = keyEvent.code;
      auto isKeyDown = keyEvent.isKeyDown;
      auto vkCode = getMacVKCode(code);

      if (!isKeyDown) stopKeyRepeat();

      if (vkCode == 55 || vkCode == 54) {
        Global::isCmdDown = isKeyDown;
        postKey(vkCode, isKeyDown);
      } else if (vkCode == 56 || vkCode == 60) {
        Global::isShiftDown = isKeyDown;
        postKey(vkCode, isKeyDown);
      } else if (vkCode == 58 || vkCode == 61) {
        Global::isAltDown = isKeyDown;
        postKey(vkCode, isKeyDown);
      } else if (vkCode == 59 || vkCode == 62) {
        Global::isCtrlDown = isKeyDown;
        postKey(vkCode, isKeyDown);
      } else if (vkCode == 63) {
        Global::isFnDown = isKeyDown;
        postKey(vkCode, isKeyDown);
      } else if (vkCode == 57) {
        if (isKeyDown) MyIOHIDManager::toggleCapslockState();
      } else if (vkCode == 241) {
        MouseManager::handleMouseDownUp(isKeyDown);
      } else if (vkCode == 242) {
        MouseManager::handleMouseDownUp(isKeyDown, "right");
      } else if (Global::isMediaVkKeyCode(vkCode)) {
        postDownUpMediaKey(vkCode, isKeyDown);
        handleKeyRepeat(vkCode, isKeyDown);
      } else {
        postKey(vkCode, isKeyDown);
        handleKeyRepeat(vkCode, isKeyDown);
      }
    }
  } catch (const std::exception& err) {
    onError("ApplyKeysError: " + std::string(err.what()));
  } catch (...) {
    onError("ApplyKeysError: Unknown error");
  }
}

int start(std::string configPath, std::string symbolsPath, int profileIdx,
          std::string appName) {
  try {
    auto config = Helpers::getJsonFile(configPath);

    Global::reset();
    Global::symbols = Helpers::getJsonFile(symbolsPath);
    auto profiles = config["profiles"];

    // TODO: Better to throw than returning
    if (!profiles.is_array()) return 5;  // invalid profiles
    auto activeProfile = profiles.at(profileIdx);

    if (!activeProfile.is_object()) return 6;  // invalid profile

    Global::keyRemapper = new KeyRemapper(activeProfile, Global::symbols);
    Global::keyRemapper->setAppName(appName);

    Global::delayUntilRepeat = activeProfile.value("delayUntilRepeat", 250);
    Global::keyRepeatInterval = activeProfile.value("keyRepeatInterval", 25);

    auto mouseManagerStartRes = MouseManager::start();
    if (mouseManagerStartRes == 1) return 1;
    if (mouseManagerStartRes != 0) return 2;

    MouseManager::doubleClickSpeed =
        activeProfile.value("doubleClickSpeed", 500.0);

    MyIOHIDManager::start();
    MyIOHIDManager::onIOHIDKeyboardInput = handleIOHIDKeyboardInput;
  } catch (const std::exception& err) {
    onError("StartError: " + std::string(err.what()));
  } catch (...) {
    onError("StartError: Unknown error");
  }

  return 0;
}

void stop() {
  stopKeyRepeat();
  MouseManager::stop();
  MyIOHIDManager::stop();
  Global::reset();
}
}  // namespace Runtime
