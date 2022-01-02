//
//  main.cpp
//  KeyRemapper
//
//  Created by Erwin Gaitan Ospino on 7/12/21.
//

#include <ApplicationServices/ApplicationServices.h>

#import <AppKit/AppKit.h>
#import <AppKit/NSWorkspace.h>

#include <iostream>
#include <thread>
#include <IOKit/hidsystem/ev_keymap.h>

#include "../../common/TestHelpers.hpp"
#include "../../common/Helpers.hpp"
#include "../../common/vendors/json.hpp"
#include "./main.h"
#include "./Application.h"
#include "./IOHIDManager.hpp"
#include "./MouseHandler.hpp"
#include "./Global.hpp"

void sendNotification(std::string message, std::string title = "KeyRemapper") {
  Helpers::print(message);
  system(("osascript -e 'display notification \"" + message + "\" with title \"" + title + "\"'").c_str());
}

// TODO: This is inefficient
ushort getMacVKCode(short scanCode) {
  for (auto &[key, value] : Global::symbols.items()) {
    if (value[0] == scanCode) return value[1];
  }

  return {};
}

void setActiveApp(std::string activeApp) {
  Global::activeApp = activeApp;
  Global::keyDispatcher->setAppName(Global::activeApp);
}

void setModifierFlagsToKeyEvent(CGEventRef event, short vkCode, bool isKeyDown) {
  CGEventFlags flags = 0;

  if (Global::isCmdDown) flags = flags | kCGEventFlagMaskCommand;
  if (Global::isShiftDown) flags = flags | kCGEventFlagMaskShift;
  if (Global::isAltDown) flags = flags | kCGEventFlagMaskAlternate;
  if (Global::isCtrlDown) flags = flags | kCGEventFlagMaskControl;
  if (Global::isFnDown) flags = flags | kCGEventFlagMaskSecondaryFn;
  if (IOHIDManager::capslockState) flags = flags | kCGEventFlagMaskAlphaShift;

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

void postDownUpMediaKey(ushort vkCode, bool isKeyDown) {
  if (!isKeyDown) return;

  auto cgDownEvent = cgEventCreateMediaKey(vkCode, true);
  auto cgUpEvent = cgEventCreateMediaKey(vkCode, false);

  CGEventPost(kCGHIDEventTap, cgDownEvent);
  CGEventPost(kCGHIDEventTap, cgUpEvent);
  CFRelease(cgDownEvent);
  CFRelease(cgUpEvent);
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

void handleKeyRepeat(CGKeyCode vkCode, bool isKeyDown) {
  if (!isKeyDown || Global::isModifierKeyVkCode(vkCode)) {
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
    while (Global::keyRepeatThreadCount == threadIdx && Global::shouldKeyRepeat) {
      auto vkCode = (CGKeyCode)Global::repeatedKey;

      if (Global::isMediaVkKeyCode(vkCode)) postDownUpMediaKey(vkCode, true);
      else postKey(vkCode, true, true);

      std::this_thread::sleep_for(std::chrono::milliseconds(Global::keyRepeatInterval));
    }
  },
  Global::keyRepeatThreadCount);
  threadObj.detach();
}

void handleIOHIDKeyboardInput(ushort scancode, bool isKeyDown, int vendorId, int productId, std::string manufacturer, std::string product) {
  auto keyboard = std::to_string(productId) + ":" + std::to_string(vendorId);

  Global::keyDispatcher->setKeyboard(keyboard, manufacturer + " | " + product);

  auto newKeys = Global::keyDispatcher->applyKeys({{scancode, isKeyDown}});

  auto newKeysSize = newKeys.size();

  for (size_t i = 0; i < newKeysSize; i++) {
    auto [code, isKeyDown] = newKeys[i];
    auto vkCode = getMacVKCode(code);

    if (vkCode == 246 && isKeyDown) return initializeKeyDispatcher(0);
    if (vkCode == 247 && isKeyDown) return initializeKeyDispatcher(1);
    if (vkCode == 248 && isKeyDown) return initializeKeyDispatcher(2);
    if (vkCode == 249 && isKeyDown) return initializeKeyDispatcher(3);

    if (vkCode == 55) Global::isCmdDown = isKeyDown;
    else if (vkCode == 56) Global::isShiftDown = isKeyDown;
    else if (vkCode == 58) Global::isAltDown = isKeyDown;
    else if (vkCode == 59) Global::isCtrlDown = isKeyDown;
    else if (vkCode == 63) Global::isFnDown = isKeyDown;
    
    if (vkCode == 57) {
      if (isKeyDown) IOHIDManager::toggleCapslockState();
      continue;
    }
    
    if (vkCode == 241) {
      MouseHandler::handleMouseDownUp(isKeyDown);
      continue;
    }
    
    if (vkCode == 242) {
      MouseHandler::handleMouseDownUp(isKeyDown, "right");
      continue;
    }
      
    if (Global::isMediaVkKeyCode(vkCode)) postDownUpMediaKey(vkCode, isKeyDown);
    else postKey(vkCode, isKeyDown);

    handleKeyRepeat(vkCode, isKeyDown);
  }
}

void initializeKeyDispatcher(int mode) {
  std::vector<std::string> modes = {"mode1.json", "mode2.json", "mode3.json", "mode4.json"};

  std::string selectedMode = modes[mode];
  if (access((Global::resourcesParentDirPath + "/" + selectedMode).c_str(), R_OK) < 0) {
    sendNotification(selectedMode + " file does not exist");
    if (selectedMode == "mode1.json") exit(1);
    return;
  }

  auto rules = Helpers::getJsonFile(Global::resourcesParentDirPath, selectedMode);
  Global::symbols = Helpers::getJsonFile(Global::resourcesParentDirPath, "/symbols.json");

  if (rules.is_null() || Global::symbols.is_null()) {
    sendNotification("No rules (" + selectedMode + ") or symbols.json files provided");
    return;
  }

  MouseHandler::doubleClickSpeed = rules["doubleClickSpeed"].is_null()
  ? MouseHandler::doubleClickSpeed
  : rules["doubleClickSpeed"].get<double>();
  Global::delayUntilRepeat = rules["delayUntilRepeat"].is_null()
  ? Global::delayUntilRepeat
    : rules["delayUntilRepeat"].get<int>();
  Global::keyRepeatInterval = rules["keyRepeatInterval"].is_null()
    ? Global::keyRepeatInterval
    : rules["keyRepeatInterval"].get<int>();

  delete Global::keyDispatcher;
  Global::keyDispatcher = new KeyDispatcher(rules, Global::symbols);
  Global::keyDispatcher->setAppName(Global::activeApp);

  Global::keyDispatcher->setApplyKeysCb([](std::string appName, std::string keyboardId, std::string keyboardDescription, std::string keys) {
    Helpers::print(
      "App: " + appName + "\n" +
      "Keyboard (vendorId:productId): " + keyboardId + "\n" +
      "KeyboardDescription: " + keyboardDescription + "\n" +
      "Keys: " + keys + "\n");
  });


  auto tests = rules["tests"];
  std::string testsResultsMsg = "";
  if (!tests.is_null()) {
    auto testResults = TestHelpers::runTests(tests, rules, Global::symbols);
    testsResultsMsg = "\n" + std::string(!testResults.is_null() ?
    testResults["message"] : "NO TESTS RAN");
  }

  sendNotification(selectedMode + " selected" + testsResultsMsg);
}

CFMachPortRef g_appToggleEventTap;
CFRunLoopSourceRef g_appToggleRunLoopSource;
void initializeAppToggleHotkey() {
  g_appToggleEventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, CGEventMaskBit(kCGEventKeyUp), [](CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    auto flags = CGEventGetFlags(event);
    bool isCtrlKeyDown = (flags & kCGEventFlagMaskControl) == kCGEventFlagMaskControl;
    auto code = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

    // Ctrl + Esc
    if (code == 53 && isCtrlKeyDown) toggleAppEnabled();

    return event;
  }, NULL);

  if (!g_appToggleEventTap) {
    std::cout << "Accesibility disabled for this app";
    exit(1);
  }

  g_appToggleRunLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, g_appToggleEventTap, 0);

  if (!g_appToggleRunLoopSource) {
    std::cout << "Couldn't create runLoopSource";
    exit(1);
  }

  CFRunLoopAddSource(CFRunLoopGetMain(), g_appToggleRunLoopSource, kCFRunLoopDefaultMode);
}

void terminateAppToggleHotkey() {
  if (g_appToggleEventTap) {
    CFMachPortInvalidate(g_appToggleEventTap);
    CFRelease(g_appToggleEventTap);
  }

  if (g_appToggleRunLoopSource) {
    CFRunLoopRemoveSource(CFRunLoopGetMain(), g_appToggleRunLoopSource, kCFRunLoopDefaultMode);
    CFRelease(g_appToggleRunLoopSource);
  }
}

void toggleAppEnabled() {
  Global::isAppEnabled = !Global::isAppEnabled;

  if (Global::isAppEnabled) {
    Global::reset();
    initializeKeyDispatcher();
    IOHIDManager::onIOHIDKeyboardInput = handleIOHIDKeyboardInput;
    IOHIDManager::initialize();
    MouseHandler::initialize();
  } else {
    IOHIDManager::terminate();
    MouseHandler::terminate();
    sendNotification("App status: Disabled");
  }

  terminateAppToggleHotkey();
  initializeAppToggleHotkey();
}

int main(int argc, const char *argv[]) {
  // argv[0] is the absolute path of the executable
  Global::resourcesParentDirPath = std::string(argv[0]);
  Global::resourcesParentDirPath = Global::resourcesParentDirPath.substr(0, Global::resourcesParentDirPath.find_last_of("/")).append("/resources");

  // NOTE: Running this Objective C code inside a function doesn't work
  Application *application = [[Application alloc]init];
  application.activeApplicationChangeCb = setActiveApp;
  [[[NSWorkspace sharedWorkspace] notificationCenter]
    addObserver:application
    selector:@selector(handleApplicationChange:)
    name:NSWorkspaceDidActivateApplicationNotification
    object:nil
  ];
  NSRunningApplication *frontmostApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
  NSString *bundleIdentifier = [frontmostApp bundleIdentifier];
  auto currentApp = std::string([bundleIdentifier UTF8String]);
  Helpers::print(currentApp);

  toggleAppEnabled();
  setActiveApp(currentApp);

  CFRunLoopRun();
}
