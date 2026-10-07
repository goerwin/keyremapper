#pragma once

#import <AppKit/AppKit.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hidsystem/ev_keymap.h>

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// https://developer.apple.com/library/archive/documentation/DeviceDrivers/Conceptual/HID/new_api_10_5/tn2187.html

// Consumer keys (eg. media buttons): their scancode in symbols.json and the
// event macOS turns them into (a media key, a vkCode or none, -1)
struct ConsumerKey {
  uint32_t usage;
  ushort scancode;
  int nxKeyType;
  int vkCode;
};

inline const std::vector<ConsumerKey> consumerKeys = {
    {kHIDUsage_Csmr_PlayOrPause, 307, NX_KEYTYPE_PLAY, -1},
    {kHIDUsage_Csmr_Mute, 309, NX_KEYTYPE_MUTE, -1},
    {kHIDUsage_Csmr_VolumeDecrement, 310, NX_KEYTYPE_SOUND_DOWN, -1},
    {kHIDUsage_Csmr_VolumeIncrement, 311, NX_KEYTYPE_SOUND_UP, -1},
    {kHIDUsage_Csmr_VoiceCommand, 312, -1, 176},
    {kHIDUsage_Csmr_ACBack, 313, -1, -1},
};

// Seizes the keyboards (also the ones connected later), so their events only
// reach onInput and not the OS.
// Consumer devices that aren't keyboards (eg. the media buttons of a remote,
// which share the device with its pointer) can't be seized without losing the
// pointer. Their consumer keys are read without seizing, and the events macOS
// makes from them have to be dropped (shouldDrop)
class Keyboards {
 public:
  // The native events shouldDrop takes
  static constexpr CGEventMask eventMask =
      CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp) |
      CGEventMaskBit(NSEventTypeSystemDefined);

  // scancode, isKeyDown, vendorId, productId, manufacturer, product
  std::function<void(ushort, bool, int, int, std::string, std::string)>
      onInput;

  ~Keyboards() { stop(); }

  void start() {
    stop();

    manager = createManager(
        {{kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard},
         {kHIDPage_GenericDesktop, kHIDUsage_GD_Keypad}},
        inputValueCb);
    IOHIDManagerOpen(manager, kIOHIDOptionsTypeSeizeDevice);

    consumerManager =
        createManager({{kHIDPage_Consumer, kHIDUsage_Csmr_ConsumerControl}},
                      consumerInputValueCb);
    IOHIDManagerRegisterDeviceMatchingCallback(consumerManager,
                                               consumerDeviceMatchedCb, this);
    IOHIDManagerOpen(consumerManager, kIOHIDOptionsTypeNone);
  }

  void stop() {
    stopManager(manager, kIOHIDOptionsTypeSeizeDevice);
    stopManager(consumerManager, kIOHIDOptionsTypeNone);
    isSenderConsumerDevice.clear();
  }

  // Whether macOS made the event from a consumer key of a device that isn't
  // seized
  bool shouldDrop(CGEventType type, CGEventRef event) {
    return isConsumerKeyEvent(type, event) && isFromConsumerDevice(event);
  }

  // -1 for the usages that aren't keys
  static int getScancode(uint32_t usagePage, uint32_t usage) {
    if (usagePage == kHIDPage_Consumer) {
      for (auto &key : consumerKeys)
        if (key.usage == usage) return key.scancode;
      return -1;
    }

    // Other pages, like Apple's one for the Fn key (3), are taken as keyboard
    // usages
    return usage < 3 || usage > 235 ? -1 : usage;
  }

  // Whether macOS makes the event from a consumer key
  static bool isConsumerKeyEvent(CGEventType type, CGEventRef event) {
    if (type == kCGEventKeyDown || type == kCGEventKeyUp) {
      auto vkCode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
      for (auto &key : consumerKeys)
        if (key.vkCode == vkCode) return true;
      return false;
    }

    @autoreleasepool {
      NSEvent *nsEvent = [NSEvent eventWithCGEvent:event];
      if (nsEvent.subtype != NX_SUBTYPE_AUX_CONTROL_BUTTONS) return false;
      int nxKeyType = (int)((nsEvent.data1 & 0xffff0000) >> 16);
      for (auto &key : consumerKeys)
        if (key.nxKeyType == nxKeyType) return true;
      return false;
    }
  }

 private:
  // Undocumented, the registry ID of the IOHIDEventService that made the
  // event. 0 for the posted events
  static constexpr CGEventField senderIdField = (CGEventField)87;

  IOHIDManagerRef manager = NULL;
  IOHIDManagerRef consumerManager = NULL;
  std::unordered_map<uint64_t, bool> isSenderConsumerDevice;

  IOHIDManagerRef createManager(
      std::vector<std::pair<UInt32, UInt32>> usages,
      IOHIDValueCallback callback) {
    auto manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

    CFMutableArrayRef matches =
        CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    for (auto &[usagePage, usage] : usages) {
      auto match = getDeviceMatchDictionary(usagePage, usage);
      CFArrayAppendValue(matches, match);
      CFRelease(match);
    }

    IOHIDManagerSetDeviceMatchingMultiple(manager, matches);
    IOHIDManagerRegisterInputValueCallback(manager, callback, this);
    CFRelease(matches);

    IOHIDManagerScheduleWithRunLoop(manager, CFRunLoopGetMain(),
                                    kCFRunLoopCommonModes);
    return manager;
  }

  static void stopManager(IOHIDManagerRef &manager, IOOptionBits options) {
    if (!manager) return;

    IOHIDManagerRegisterInputValueCallback(manager, NULL, NULL);
    IOHIDManagerRegisterDeviceMatchingCallback(manager, NULL, NULL);
    IOHIDManagerSetDeviceMatchingMultiple(manager, NULL);
    IOHIDManagerUnscheduleFromRunLoop(manager, CFRunLoopGetMain(),
                                      kCFRunLoopCommonModes);
    IOHIDManagerClose(manager, options);
    CFRelease(manager);
    manager = NULL;
  }

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

  static bool isKeyboard(IOHIDDeviceRef device) {
    return IOHIDDeviceConformsTo(device, kHIDPage_GenericDesktop,
                                 kHIDUsage_GD_Keyboard) ||
           IOHIDDeviceConformsTo(device, kHIDPage_GenericDesktop,
                                 kHIDUsage_GD_Keypad);
  }

  static void inputValueCb(void *context, IOReturn result, void *sender,
                           IOHIDValueRef value) {
    auto self = (Keyboards *)context;
    auto element = IOHIDValueGetElement(value);
    int scancode = getScancode(IOHIDElementGetUsagePage(element),
                               IOHIDElementGetUsage(element));
    bool isKeyDown = IOHIDValueGetIntegerValue(value) == 1;

    if (scancode < 0 || !self->onInput) return;

    auto device = (IOHIDDeviceRef)sender;
    self->onInput(scancode, isKeyDown,
                  getIntProperty(device, CFSTR(kIOHIDVendorIDKey)),
                  getIntProperty(device, CFSTR(kIOHIDProductIDKey)),
                  getStringProperty(device, CFSTR(kIOHIDManufacturerKey)),
                  getStringProperty(device, CFSTR(kIOHIDProductKey)));
  }

  // The keyboards are seized and send their consumer keys to inputValueCb
  static void consumerInputValueCb(void *context, IOReturn result,
                                   void *sender, IOHIDValueRef value) {
    auto element = IOHIDValueGetElement(value);
    if (IOHIDElementGetUsagePage(element) != kHIDPage_Consumer ||
        isKeyboard((IOHIDDeviceRef)sender))
      return;

    inputValueCb(context, result, sender, value);
  }

  static void consumerDeviceMatchedCb(void *context, IOReturn result,
                                      void *sender, IOHIDDeviceRef device) {
    // The events of the new device could have been taken as not from a
    // consumer device
    ((Keyboards *)context)->isSenderConsumerDevice.clear();
  }

  bool isFromConsumerDevice(CGEventRef event) {
    uint64_t senderId = CGEventGetIntegerValueField(event, senderIdField);
    if (!senderId || !consumerManager) return false;

    auto it = isSenderConsumerDevice.find(senderId);
    if (it != isSenderConsumerDevice.end()) return it->second;

    std::unordered_set<uint64_t> deviceIds;
    CFSetRef devices = IOHIDManagerCopyDevices(consumerManager);
    if (devices) {
      std::vector<const void *> values(CFSetGetCount(devices));
      CFSetGetValues(devices, values.data());
      for (auto device : values) {
        uint64_t deviceId = 0;
        IORegistryEntryGetRegistryEntryID(
            IOHIDDeviceGetService((IOHIDDeviceRef)device), &deviceId);
        deviceIds.insert(deviceId);
      }
      CFRelease(devices);
    }

    // The sender is a service under the device
    bool isConsumerDevice = false;
    io_registry_entry_t entry = IOServiceGetMatchingService(
        kIOMainPortDefault, IORegistryEntryIDMatching(senderId));
    while (entry && !isConsumerDevice) {
      uint64_t entryId = 0;
      IORegistryEntryGetRegistryEntryID(entry, &entryId);
      isConsumerDevice = deviceIds.count(entryId);

      io_registry_entry_t parent = 0;
      IORegistryEntryGetParentEntry(entry, kIOServicePlane, &parent);
      IOObjectRelease(entry);
      entry = parent;
    }
    if (entry) IOObjectRelease(entry);

    return isSenderConsumerDevice[senderId] = isConsumerDevice;
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
