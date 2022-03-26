#pragma once

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>

#include <iostream>

// https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/HID/new_api_10_5/tn2187.html

class MyIOHIDManager {
public:
  static IOHIDManagerRef hidManager;
  static bool capslockState;

  // scancode, isKeyDown, vendorId, productId, manufacturer, product
  static std::function<void(ushort, bool, int, int, std::string, std::string)> onIOHIDKeyboardInput;

private:
  static CFMutableDictionaryRef getDeviceMatchDictionary(UInt32 usagePage, UInt32 usage) {
    CFMutableDictionaryRef ret = CFDictionaryCreateMutable(
      kCFAllocatorDefault,
      0,
      &kCFTypeDictionaryKeyCallBacks,
      &kCFTypeDictionaryValueCallBacks
    );

    if (!ret) return NULL;

    CFNumberRef pageNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usagePage);

    if (pageNumberRef == NULL) {
      CFRelease(ret);
      return NULL;
    }

    CFDictionarySetValue(ret, CFSTR(kIOHIDDeviceUsagePageKey), pageNumberRef);
    CFRelease(pageNumberRef);

    CFNumberRef usageNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
    if (usageNumberRef == NULL) {
      CFRelease(ret);
      return NULL;
    }

    CFDictionarySetValue(ret, CFSTR(kIOHIDDeviceUsageKey), usageNumberRef);
    CFRelease(usageNumberRef);

    return ret;
  }

  static void inputValueCb(void *context, IOReturn result, void *sender, IOHIDValueRef value) {
    IOHIDElementRef elem = IOHIDValueGetElement(value);
    ushort scancode = IOHIDElementGetUsage(elem);

    long pressed = IOHIDValueGetIntegerValue(value);
    auto isKeyDown = pressed == 1;

    if (scancode < 3 || scancode > 235) return;

    IOHIDDeviceRef ioHIDDeviceRef = (IOHIDDeviceRef)sender;
    int vendorId = 0, productId = 0;
    char manufacturer[256] = "-", product[256] = "-";

    CFTypeRef vendorIdRef = IOHIDDeviceGetProperty(ioHIDDeviceRef, CFSTR(kIOHIDVendorIDKey));
    if (vendorIdRef) CFNumberGetValue((CFNumberRef)vendorIdRef, kCFNumberSInt32Type, &vendorId);

    CFTypeRef productIdRef = IOHIDDeviceGetProperty(ioHIDDeviceRef, CFSTR(kIOHIDProductIDKey));
    if (productIdRef) CFNumberGetValue((CFNumberRef)productIdRef, kCFNumberSInt32Type, &productId);

    CFTypeRef manufacturerRef = IOHIDDeviceGetProperty(ioHIDDeviceRef, CFSTR(kIOHIDManufacturerKey));
    if (manufacturerRef) CFStringGetCString((CFStringRef)manufacturerRef, manufacturer, sizeof(manufacturer), kCFStringEncodingUTF8);

    CFTypeRef propertyRef = IOHIDDeviceGetProperty(ioHIDDeviceRef, CFSTR(kIOHIDProductKey));
    if (propertyRef) CFStringGetCString((CFStringRef)propertyRef, product, sizeof(product), kCFStringEncodingUTF8);

    onIOHIDKeyboardInput(
      scancode,
      isKeyDown,
      vendorId,
      productId,
      manufacturer,
      product
    );
  }

public:
  static int start() {
    hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

    if (CFGetTypeID(hidManager) != IOHIDManagerGetTypeID()) return 1;

    capslockState = getCapslockState();

    CFMutableDictionaryRef keyboard = getDeviceMatchDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);
    CFMutableDictionaryRef keypad = getDeviceMatchDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keypad);
    CFMutableDictionaryRef matchesListRef[] = { keyboard, keypad };
    CFArrayRef matches = CFArrayCreate(kCFAllocatorDefault, (const void **)matchesListRef, 2, NULL);

    IOHIDManagerSetDeviceMatchingMultiple(hidManager, matches);

    IOHIDManagerRegisterInputValueCallback(hidManager, inputValueCb, NULL);

    CFRelease(keyboard);
    CFRelease(keypad);
    CFRelease(matches);

    // kIOHIDOptionsTypeSeizeDevice: Used to open exclusive communication with the device. This will prevent the system and other clients from receiving events from the device.
    // kIOHIDOptionsTypeNone: listens to keyboard input and let it through the OS
    IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetMain(), kCFRunLoopCommonModes);
    IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeSeizeDevice);
    
    return 0;
  }

  static void stop() {
    IOHIDManagerRegisterInputValueCallback(hidManager, NULL, NULL);
    IOHIDManagerSetDeviceMatchingMultiple(hidManager, NULL);
    IOHIDManagerUnscheduleFromRunLoop(hidManager, CFRunLoopGetMain(), kCFRunLoopCommonModes);
    IOHIDManagerClose(hidManager, kIOHIDOptionsTypeSeizeDevice);
  }

  static bool getCapslockState() {
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

  static void toggleCapslockState() {
    io_connect_t ioc;
    CFMutableDictionaryRef mdict = IOServiceMatching(kIOHIDSystemClass);
    io_connect_t ios = IOServiceGetMatchingService(kIOMasterPortDefault, (CFDictionaryRef) mdict);

    // I had to keep track of capslock on a global variable.
    // When I was calling getModifierLockState and then toggling via setModifierLockState, it was working the first time, but subsequent get calls return the same state
    capslockState = !capslockState;
    IOServiceOpen(ios, mach_task_self(), kIOHIDParamConnectType, &ioc);
    IOHIDSetModifierLockState(ioc, kIOHIDCapsLockState, capslockState);
    IOObjectRelease(ios);
  }
};

// Need to initialize static variables

bool MyIOHIDManager::capslockState;
IOHIDManagerRef MyIOHIDManager::hidManager;
std::function<void(ushort, bool, int, int, std::string, std::string)> MyIOHIDManager::onIOHIDKeyboardInput;
