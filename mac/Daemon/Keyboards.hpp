#pragma once

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>

#include <functional>
#include <string>

// https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/HID/new_api_10_5/tn2187.html

// Seizes the keyboards (also the ones connected later), so their events only
// reach onInput and not the OS
class Keyboards {
 public:
  // scancode, isKeyDown, vendorId, productId, manufacturer, product
  std::function<void(ushort, bool, int, int, std::string, std::string)>
      onInput;

  ~Keyboards() { stop(); }

  void start() {
    stop();
    manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

    CFMutableDictionaryRef keyboard =
        getDeviceMatchDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);
    CFMutableDictionaryRef keypad =
        getDeviceMatchDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keypad);
    CFMutableDictionaryRef matchesList[] = {keyboard, keypad};
    CFArrayRef matches = CFArrayCreate(
        kCFAllocatorDefault, (const void **)matchesList, 2, &kCFTypeArrayCallBacks);

    IOHIDManagerSetDeviceMatchingMultiple(manager, matches);
    IOHIDManagerRegisterInputValueCallback(manager, inputValueCb, this);

    CFRelease(keyboard);
    CFRelease(keypad);
    CFRelease(matches);

    IOHIDManagerScheduleWithRunLoop(manager, CFRunLoopGetMain(),
                                    kCFRunLoopCommonModes);
    IOHIDManagerOpen(manager, kIOHIDOptionsTypeSeizeDevice);
  }

  void stop() {
    if (!manager) return;

    IOHIDManagerRegisterInputValueCallback(manager, NULL, NULL);
    IOHIDManagerSetDeviceMatchingMultiple(manager, NULL);
    IOHIDManagerUnscheduleFromRunLoop(manager, CFRunLoopGetMain(),
                                      kCFRunLoopCommonModes);
    IOHIDManagerClose(manager, kIOHIDOptionsTypeSeizeDevice);
    CFRelease(manager);
    manager = NULL;
  }

 private:
  IOHIDManagerRef manager = NULL;

  static CFMutableDictionaryRef getDeviceMatchDictionary(UInt32 usagePage,
                                                         UInt32 usage) {
    CFMutableDictionaryRef ret = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);

    CFNumberRef pageNumber =
        CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usagePage);
    CFDictionarySetValue(ret, CFSTR(kIOHIDDeviceUsagePageKey), pageNumber);
    CFRelease(pageNumber);

    CFNumberRef usageNumber =
        CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
    CFDictionarySetValue(ret, CFSTR(kIOHIDDeviceUsageKey), usageNumber);
    CFRelease(usageNumber);

    return ret;
  }

  static int getIntProperty(IOHIDDeviceRef device, CFStringRef key) {
    int value = 0;
    CFTypeRef ref = IOHIDDeviceGetProperty(device, key);
    if (ref) CFNumberGetValue((CFNumberRef)ref, kCFNumberSInt32Type, &value);
    return value;
  }

  static std::string getStringProperty(IOHIDDeviceRef device, CFStringRef key) {
    char value[256] = "-";
    CFTypeRef ref = IOHIDDeviceGetProperty(device, key);
    if (ref)
      CFStringGetCString((CFStringRef)ref, value, sizeof(value),
                         kCFStringEncodingUTF8);
    return value;
  }

  static void inputValueCb(void *context, IOReturn result, void *sender,
                           IOHIDValueRef value) {
    auto self = (Keyboards *)context;
    ushort scancode = IOHIDElementGetUsage(IOHIDValueGetElement(value));
    bool isKeyDown = IOHIDValueGetIntegerValue(value) == 1;

    if (scancode < 3 || scancode > 235 || !self->onInput) return;

    auto device = (IOHIDDeviceRef)sender;
    self->onInput(scancode, isKeyDown,
                  getIntProperty(device, CFSTR(kIOHIDVendorIDKey)),
                  getIntProperty(device, CFSTR(kIOHIDProductIDKey)),
                  getStringProperty(device, CFSTR(kIOHIDManufacturerKey)),
                  getStringProperty(device, CFSTR(kIOHIDProductKey)));
  }
};

namespace Capslock {
inline io_connect_t open() {
  io_connect_t connect = 0;
  io_service_t service = IOServiceGetMatchingService(
      kIOMainPortDefault, IOServiceMatching(kIOHIDSystemClass));
  if (!service) return 0;

  IOServiceOpen(service, mach_task_self(), kIOHIDParamConnectType, &connect);
  IOObjectRelease(service);
  return connect;
}

inline bool getState() {
  bool state = false;
  io_connect_t connect = open();
  if (!connect) return false;

  IOHIDGetModifierLockState(connect, kIOHIDCapsLockState, &state);
  IOServiceClose(connect);
  return state;
}

// The state is tracked by the caller, because after setting it, getState
// keeps returning the old one
inline void setState(bool state) {
  io_connect_t connect = open();
  if (!connect) return;

  IOHIDSetModifierLockState(connect, kIOHIDCapsLockState, state);
  IOServiceClose(connect);
}
}  // namespace Capslock
