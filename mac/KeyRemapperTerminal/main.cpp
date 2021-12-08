//
//  main.cpp
//  KeyRemapperTerminal
//
//  Created by Erwin Gaitan Ospino on 7/12/21.
//

#include <ApplicationServices/ApplicationServices.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>

#include <iostream>
#include <thread>
#include "../../common/Helpers.hpp"
#include "../../common/vendors/json.hpp"
#include "../../common/KeyDispatcher.hpp"

std::string g_path;
bool g_capslockState;
KeyDispatcher *g_keyDispatcher;
nlohmann::json g_symbols;
int g_delayUntilRepeat = 250;
int g_keyRepeatInterval = 25;
bool isCmdDown = false;
bool isShiftDown = false;
bool isAltDown = false;
bool isCtrlDown = false;
bool isFnDown = false;
auto g_fnKeyVkCodes = {122, 120, 99, 118, 96, 97, 98, 100, 101, 109, 103, 111};
auto g_arrowKeyVkCodes = {123,124,125,126};
bool g_shouldKeyRepeat = false;
CGKeyCode g_repeatedKey;
int g_keyRepeatThreadCount = 0;
CGEventSourceRef g_eventSource = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);

IOHIDManagerRef g_hidManager;

CFMutableDictionaryRef myCreateDeviceMatchingDictionary(UInt32 usagePage, UInt32 usage) {
  CFMutableDictionaryRef ret = CFDictionaryCreateMutable(
    kCFAllocatorDefault,
    0,
    &kCFTypeDictionaryKeyCallBacks,
    &kCFTypeDictionaryValueCallBacks
  );
  
  if (!ret) return NULL;

  CFNumberRef pageNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usagePage);
  
  if (!pageNumberRef) {
    CFRelease(ret);
    return NULL;
  }

  CFDictionarySetValue(ret, CFSTR(kIOHIDDeviceUsagePageKey), pageNumberRef);
  CFRelease(pageNumberRef);

  CFNumberRef usageNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
  if (!usageNumberRef) {
    CFRelease(ret);
    return NULL;
  }

  CFDictionarySetValue(ret, CFSTR(kIOHIDDeviceUsageKey), usageNumberRef);
  CFRelease(usageNumberRef);

  return ret;
}

bool isArrowKeyVkCode(ushort vkCode) {
  return std::find(g_arrowKeyVkCodes.begin(), g_arrowKeyVkCodes.end(), vkCode) != g_arrowKeyVkCodes.end();
}

bool isFunctionKeyVkCode(ushort vkCode) {
  return std::find(g_fnKeyVkCodes.begin(), g_fnKeyVkCodes.end(), vkCode) != g_fnKeyVkCodes.end();
}

void setModifierFlagsToKeyEvent(CGEventRef event, short vkCode, bool isKeyDown) {
  CGEventFlags flags = 0;

  if (isCmdDown) flags = flags | kCGEventFlagMaskCommand;
  if (isShiftDown) flags = flags | kCGEventFlagMaskShift;
  if (isAltDown) flags = flags | kCGEventFlagMaskAlternate;
  if (isCtrlDown) flags = flags | kCGEventFlagMaskControl;
  if (isFnDown) flags = flags | kCGEventFlagMaskSecondaryFn;
  if (g_capslockState) flags = flags | kCGEventFlagMaskAlphaShift;

  if (isArrowKeyVkCode(vkCode))
    flags = flags | kCGEventFlagMaskNumericPad | kCGEventFlagMaskSecondaryFn;
  else if (isFunctionKeyVkCode(vkCode)) flags = flags | kCGEventFlagMaskSecondaryFn;
  else flags = flags | kCGEventFlagMaskNonCoalesced;

  CGEventSetFlags(event, flags);
}

void setModifierFlagsToExternalMouseEvent(CGEventRef event) {
  CGEventFlags flags = 0;

  if (isCmdDown) flags = flags | kCGEventFlagMaskCommand;
  if (isShiftDown) flags = flags | kCGEventFlagMaskShift;
  if (isAltDown) flags = flags | kCGEventFlagMaskAlternate;
  if (isCtrlDown) flags = flags | kCGEventFlagMaskControl;
  if (isFnDown) flags = flags | kCGEventFlagMaskSecondaryFn;

  CGEventSetFlags(event, flags);
}

void initializeKeyDispatcher() {
  auto rules = Helpers::getJsonFile(g_path, "mode1.json");
  g_symbols = Helpers::getJsonFile(g_path, "/symbols.json");
  
  if (rules.is_null() || g_symbols.is_null()) {
    Helpers::print("No rules (mode1.json) or symbols.json files provided");
    exit(0);
    return;
  }
  
  g_delayUntilRepeat = rules["delayUntilRepeat"].is_null()
    ? g_delayUntilRepeat
    : rules["delayUntilRepeat"].get<int>();
  g_keyRepeatInterval = rules["keyRepeatInterval"].is_null()
    ? g_keyRepeatInterval
    : rules["keyRepeatInterval"].get<int>();

  g_keyDispatcher = new KeyDispatcher(rules, g_symbols);
  
  auto testResults = g_keyDispatcher->runTests();
  Helpers::print(!testResults.is_null() ? testResults["message"]
   : "NO TESTS RUN");
}

void initializeMouseListener() {
  // apparently, real mouse events don't register modifier presses, so I have to listen for
  // mouse events and attach the modifier flags accordingly
  auto myEventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
                                     CGEventMaskBit(kCGEventLeftMouseDown) | CGEventMaskBit(kCGEventLeftMouseUp) | CGEventMaskBit(kCGEventMouseMoved) | CGEventMaskBit(kCGEventLeftMouseDragged) | CGEventMaskBit(kCGEventRightMouseDown) | CGEventMaskBit(kCGEventRightMouseUp) | CGEventMaskBit(kCGEventRightMouseDragged),
                                     [](CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon)
                                     {
                                       setModifierFlagsToExternalMouseEvent(event);
                                       return event;
                                     }, NULL);
  if (myEventTap) {
    auto myRunLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, myEventTap, 0);
    if (myRunLoopSource) {
      CFRunLoopAddSource(CFRunLoopGetMain(), myRunLoopSource, kCFRunLoopDefaultMode);
    }
  } else {
    std::cout << "Accesibility disabled for this app";
  }
}

bool getCapslockState() {
  kern_return_t kr;
  io_connect_t ioc;
  CFMutableDictionaryRef mdict = IOServiceMatching(kIOHIDSystemClass);
  io_connect_t ios = IOServiceGetMatchingService(kIOMasterPortDefault, (CFDictionaryRef) mdict);

  if (!ios) {
    if (mdict) CFRelease(mdict);
    std::cout << "IOServiceGetMatchingService() failed\n";
    return false;
  }

  kr = IOServiceOpen(ios, mach_task_self(), kIOHIDParamConnectType, &ioc);
  IOObjectRelease(ios);
  if (kr != KERN_SUCCESS) {
    std::cout << "IOServiceOpen() failed: " << kr << "\n";
    return false;
  }

  // CAPSLOCK_QUERY
  bool state;

  kr = IOHIDGetModifierLockState(ioc, kIOHIDCapsLockState, &state);
  if (kr != KERN_SUCCESS) {
    IOServiceClose(ioc);
    std::cout << "IOHIDGetModifierLockState() failed: " << kr << "\n";
    return false;
  }

  return state;
}

void toggleCapslockState() {
  // I had to keep track of capslock on a global variable.
  // When I was calling getModifierLockState and then toggling via setModifierLockState
  // it was working the first time, but subsequent get calls return the same state
  kern_return_t kr;
  io_connect_t ioc;
  CFMutableDictionaryRef mdict = IOServiceMatching(kIOHIDSystemClass);
  io_connect_t ios = IOServiceGetMatchingService(kIOMasterPortDefault, (CFDictionaryRef) mdict);

  g_capslockState = !g_capslockState;
  kr = IOServiceOpen(ios, mach_task_self(), kIOHIDParamConnectType, &ioc);
  IOHIDSetModifierLockState(ioc, kIOHIDCapsLockState, g_capslockState);
  IOObjectRelease(ios);
}

// TODO: This is inefficient
ushort getMacVKCode(short scanCode) {
  for (auto &[key, value] : g_symbols.items()) {
    if (value[0] == scanCode) return value[3];
  }

  return {};
}

void handleKeyRepeat(CGKeyCode vkCode, ushort state) {
  if (state != 0) {
    g_shouldKeyRepeat = false;
    g_repeatedKey = {};
    return;
  }

  g_shouldKeyRepeat = true;
  g_repeatedKey = vkCode;
  g_keyRepeatThreadCount = g_keyRepeatThreadCount > 9999 ? 0 : g_keyRepeatThreadCount + 1;

  std::thread threadObj([](int threadIdx) {
    if (threadIdx != g_keyRepeatThreadCount || !g_shouldKeyRepeat)
      return;

    std::this_thread::sleep_for(
        std::chrono::milliseconds(g_delayUntilRepeat));

    while (g_keyRepeatThreadCount == threadIdx && g_shouldKeyRepeat)
    {
      auto vkCode = (CGKeyCode)g_repeatedKey;
      auto event = CGEventCreateKeyboardEvent(g_eventSource, vkCode, 1);
      setModifierFlagsToKeyEvent(event, vkCode, true);
      CGEventSetIntegerValueField(event, kCGKeyboardEventAutorepeat, 1);
      CGEventPost(kCGHIDEventTap, event);
      std::this_thread::sleep_for(
      std::chrono::milliseconds(g_keyRepeatInterval));
    }
  },
  g_keyRepeatThreadCount);
  threadObj.detach();
}

void handleIOKitKeyEvent(ushort scancode, bool isKeyDown) {
  auto newKeys = g_keyDispatcher->applyKeys({{scancode, ushort(isKeyDown ? 0 : 1)}});
  auto newKeysSize = newKeys.size();

  for (size_t i = 0; i < newKeysSize; i++) {
    auto [code, state] = newKeys[i];
    auto vkCode = getMacVKCode(code);
    auto isKeyDown = state == 0;
    
    if (vkCode == 999 && isKeyDown) {
      exit(0);
      return;
    }
    
    if (vkCode == 57 && isKeyDown) return toggleCapslockState();

    if (vkCode == 55 && isKeyDown) isCmdDown = true;
    else if (vkCode == 55 && !isKeyDown) isCmdDown = false;
    else if (vkCode == 56 && isKeyDown) isShiftDown = true;
    else if (vkCode == 56 && !isKeyDown) isShiftDown = false;
    else if (vkCode == 58 && isKeyDown) isAltDown = true;
    else if (vkCode == 58 && !isKeyDown) isAltDown = false;
    else if (vkCode == 59 && isKeyDown) isCtrlDown = true;
    else if (vkCode == 59 && !isKeyDown) isCtrlDown = false;
    else if (vkCode == 63 && isKeyDown) isFnDown = true;
    else if (vkCode == 63 && !isKeyDown) isFnDown = false;

    if (vkCode == 241 && isKeyDown) {
// https://github.com/pqrs-org/Karabiner-Elements/blob/fdc9d542a6f17258655f595e4d51d1e26aa25d41/src/vendor/cget/cget/pkg/pqrs-org__cpp-osx-cg_event/install/include/pqrs/osx/cg_event/mouse.hpp
//    https://stackoverflow.com/questions/1483657/performing-a-double-click-using-cgeventcreatemouseevent
        // Note: Drag partially works on webpages, not in system wide events though
        CGEventRef ourEvent = CGEventCreate(NULL);
        auto mouseLoc  = CGEventGetLocation(ourEvent); //get current mouse position
        auto newMouseEvent = CGEventCreateMouseEvent(g_eventSource, kCGEventLeftMouseDown, mouseLoc, kCGMouseButtonLeft);
//        auto newMouseEventDrag = CGEventCreateMouseEvent(eventSource, kCGEventLeftMouseDragged, mouseLoc, kCGMouseButtonLeft);
        CGEventSetIntegerValueField(newMouseEvent, kCGMouseEventClickState, 1);

//        CGEventSetFlags(newMouseEvent, kCGEventLeftMouseDragged);
        CGEventPost(kCGHIDEventTap, newMouseEvent);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
//        CGEventPost(kCGHIDEventTap, newMouseEventDrag);

//        CFRelease(newMouseEvent);
        return;
    } else if (vkCode == 241 && !isKeyDown) {
        CGEventRef ourEvent = CGEventCreate(NULL);
        auto mouseLoc  = CGEventGetLocation(ourEvent); //get current mouse position
      auto newMouseEvent = CGEventCreateMouseEvent(g_eventSource, kCGEventLeftMouseUp, mouseLoc, kCGMouseButtonLeft);
        CGEventSetIntegerValueField(newMouseEvent, kCGMouseEventClickState, 1);

        CGEventPost(kCGHIDEventTap, newMouseEvent);
      CFRelease(newMouseEvent);

      return;
    }
    
    auto newEvent = CGEventCreateKeyboardEvent(g_eventSource, (CGKeyCode)vkCode, state == 0);

    setModifierFlagsToKeyEvent(newEvent, vkCode, isKeyDown);
    CGEventPost(kCGHIDEventTap, newEvent);
    CFRelease(newEvent);

    handleKeyRepeat(vkCode, state);
  }
}

void myHIDKeyboardCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value) {
  IOHIDElementRef elem = IOHIDValueGetElement(value);
  ushort scancode = IOHIDElementGetUsage(elem);
  long pressed = IOHIDValueGetIntegerValue(value);
  auto isKeyDown = pressed == 1;

  if (scancode < 3 || scancode > 235) return;
  
  handleIOKitKeyEvent(scancode, isKeyDown);
}

void initializeIOHIDManager() {
  CFMutableDictionaryRef keyboard = myCreateDeviceMatchingDictionary(0x01, 6);
  CFMutableDictionaryRef keypad = myCreateDeviceMatchingDictionary(0x01, 7);

  CFMutableDictionaryRef matchesList[] = { keyboard, keypad };

  CFArrayRef matches = CFArrayCreate(kCFAllocatorDefault, (const void **)matchesList, 2, NULL);

  IOHIDManagerSetDeviceMatchingMultiple(g_hidManager, matches);
  IOHIDManagerRegisterInputValueCallback(g_hidManager, myHIDKeyboardCallback, NULL);
  IOHIDManagerSetInputValueMatching(g_hidManager, keyboard);
  
  // kIOHIDOptionsTypeSeizeDevice: aUsed to open exclusive communication with the device. This will prevent the system and other clients from receiving events from the device.
  // kIOHIDOptionsTypeNone: captures keyboard input evand let it through the OS
  IOHIDManagerScheduleWithRunLoop(g_hidManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
  IOHIDManagerOpen(g_hidManager, kIOHIDOptionsTypeSeizeDevice);
}

int main(int argc, const char *argv[]) {
  // argv[0] is the absolute path of the executable
  g_path = std::string(argv[0]);
  g_path = g_path.substr(0, g_path.find_last_of("/")).append("/resources");
  g_capslockState = getCapslockState();

  initializeKeyDispatcher();

  g_hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
  
  CFMutableDictionaryRef keyboard = myCreateDeviceMatchingDictionary(0x01, 6);
  CFMutableDictionaryRef keypad = myCreateDeviceMatchingDictionary(0x01, 7);

  CFMutableDictionaryRef matchesList[] = { keyboard, keypad, };

  CFArrayRef matches = CFArrayCreate(kCFAllocatorDefault, (const void **)matchesList, 2, NULL);

  IOHIDManagerSetDeviceMatchingMultiple(g_hidManager, matches);
  IOHIDManagerRegisterInputValueCallback(g_hidManager, myHIDKeyboardCallback, NULL);
  IOHIDManagerSetInputValueMatching(g_hidManager, keyboard);
  
  // kIOHIDOptionsTypeSeizeDevice: aUsed to open exclusive communication with the device. This will prevent the system and other clients from receiving events from the device.
  // kIOHIDOptionsTypeNone: captures keyboard input evand let it through the OS
  IOHIDManagerScheduleWithRunLoop(g_hidManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
  IOHIDManagerOpen(g_hidManager, kIOHIDOptionsTypeSeizeDevice);
  
  initializeMouseListener();
  
  CFRunLoopRun();
}
