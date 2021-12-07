#include "KeyDispatcher.hpp"
#include <ApplicationServices/ApplicationServices.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDElement.h>
#include <IOKit/hid/IOHIDValue.h>

// If you dont need this, delete the app in the build phase of the project
// I think I will have to create a swift project instead
//#include <AppKit/NSWorkspace.h>
//#include <AppKit/NSApplication.h>

#include <thread>
#include <iostream>
#include <chrono>

#include "json.hpp"

// HELPFUL
//https://stackoverflow.com/questions/7190852/using-iohidmanager-to-get-modifier-key-events
//https://stackoverflow.com/questions/8676135/osx-hid-filter-for-secondary-keyboard

// Note from karabiner-elements
// We should not dispatch modifier key events while key repeating.
// (See comments in `handle_pointing_device_event_from_event_tap` for details.)

KeyDispatcher *keyDispatcher;
nlohmann::json g_symbols;


struct IOKitKeyEvent
{
  ushort scancode;
  bool isPressed;
};

CFMutableDictionaryRef myCreateDeviceMatchingDictionary(UInt32 usagePage,
                                                        UInt32 usage)
{
  CFMutableDictionaryRef ret = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                                         0, &kCFTypeDictionaryKeyCallBacks,
                                                         &kCFTypeDictionaryValueCallBacks);
  if (!ret)
    return NULL;

  CFNumberRef pageNumberRef = CFNumberCreate(kCFAllocatorDefault,
                                             kCFNumberIntType, &usagePage);
  if (!pageNumberRef)
  {
    CFRelease(ret);
    return NULL;
  }

  CFDictionarySetValue(ret, CFSTR(kIOHIDDeviceUsagePageKey), pageNumberRef);
  CFRelease(pageNumberRef);

  CFNumberRef usageNumberRef = CFNumberCreate(kCFAllocatorDefault,
                                              kCFNumberIntType, &usage);
  if (!usageNumberRef)
  {
    CFRelease(ret);
    return NULL;
  }

  CFDictionarySetValue(ret, CFSTR(kIOHIDDeviceUsageKey), usageNumberRef);
  CFRelease(usageNumberRef);

  return ret;
}

// TODO: This is inefficient
ushort getMacVKCode(short scanCode)
{
  for (auto &[key, value] : g_symbols.items())
  {
    if (value[0] == scanCode)
    {
      return value[3];
    }
  }

  return {};
}

bool isCmdDown = false;
bool isShiftDown = false;
bool isAltDown = false;
bool isCtrlDown = false;
bool isFnDown = false;
//CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStatePrivate);
CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);

auto g_fnKeyVkCodes = {122, 120, 99, 118, 96, 97, 98, 100, 101, 109, 103, 111};
auto g_arrowKeyVkCodes = {123,124,125,126};

bool isArrowKeyVkCode(ushort vkCode) {
    return std::find(g_arrowKeyVkCodes.begin(), g_arrowKeyVkCodes.end(), vkCode) != g_arrowKeyVkCodes.end();
}

bool isFunctionKeyVkCode(ushort vkCode) {
    return std::find(g_fnKeyVkCodes.begin(), g_fnKeyVkCodes.end(), vkCode) != g_fnKeyVkCodes.end();
}

void setModifierFlagsToKeyEvent(CGEventRef event, short vkCode, bool isKeyDown)
{
  CGEventFlags flags = 0;

  if (isCmdDown)
    flags = flags | kCGEventFlagMaskCommand;
  if (isShiftDown)
    flags = flags | kCGEventFlagMaskShift;
  if (isAltDown)
    flags = flags | kCGEventFlagMaskAlternate;
  if (isCtrlDown)
    flags = flags | kCGEventFlagMaskControl;
  if (isFnDown)
    flags = flags | kCGEventFlagMaskSecondaryFn;

  if (isArrowKeyVkCode(vkCode))
      flags = flags | kCGEventFlagMaskNumericPad | kCGEventFlagMaskSecondaryFn;
  else if (isFunctionKeyVkCode(vkCode))
    flags = flags | kCGEventFlagMaskSecondaryFn;
  else
    flags = flags | kCGEventFlagMaskNonCoalesced;

  CGEventSetFlags(event, flags);
}

void setModifierFlagsToExternalMouseEvent(CGEventRef event)
{
  CGEventFlags flags = 0;

  if (isCmdDown)
    flags = flags | kCGEventFlagMaskCommand;
  if (isShiftDown)
    flags = flags | kCGEventFlagMaskShift;
  if (isAltDown)
    flags = flags | kCGEventFlagMaskAlternate;
  if (isCtrlDown)
    flags = flags | kCGEventFlagMaskControl;
  if (isFnDown)
    flags = flags | kCGEventFlagMaskSecondaryFn;

  CGEventSetFlags(event, flags);
}


bool g_shouldKeyRepeat = false;
CGKeyCode g_repeatedKey;
int g_delayUntilRepeat;
int g_keyRepeatInterval;
int g_keyRepeatThreadCount = 0;
nlohmann::json g_noAllowedRepeatVKCodes = {};
void handleKeyRepeat(CGKeyCode vkCode, ushort state)
{
  if (state == 0)
  {
    g_shouldKeyRepeat = true;
    g_repeatedKey = vkCode;
    g_keyRepeatThreadCount = g_keyRepeatThreadCount > 9999 ? 0 : g_keyRepeatThreadCount + 1;

    // In theory only non letter/numbers/modifiers should repeat
    std::thread threadObj([](int threadIdx)
                          {
                            if (threadIdx != g_keyRepeatThreadCount || !g_shouldKeyRepeat)
                              return;

                            std::this_thread::sleep_for(
                                std::chrono::milliseconds(g_delayUntilRepeat));

                            while (g_keyRepeatThreadCount == threadIdx && g_shouldKeyRepeat)
                            {
                              auto vkCode = (CGKeyCode)g_repeatedKey;
                              auto event = CGEventCreateKeyboardEvent(eventSource, vkCode, 1);
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
  else
  {
    g_shouldKeyRepeat = false;
    g_repeatedKey = {};
  }
}

void handleIOKitKeyEvent(IOKitKeyEvent ioKitKeyEvent)
{
  auto newKeys = keyDispatcher->applyKeys({{ioKitKeyEvent.scancode, ushort(ioKitKeyEvent.isPressed ? 0 : 1)}});
  auto newKeysSize = newKeys.size();

  for (size_t i = 0; i < newKeysSize; i++)
  {
    auto [code, state] = newKeys[i];
    auto vkCode = getMacVKCode(code);
    auto isKeyDown = state == 0;

    if (vkCode == 55 && isKeyDown)
      isCmdDown = true;
    else if (vkCode == 55 && !isKeyDown)
      isCmdDown = false;
    else if (vkCode == 56 && isKeyDown)
      isShiftDown = true;
    else if (vkCode == 56 && !isKeyDown)
      isShiftDown = false;
    else if (vkCode == 58 && isKeyDown)
      isAltDown = true;
    else if (vkCode == 58 && !isKeyDown)
      isAltDown = false;
    else if (vkCode == 59 && isKeyDown)
      isCtrlDown = true;
    else if (vkCode == 59 && !isKeyDown)
      isCtrlDown = false;
    else if (vkCode == 63 && isKeyDown)
      isFnDown = true;
    else if (vkCode == 63 && !isKeyDown)
      isFnDown = false;

    if (vkCode == 241 && isKeyDown) {
// https://github.com/pqrs-org/Karabiner-Elements/blob/fdc9d542a6f17258655f595e4d51d1e26aa25d41/src/vendor/cget/cget/pkg/pqrs-org__cpp-osx-cg_event/install/include/pqrs/osx/cg_event/mouse.hpp
//    https://stackoverflow.com/questions/1483657/performing-a-double-click-using-cgeventcreatemouseevent
        // Note: Drag partially works on webpages, not in system wide events though
        CGEventRef ourEvent = CGEventCreate(NULL);
        auto mouseLoc  = CGEventGetLocation(ourEvent); //get current mouse position
        auto newMouseEvent = CGEventCreateMouseEvent(eventSource, kCGEventLeftMouseDown, mouseLoc, kCGMouseButtonLeft);
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
      auto newMouseEvent = CGEventCreateMouseEvent(eventSource, kCGEventLeftMouseUp, mouseLoc, kCGMouseButtonLeft);
        CGEventSetIntegerValueField(newMouseEvent, kCGMouseEventClickState, 1);

        CGEventPost(kCGHIDEventTap, newMouseEvent);
      CFRelease(newMouseEvent);

      return;
    }


    auto newEvent = CGEventCreateKeyboardEvent(eventSource, (CGKeyCode)vkCode, state == 0);

    setModifierFlagsToKeyEvent(newEvent, vkCode, isKeyDown);
    CGEventPost(kCGHIDEventTap, newEvent);
    CFRelease(newEvent);

    handleKeyRepeat(vkCode, state);
  }
}

void myHIDKeyboardCallback(void *context, IOReturn result, void *sender,
                           IOHIDValueRef value)
{
  IOHIDElementRef elem = IOHIDValueGetElement(value);
  ushort scancode = IOHIDElementGetUsage(elem);
  long pressed = IOHIDValueGetIntegerValue(value);
  auto isPressed = pressed == 1;

  if (scancode < 3 || scancode > 235)
    return;

  handleIOKitKeyEvent(IOKitKeyEvent({scancode, isPressed}));
}

void initializeKeyDispatcher()
{
  auto rules = Helpers::getJsonFile("mode1.json");
  g_symbols = Helpers::getJsonFile("symbols.json");
  g_delayUntilRepeat = rules["delayUntilRepeat"].is_null()
    ? 250
    : rules["delayUntilRepeat"].get<int>();
  g_keyRepeatInterval = rules["keyRepeatInterval"].is_null()
    ? 25
    : rules["keyRepeatInterval"].get<int>();

  keyDispatcher = new KeyDispatcher(rules, g_symbols);
  //auto testResults = keyDispatcher->runTests();
  //Helpers::print(!testResults.is_null() ? testResults["message"]
  // TODO: "NO TESTS RUN");
}

CGEventRef myEventTapCallBack(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon)
{
  setModifierFlagsToExternalMouseEvent(event);
  return event;
}

void myNotificationCenterCallback (CFNotificationCenterRef center, void* observer, CFNotificationName name,
               const void* object,
               CFDictionaryRef userInfo)
{
  std::cout << "Hello World\n";
  //NSLog(@"New application: %@", [[name userInfo] objectForKey:NSWorkspaceApplicationKey]);
}

int main(int argc, const char *argv[])
{
  initializeKeyDispatcher();

  IOHIDManagerRef hidManager = IOHIDManagerCreate(kCFAllocatorDefault,
                                                  kIOHIDOptionsTypeNone);

  CFMutableDictionaryRef keyboard =
      myCreateDeviceMatchingDictionary(0x01, 6);
  CFMutableDictionaryRef keypad =
      myCreateDeviceMatchingDictionary(0x01, 7);

  CFMutableDictionaryRef matchesList[] = {
      keyboard,
      keypad,
  };

  CFArrayRef matches = CFArrayCreate(kCFAllocatorDefault,
                                     (const void **)matchesList, 2, NULL);

  IOHIDManagerSetDeviceMatchingMultiple(hidManager, matches);
  IOHIDManagerRegisterInputValueCallback(
      hidManager,
      myHIDKeyboardCallback,
      NULL);

  IOHIDManagerSetInputValueMatching(hidManager, keyboard);
  IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetMain(),
                                  kCFRunLoopDefaultMode);


//    Listen for frontmostapp not working for now
//    NSWorkspaceDidActivateApplicationNotification  CFSTR

    CFNotificationCenterRef center = CFNotificationCenterGetDistributedCenter();
      CFNotificationCenterAddObserver(center, nullptr, myNotificationCenterCallback,
                                      NULL, CFSTR("NSWorkspaceDidActivateApplicationNotification"),
                                      CFNotificationSuspensionBehaviorDeliverImmediately);



  // had issues syncronizing timing from hidManager loop and this one.
  // So I'm emulating the flags for keyboard events in the hidManager loop and use this only for
  // mouse events

  auto myEventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
                                     CGEventMaskBit(kCGEventLeftMouseDown) | CGEventMaskBit(kCGEventLeftMouseUp) | CGEventMaskBit(kCGEventMouseMoved) | CGEventMaskBit(kCGEventLeftMouseDragged) | CGEventMaskBit(kCGEventRightMouseDown) | CGEventMaskBit(kCGEventRightMouseUp) | CGEventMaskBit(kCGEventRightMouseDragged),
                                     myEventTapCallBack, NULL);
  if (myEventTap)
  {
    auto myRunLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, myEventTap, 0);
      if (myRunLoopSource) {
          CFRunLoopAddSource(CFRunLoopGetMain(), myRunLoopSource, kCFRunLoopDefaultMode);
      }
  } else {
      std::cout << "Accesibility disabled for this app";
  }
  //    auto myEventTap = CGEventTapCreate(kCGHIDEventTap,
  //                                                kCGHeadInsertEventTap,
  //                                                kCGEventTapOptionDefault,
  //                                                CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp) | CGEventMaskBit(kCGEventFlagsChanged),
  //                                                MyEventTapCallBack,
  //                                                (__bridge void *)self);

  //CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, CGEventMaskBit(kCGEventKeyDown | CGEventMaskBit(kCGEventKeyUp) | CGEventMaskBit(kCGEventFlagsChanged)), <#CGEventTapCallBack  _Nonnull callback#>, <#void * _Nullable userInfo#>)

//   kIOHIDOptionsTypeSeizeDevice: aUsed to open exclusive communication with the device. This will prevent the system and other clients from receiving events from the device.
//      kIOHIDOptionsTypeNone: captures keyboard input evand let it through the OS
  IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeSeizeDevice);

  // main thread
  CFRunLoopRun();
}
