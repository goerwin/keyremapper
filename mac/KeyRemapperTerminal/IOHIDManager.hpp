#pragma once

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>

#include <iostream>
#include <thread>

class IOHIDManager {
public:
  static IOHIDManagerRef hidManager;
  static bool capslockState;

  // scancode, isKeyDown, vendorId, productId, manufacturer, product
  static std::function<void(ushort, bool, int, int, std::string, std::string)> onIOHIDKeyboardInput;

private:
  static CFMutableDictionaryRef myCreateDeviceMatchingDictionary(UInt32 usagePage, UInt32 usage) {
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

public:
  static void initializeIOHIDManager() {
    IOHIDManager::capslockState = IOHIDManager::getCapslockState();

    auto mainLoop = CFRunLoopGetMain();

    auto ioHIDManagerThread = std::thread([](CFRunLoopRef loop) {
      IOHIDManager::hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
      CFMutableDictionaryRef keyboard = myCreateDeviceMatchingDictionary(0x01, 6);
      CFMutableDictionaryRef keypad = myCreateDeviceMatchingDictionary(0x01, 7);

      CFMutableDictionaryRef matchesList[] = { keyboard, keypad };

      CFArrayRef matches = CFArrayCreate(kCFAllocatorDefault, (const void **)matchesList, 2, NULL);

      IOHIDManagerSetDeviceMatchingMultiple(IOHIDManager::hidManager, matches);
      IOHIDManagerRegisterInputValueCallback(IOHIDManager::hidManager, [](void *context, IOReturn result, void *sender, IOHIDValueRef value) {
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

        IOHIDManager::onIOHIDKeyboardInput(
          scancode,
          isKeyDown,
          vendorId,
          productId,
          manufacturer,
          product
        );
      }, NULL);

      // kIOHIDOptionsTypeSeizeDevice: aUsed to open exclusive communication with the device. This will prevent the system and other clients from receiving events from the device.
      // kIOHIDOptionsTypeNone: captures keyboard input evand let it through the OS
      IOHIDManagerScheduleWithRunLoop(IOHIDManager::hidManager, loop, kCFRunLoopDefaultMode);
      IOHIDManagerOpen(IOHIDManager::hidManager, kIOHIDOptionsTypeSeizeDevice);
    }, mainLoop);

    ioHIDManagerThread.detach();
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
    kern_return_t kr;
    io_connect_t ioc;
    CFMutableDictionaryRef mdict = IOServiceMatching(kIOHIDSystemClass);
    io_connect_t ios = IOServiceGetMatchingService(kIOMasterPortDefault, (CFDictionaryRef) mdict);

    // I had to keep track of capslock on a global variable.
    // When I was calling getModifierLockState and then toggling via setModifierLockState, it was working the first time, but subsequent get calls return the same state
    IOHIDManager::capslockState = !IOHIDManager::capslockState;
    kr = IOServiceOpen(ios, mach_task_self(), kIOHIDParamConnectType, &ioc);
    IOHIDSetModifierLockState(ioc, kIOHIDCapsLockState, IOHIDManager::capslockState);
    IOObjectRelease(ios);
  }
};

// Need to initialize static variables

bool IOHIDManager::capslockState;
IOHIDManagerRef IOHIDManager::hidManager;
std::function<void(ushort, bool, int, int, std::string, std::string)> IOHIDManager::onIOHIDKeyboardInput;