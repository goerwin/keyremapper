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

#include "../../common/TestHelpers.hpp"
#include "../../common/Helpers.hpp"
#include "../../common/vendors/json.hpp"
#include "./main.h"
#include "./Application.h"
#include "./IOHIDManager.hpp"
#include "./MouseHandler.hpp"
#include "./Global.hpp"

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>

bool isArrowKeyVkCode(ushort vkCode) {
  return std::find(Global::arrowKeyVkCodes.begin(), Global::arrowKeyVkCodes.end(), vkCode) != Global::arrowKeyVkCodes.end();
}

bool isFunctionKeyVkCode(ushort vkCode) {
  return std::find(Global::fnKeyVkCodes.begin(), Global::fnKeyVkCodes.end(), vkCode) != Global::fnKeyVkCodes.end();
}

void sendNotification(std::string message, std::string title = "KeyRemapper") {
  Helpers::print(message);
  system(("osascript -e 'display notification \""
    + message + "\" with title \"" + title + "\"'").c_str());
}

// TODO: This is inefficient
ushort getMacVKCode(short scanCode) {
  for (auto &[key, value] : Global::symbols.items()) {
    if (value[0] == scanCode) return value[3];
  }

  return {};
}

void toggleAppEnabled() {
  Global::isAppEnabled = !Global::isAppEnabled;
  if (Global::isAppEnabled) initializeKeyDispatcher();
  else {
    
  }
  sendNotification("App status: " + std::string(Global::isAppEnabled ? "Enabled" : "Disabled"));
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

  if (isArrowKeyVkCode(vkCode))
    flags = flags | kCGEventFlagMaskNumericPad | kCGEventFlagMaskSecondaryFn;
  else if (isFunctionKeyVkCode(vkCode)) flags = flags | kCGEventFlagMaskSecondaryFn;
  else flags = flags | kCGEventFlagMaskNonCoalesced;

  CGEventSetFlags(event, flags);
}

void handleKeyRepeat(CGKeyCode vkCode, ushort state) {
  if (state != 0) {
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
      auto event = CGEventCreateKeyboardEvent(Global::eventSource, vkCode, 1);
      setModifierFlagsToKeyEvent(event, vkCode, true);
      CGEventSetIntegerValueField(event, kCGKeyboardEventAutorepeat, 1);
      CGEventPost(kCGHIDEventTap, event);
      std::this_thread::sleep_for(std::chrono::milliseconds(Global::keyRepeatInterval));
    }
  },
  Global::keyRepeatThreadCount);
  threadObj.detach();
}

void handleIOHIDKeyboardInput(ushort scancode, bool isKeyDown, int vendorId, int productId, std::string manufacturer, std::string product) {
  auto keyboard = std::to_string(productId) + ":" + std::to_string(vendorId);
  
  Global::keyDispatcher->setKeyboard(keyboard, manufacturer + " | " + product);
  
  auto newKeys = Global::keyDispatcher->applyKeys({{scancode, ushort(isKeyDown ? 0 : 1)}});
  
  auto newKeysSize = newKeys.size();

  for (size_t i = 0; i < newKeysSize; i++) {
    auto [code, state] = newKeys[i];
    auto vkCode = getMacVKCode(code);
    auto isKeyDown = state == 0;

    if (vkCode == 999 && isKeyDown) {
      exit(0);
      return;
    }

    if (code == 245) return toggleAppEnabled();
    else if (!Global::isAppEnabled) continue;

    if (vkCode == 246 && isKeyDown) return initializeKeyDispatcher(0);
    if (vkCode == 247 && isKeyDown) return initializeKeyDispatcher(1);
    if (vkCode == 248 && isKeyDown) return initializeKeyDispatcher(2);
    if (vkCode == 249 && isKeyDown) return initializeKeyDispatcher(3);

    if (vkCode == 55) Global::isCmdDown = isKeyDown;
    else if (vkCode == 56) Global::isShiftDown = isKeyDown;
    else if (vkCode == 58) Global::isAltDown = isKeyDown;
    else if (vkCode == 59) Global::isCtrlDown = isKeyDown;
    else if (vkCode == 63) Global::isFnDown = isKeyDown;

    if (vkCode == 57 && isKeyDown) return IOHIDManager::toggleCapslockState();
    
    if (vkCode == 241) return MouseHandler::handleMouseDownUp(isKeyDown);
    else if (vkCode == 242) return MouseHandler::handleMouseDownUp(isKeyDown, "right");

    auto newEvent = CGEventCreateKeyboardEvent(Global::eventSource, (CGKeyCode)vkCode, state == 0);
    setModifierFlagsToKeyEvent(newEvent, vkCode, isKeyDown);
    CGEventPost(kCGHIDEventTap, newEvent);
    CFRelease(newEvent);

    handleKeyRepeat(vkCode, state);
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

int main(int argc, const char *argv[]) {
  // argv[0] is the absolute path of the executable
  Global::resourcesParentDirPath = std::string(argv[0]);
  Global::resourcesParentDirPath = Global::resourcesParentDirPath.substr(0, Global::resourcesParentDirPath.find_last_of("/")).append("/resources");

  initializeKeyDispatcher();
  IOHIDManager::onIOHIDKeyboardInput = handleIOHIDKeyboardInput;
  IOHIDManager::initializeIOHIDManager();
  MouseHandler::initialize();
  
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
  Helpers::print(std::string([bundleIdentifier UTF8String]));
  setActiveApp(std::string([bundleIdentifier UTF8String]));

  CFRunLoopRun();
}
