#include "KeyDispatcher.hpp"
#include <Carbon/Carbon.h> // TODO: see if you can use another library (this is gonna be deprecated)
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDElement.h>
#include <IOKit/hid/IOHIDValue.h>

// If you dont need this, delete the app in the build phase of the project
// I think I will have to create a swift project instead
//#include <AppKit/AppKit.h>

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

//CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStatePrivate);

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
bool isModifierDown = false;
CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);

void setModifierFlagsToEvent(CGEventRef event)
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
                              auto event = CGEventCreateKeyboardEvent(eventSource, (CGKeyCode)g_repeatedKey, 1);
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

    auto newEvent = CGEventCreateKeyboardEvent(eventSource, (CGKeyCode)vkCode, state == 0);

    if (vkCode == 55 && state == 0)
      isCmdDown = true;
    else if (vkCode == 55 && state == 1)
      isCmdDown = false;
    else if (vkCode == 56 && state == 0)
      isShiftDown = true;
    else if (vkCode == 56 && state == 1)
      isShiftDown = false;
    else if (vkCode == 58 && state == 0)
      isAltDown = true;
    else if (vkCode == 58 && state == 1)
      isAltDown = false;
    else if (vkCode == 59 && state == 0)
      isCtrlDown = true;
    else if (vkCode == 59 && state == 1)
      isCtrlDown = false;

    // This enables onhold popup it for a/e/i/o/u
    //    CGEventSetIntegerValueField(newEvent, kCGKeyboardEventAutorepeat, 1);

    // TODO: pressed n times feature
    // after testing laptop keyboard seems that numbers/letters are the ones that dont repeat
    // other keys repeat

    setModifierFlagsToEvent(newEvent);
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
  setModifierFlagsToEvent(event);
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
//      CFNotificationCenterAddObserver(CFNotificationCenterGetDistributedCenter(), nullptr, myNotificationCenterCallback,
//                                      CFSTR("NSWorkspaceDidActivateApplicationNotification"), nullptr,
//                                      CFNotificationSuspensionBehaviorDeliverImmediately);
    
    

  // had issues syncronizing timing from hidManager loop and this one.
  // So I'm emulating the flags for keyboard events in the hidManager loop and use this only for
  // mouse events
  auto myEventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, CGEventMaskBit(kCGEventLeftMouseDown) | CGEventMaskBit(kCGEventLeftMouseUp) | CGEventMaskBit(kCGEventMouseMoved) | CGEventMaskBit(kCGEventLeftMouseDragged) | CGEventMaskBit(kCGEventRightMouseDown) | CGEventMaskBit(kCGEventRightMouseUp) | CGEventMaskBit(kCGEventRightMouseDragged), myEventTapCallBack, NULL);
  if (myEventTap)
  {
    auto myRunLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, myEventTap, 0);
    if (myRunLoopSource)
      CFRunLoopAddSource(CFRunLoopGetMain(), myRunLoopSource, kCFRunLoopDefaultMode);
  }
  //    auto myEventTap = CGEventTapCreate(kCGHIDEventTap,
  //                                                kCGHeadInsertEventTap,
  //                                                kCGEventTapOptionDefault,
  //                                                CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp) | CGEventMaskBit(kCGEventFlagsChanged),
  //                                                MyEventTapCallBack,
  //                                                (__bridge void *)self);

  //CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, CGEventMaskBit(kCGEventKeyDown | CGEventMaskBit(kCGEventKeyUp) | CGEventMaskBit(kCGEventFlagsChanged)), <#CGEventTapCallBack  _Nonnull callback#>, <#void * _Nullable userInfo#>)

  // kIOHIDOptionsTypeSeizeDevice: aUsed to open exclusive communication with the device. This will prevent the system and other clients from receiving events from the device.
  //    kIOHIDOptionsTypeNone: captures keyboard input evand let it through the OS
  IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeSeizeDevice);
    
  // main thread
  CFRunLoopRun();
}
