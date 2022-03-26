// All about XPC
// https://rderik.com/blog/xpc-services-on-macos-apps-using-swift/#the-idea-behind-xpc-and-its-uses

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <AppKit/NSWorkspace.h>

#import <IOKit/hidsystem/ev_keymap.h>

#import "../../common/KeyRemapper.hpp"
#import "../../common/Helpers.hpp"
#import "../../common/TestHelpers.hpp"

#import "./Global.hpp"
#import "./MyIOHIDManager.hpp"
#import "./MouseManager.hpp"
#import "./AppBridge.h"

// TODO: This is inefficient
ushort getMacVKCode(short scanCode) {
  for (auto &[key, value] : Global::symbols.items()) {
    if (value[0] == scanCode) return value[3];
  }

  return {};
}

void setModifierFlagsToKeyEvent(CGEventRef event, short vkCode, bool isKeyDown) {
  CGEventFlags flags = 0;

  if (Global::isCmdDown) flags = flags | kCGEventFlagMaskCommand;
  if (Global::isShiftDown) flags = flags | kCGEventFlagMaskShift;
  if (Global::isAltDown) flags = flags | kCGEventFlagMaskAlternate;
  if (Global::isCtrlDown) flags = flags | kCGEventFlagMaskControl;
  if (Global::isFnDown) flags = flags | kCGEventFlagMaskSecondaryFn;
  if (MyIOHIDManager::capslockState) flags = flags | kCGEventFlagMaskAlphaShift;

  if (Global::isArrowKeyVkCode(vkCode))
    flags = flags | kCGEventFlagMaskNumericPad | kCGEventFlagMaskSecondaryFn;
  else if (Global::isFunctionKeyVkCode(vkCode)) flags = flags | kCGEventFlagMaskSecondaryFn;
  else flags = flags | kCGEventFlagMaskNonCoalesced;

  CGEventSetFlags(event, flags);
}

CGEventRef cgEventCreateMediaKey(ushort vkCode, bool isDown) {
  ushort newCode = 0;

  if (vkCode == 300) newCode = NX_KEYTYPE_BRIGHTNESS_DOWN;
  else if (vkCode == 301) newCode = NX_KEYTYPE_BRIGHTNESS_UP;
  else if (vkCode == 304) newCode = NX_KEYTYPE_ILLUMINATION_DOWN;
  else if (vkCode == 305) newCode = NX_KEYTYPE_ILLUMINATION_UP;

  else if (vkCode == 306) newCode = NX_KEYTYPE_REWIND;
  else if (vkCode == 307) newCode = NX_KEYTYPE_PLAY;
  else if (vkCode == 308) newCode = NX_KEYTYPE_FAST;

  else if (vkCode == 309) newCode = NX_KEYTYPE_MUTE;
  else if (vkCode == 310) newCode = NX_KEYTYPE_SOUND_DOWN;
  else if (vkCode == 311) newCode = NX_KEYTYPE_SOUND_UP;

  NSEvent* nsEvent = [NSEvent
      otherEventWithType:NSEventTypeSystemDefined
      location: NSMakePoint(0, 0)
      modifierFlags:0xa00
      timestamp:0
      windowNumber:0
      context:0
      subtype:8
      data1: (newCode << 16) | (isDown ? (0xa << 8) : (0xb << 8))
      data2: -1
  ];

  return CGEventCreateCopy([nsEvent CGEvent]);
}

void postKey(ushort vkCode, bool isKeyDown, bool isRepeat = false) {
  auto eventSource = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
  auto event = CGEventCreateKeyboardEvent(eventSource, (CGKeyCode)vkCode, isKeyDown);
  setModifierFlagsToKeyEvent(event, vkCode, isKeyDown);

  if (isRepeat) CGEventSetIntegerValueField(event, kCGKeyboardEventAutorepeat, 1);
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

void handleKeyRepeat(CGKeyCode vkCode, bool isKeyDown) {
  if (!isKeyDown) {
    Global::shouldKeyRepeat = false;
    Global::repeatedKey = {};
    return;
  }

  Global::shouldKeyRepeat = true;
  Global::repeatedKey = vkCode;
  Global::keyRepeatThreadCount = Global::keyRepeatThreadCount > 9999 ? 0 : Global::keyRepeatThreadCount + 1;

  std::thread threadObj([](int threadIdx) {
    if (threadIdx != Global::keyRepeatThreadCount || !Global::shouldKeyRepeat) return;

    std::this_thread::sleep_for(std::chrono::milliseconds(Global::delayUntilRepeat));

    while (Global::isAppEnabled && Global::keyRepeatThreadCount == threadIdx && Global::shouldKeyRepeat) {
      auto vkCode = (CGKeyCode)Global::repeatedKey;

      if (Global::isMediaVkKeyCode(vkCode)) postDownUpMediaKey(vkCode, true);
      else postKey(vkCode, true, true);
      std::this_thread::sleep_for(std::chrono::milliseconds(Global::keyRepeatInterval));
    }
  },
  Global::keyRepeatThreadCount);
  threadObj.detach();
}

void enableLogging() {
  Global::keyRemapper->setApplyKeysCb([](std::string appName, std::string keyboardId, std::string keyboardDescription, std::string keys) {
    Helpers::print(
      "App: " + appName + "\n" +
      "Keyboard (vendorId:productId): " + keyboardId + "\n" +
      "KeyboardDescription: " + keyboardDescription + "\n" +
      "Keys: " + keys + "\n");
  });
}

void disableLogging() {
  Global::keyRemapper->setApplyKeysCb(nil);
}

void handleIOHIDKeyboardInput(ushort scancode, bool isKeyDown, int vendorId, int productId, std::string manufacturer, std::string product) {
  auto keyboard = std::to_string(productId) + ":" + std::to_string(vendorId);
  Global::keyRemapper->setKeyboard(keyboard, manufacturer + " | " + product);
  
  auto keyEvents = Global::keyRemapper->applyKeys({{"", scancode, ushort(isKeyDown ? 0 : 1), false}});
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

      if (!isKeyDown) Global::shouldKeyRepeat = false;

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
}

int start(std::string configPath, std::string symbolsPath) {
  auto config = Helpers::getJsonFile(configPath);
  Global::reset();
  Global::symbols = Helpers::getJsonFile(symbolsPath);
    Global::keyRemapper = new KeyRemapper(config, Global::symbols);
  Global::delayUntilRepeat = config["delayUntilRepeat"].is_null()
  ? Global::delayUntilRepeat
    : config["delayUntilRepeat"].get<int>();
  Global::keyRepeatInterval = config["keyRepeatInterval"].is_null()
    ? Global::keyRepeatInterval
    : config["keyRepeatInterval"].get<int>();
  Global::isAppEnabled = true;

    auto mouseManagerStartRes = MouseManager::start();
    if (mouseManagerStartRes == 1) return 1;
    if (mouseManagerStartRes != 0) return 2;
  
  MouseManager::doubleClickSpeed = config["doubleClickSpeed"].is_null()
  ? MouseManager::doubleClickSpeed
  : config["doubleClickSpeed"].get<double>();
  
    MyIOHIDManager::start();
    MyIOHIDManager::onIOHIDKeyboardInput = handleIOHIDKeyboardInput;

    // TODO: Only set it when logging by the user
//    enableLogging();
    
    return 0;
}

void stop() {
  MouseManager::stop();
  MyIOHIDManager::stop();
  Global::reset();
  
}

void setFrontMostAppAsAsAppName() {
  NSRunningApplication *frontmostApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
          NSString *bundleIdentifier = [frontmostApp bundleIdentifier];
          NSString *localizedName = [frontmostApp localizedName];
          std::string appName = std::string(
            bundleIdentifier != nil ?
              [bundleIdentifier UTF8String] :
              localizedName != nil ? [localizedName UTF8String] : "Unknown"
          );

  Global::keyRemapper->setAppName(appName);
}

@implementation AppBridge
- (int)start:(NSString*)configPath withSymbolsPath:(NSString*)symbolsPath {
  return start([configPath UTF8String], [symbolsPath UTF8String]);
}

- (void)stop {
  stop();
}

- (void)setAppName:(NSString*)appName {
  Global::keyRemapper->setAppName([appName UTF8String]);
}
@end
