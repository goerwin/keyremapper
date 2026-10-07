#pragma once

#import "AppKit/AppKit.h"
#import "Foundation/Foundation.h"

#import "IOKit/hidsystem/ev_keymap.h"

#import "../Engine/Helpers.hpp"
#import "../Engine/KeyRemapper.hpp"
#import "../Common/StartResult.h"

#import "./Keyboards.hpp"
#import "./Keys.hpp"
#import "./Mouse.hpp"

#include <deque>
#include <memory>
#include <unordered_map>

// Remapping runtime shared by the daemon and the dev runner: listens to the
// keyboards via IOKit, applies the KeyRemapper rules and posts the results.
// Everything runs on the main run loop
class Runtime {
 public:
  // It's already stopped by then, it doesn't keep running after an error
  std::function<void(std::string)> onError = [](std::string) {};

  // Replaceable so the tests can record what would reach the OS
  std::function<void(CGEventRef)> postEvent = [](CGEventRef event) {
    CGEventPost(kCGHIDEventTap, event);
  };
  std::function<void(bool)> setCapslock = Capslock::setState;

  ~Runtime() { stop(); }

  // config and symbols are JSON, with the config imports already resolved. It
  // stays stopped when it fails
  StartResult start(std::string config, std::string symbols, int profileIdx) {
    try {
      load(config, symbols, profileIdx);
      auto result = startEventTap();
      if (result != StartResultOk) {
        stop();
        return result;
      }

      setAppName(getFrontmostAppName());
      appObserver = [NSWorkspace.sharedWorkspace.notificationCenter
          addObserverForName:NSWorkspaceDidActivateApplicationNotification
                      object:nil
                       queue:nil
                  usingBlock:^(NSNotification*) {
                    setAppName(getFrontmostAppName());
                  }];

      capslock = Capslock::getState();
      keyboards.onInput = [this](ushort scancode, bool isKeyDown, int vendorId,
                                 int productId, std::string manufacturer,
                                 std::string product) {
        handleInput(scancode, isKeyDown, vendorId, productId, manufacturer,
                    product);
      };
      keyboards.start();
      return StartResultOk;
    } catch (const std::exception& err) {
      fail("StartError: " + std::string(err.what()));
    } catch (...) {
      fail("StartError: Unknown error");
    }

    return StartResultReportedError;
  }

  // Loads the profile without touching the keyboards or the mouse. Throws on
  // invalid JSON or profiles
  void load(std::string configJson, std::string symbolsJson, int profileIdx) {
    stop();

    // symbols.json has comments
    auto config = nlohmann::json::parse(configJson, nullptr, true, true);
    auto symbols = nlohmann::json::parse(symbolsJson, nullptr, true, true);
    auto profiles = config.is_object() ? config["profiles"] : nlohmann::json();
    auto profileName = "Profile " + std::to_string(profileIdx + 1);

    if (!profiles.is_array())
      throw std::runtime_error("The config has no \"profiles\" array");
    if (profileIdx < 0 || profileIdx >= (int)profiles.size())
      throw std::runtime_error(profileName + " not found");
    auto profile = profiles[profileIdx];
    if (!profile.is_object())
      throw std::runtime_error(profileName + " isn't an object");

    // { "keyName": [scanCode, vkCode?] }
    for (auto& [key, value] : symbols.items())
      if (value.size() > 1)
        vkCodes[value[0].get<ushort>()] = value[1].get<ushort>();

    keyRemapper = std::make_unique<KeyRemapper>(profile, symbols);

    delayUntilRepeat = profile.value("delayUntilRepeat", 250);
    keyRepeatInterval = profile.value("keyRepeatInterval", 25);
    mouse.doubleClickSpeed = profile.value("doubleClickSpeed", 500.0);
  }

  void stop() {
    stopTimer(keyRepeatTimer);
    stopTimer(holdTimer);
    stopTimer(delayTimer);
    pendingKeyEvents.clear();
    if (appObserver) {
      [NSWorkspace.sharedWorkspace.notificationCenter removeObserver:appObserver];
      appObserver = nil;
    }
    keyboards.stop();
    stopEventTap();
    keyRemapper = nullptr;
    vkCodes.clear();
    modifiers = {};
  }

  void setAppName(std::string appName) {
    if (keyRemapper) keyRemapper->setAppName(appName);
  }

  void startLogging(std::function<void(std::string)> onLog) {
    if (!keyRemapper) return;
    keyRemapper->setApplyKeysCb([onLog](std::string appName, std::string kbId,
                                        std::string kbDesc, std::string keys) {
      onLog("AppName: " + appName + "\nKeyboard (productId:vendorId): " + kbId +
            "\nKeyboardDescription: " + kbDesc + "\nKeys: " + keys + "\n");
    });
  }

  void stopLogging() {
    if (keyRemapper) keyRemapper->setApplyKeysCb(nullptr);
  }

  void handleInput(ushort scancode, bool isKeyDown, int vendorId,
                   int productId, std::string manufacturer,
                   std::string product) {
    if (!keyRemapper) return;

    auto keyboard = std::to_string(productId) + ":" + std::to_string(vendorId);
    keyRemapper->setKeyboard(keyboard, manufacturer + " | " + product);

    try {
      stopTimer(holdTimer);
      postKeyEvents(keyRemapper->applyKeys({{"", scancode, isKeyDown}}));
      startHoldTimer();
    } catch (const std::exception& err) {
      fail("ApplyKeysError: " + std::string(err.what()));
    } catch (...) {
      fail("ApplyKeysError: Unknown error");
    }
  }

 private:
  std::unique_ptr<KeyRemapper> keyRemapper;
  std::unordered_map<ushort, ushort> vkCodes;  // scancode -> vkCode
  Modifiers modifiers;
  bool capslock = false;
  int delayUntilRepeat = 250;
  int keyRepeatInterval = 25;
  dispatch_source_t keyRepeatTimer = nil;
  dispatch_source_t holdTimer = nil;
  dispatch_source_t delayTimer = nil;
  // Waiting for a delay to be over
  std::deque<KeyRemapper::KeyEvent> pendingKeyEvents;
  id appObserver = nil;  // of the frontmost app
  Mouse mouse{modifiers, postEvent};
  Keyboards keyboards;
  CFMachPortRef eventTap = NULL;
  CFRunLoopSourceRef eventTapSource = NULL;

  // For the native events of the mouse and the keyboards
  StartResult startEventTap() {
    // NOTE: kCGEventTapOptionListenOnly does not fail when clicking the app's
    // menu bar but it doesnt let me modify the event
    eventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap,
                                kCGEventTapOptionDefault,
                                Mouse::eventMask | Keyboards::eventMask,
                                eventTapCb, this);
    if (!eventTap) return StartResultNoAccessibility;

    eventTapSource =
        CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    if (!eventTapSource) return StartResultEventTapFailed;

    // NOTE: kCFRunLoopDefaultMode has issues with clicking the app's menubar
    CFRunLoopAddSource(CFRunLoopGetMain(), eventTapSource,
                       kCFRunLoopCommonModes);
    return StartResultOk;
  }

  void stopEventTap() {
    if (eventTap) {
      CFMachPortInvalidate(eventTap);
      CFRelease(eventTap);
      eventTap = NULL;
    }

    if (eventTapSource) {
      CFRunLoopRemoveSource(CFRunLoopGetMain(), eventTapSource,
                            kCFRunLoopCommonModes);
      CFRelease(eventTapSource);
      eventTapSource = NULL;
    }
  }

  static CGEventRef eventTapCb(CGEventTapProxy proxy, CGEventType type,
                               CGEventRef event, void* refcon) {
    auto self = (Runtime*)refcon;

    // macOS disables the tap when the main thread is too slow to answer and
    // it doesn't come back on its own
    if (type == kCGEventTapDisabledByTimeout ||
        type == kCGEventTapDisabledByUserInput) {
      if (self->eventTap) CGEventTapEnable(self->eventTap, true);
      return event;
    }

    if (CGEventMaskBit(type) & Mouse::eventMask)
      self->mouse.updateNativeEvent(type, event);
    else if (self->keyboards.shouldDrop(type, event))
      return NULL;

    return event;
  }

  static std::string getFrontmostAppName() {
    auto app = NSWorkspace.sharedWorkspace.frontmostApplication;
    if (app.bundleIdentifier) return app.bundleIdentifier.UTF8String;
    if (app.localizedName) return app.localizedName.UTF8String;
    return "Unknown";
  }

  void fail(std::string err) {
    stop();
    onError(err);
  }

  // In order. A delay holds the events after it, also the ones that come
  // later, until it's over
  void postKeyEvents(const KeyRemapper::KeyEvents& keyEvents) {
    pendingKeyEvents.insert(pendingKeyEvents.end(), keyEvents.begin(),
                            keyEvents.end());
    if (delayTimer) return;

    while (!pendingKeyEvents.empty()) {
      auto keyEvent = pendingKeyEvents.front();
      pendingKeyEvents.pop_front();

      if (keyEvent.name == "delay") {
        delayTimer = startTimer(keyEvent.delay, 0, ^{
          stopTimer(delayTimer);
          postKeyEvents({});
        });
        return;
      }

      postKeyEvent(keyEvent);
    }
  }

  void postKeyEvent(const KeyRemapper::KeyEvent& keyEvent) {
    // Keys without a symbol or a Mac key have no vkCode
    auto it = vkCodes.find(keyEvent.code);
    if (it == vkCodes.end()) return;

    auto isKeyDown = keyEvent.isKeyDown;
    auto vkCode = it->second;

    if (!isKeyDown) stopTimer(keyRepeatTimer);

    if (vkCode == 55 || vkCode == 54) {
      modifiers.cmd = isKeyDown;
      postKey(vkCode, isKeyDown);
    } else if (vkCode == 56 || vkCode == 60) {
      modifiers.shift = isKeyDown;
      postKey(vkCode, isKeyDown);
    } else if (vkCode == 58 || vkCode == 61) {
      modifiers.alt = isKeyDown;
      postKey(vkCode, isKeyDown);
    } else if (vkCode == 59 || vkCode == 62) {
      modifiers.ctrl = isKeyDown;
      postKey(vkCode, isKeyDown);
    } else if (vkCode == 63) {
      modifiers.fn = isKeyDown;
      postKey(vkCode, isKeyDown);
    } else if (vkCode == 57) {
      if (isKeyDown) setCapslock(capslock = !capslock);
    } else if (vkCode == 241) {
      mouse.postClick(isKeyDown);
    } else if (vkCode == 242) {
      mouse.postClick(isKeyDown, true);
    } else if (Keys::isMedia(vkCode)) {
      if (isKeyDown) postMediaKey(vkCode);
      handleKeyRepeat(vkCode, isKeyDown && keyEvent.repeats &&
                                  Keys::isRepeatingMedia(vkCode));
    } else {
      postKey(vkCode, isKeyDown);
      handleKeyRepeat(vkCode, isKeyDown && keyEvent.repeats);
    }
  }

  void setModifierFlags(CGEventRef event, ushort vkCode) {
    CGEventFlags flags = modifiers.flags();
    if (capslock) flags |= kCGEventFlagMaskAlphaShift;

    if (Keys::isArrow(vkCode))
      flags |= kCGEventFlagMaskNumericPad | kCGEventFlagMaskSecondaryFn;
    else if (Keys::isFunction(vkCode))
      flags |= kCGEventFlagMaskSecondaryFn;
    else
      flags |= kCGEventFlagMaskNonCoalesced;

    CGEventSetFlags(event, flags);
  }

  static CGEventRef createMediaKeyEvent(ushort vkCode, bool isDown) {
    static const std::unordered_map<ushort, int> mediaKeys = {
        {300, NX_KEYTYPE_BRIGHTNESS_DOWN}, {301, NX_KEYTYPE_BRIGHTNESS_UP},
        {304, NX_KEYTYPE_ILLUMINATION_DOWN}, {305, NX_KEYTYPE_ILLUMINATION_UP},
        {306, NX_KEYTYPE_REWIND}, {307, NX_KEYTYPE_PLAY},
        {308, NX_KEYTYPE_FAST}, {309, NX_KEYTYPE_MUTE},
        {310, NX_KEYTYPE_SOUND_DOWN}, {311, NX_KEYTYPE_SOUND_UP},
    };
    auto it = mediaKeys.find(vkCode);
    int code = it == mediaKeys.end() ? 0 : it->second;

    NSEvent* nsEvent = [NSEvent
        otherEventWithType:NSEventTypeSystemDefined
                  location:NSMakePoint(0, 0)
             modifierFlags:0xa00
                 timestamp:0
              windowNumber:0
                   context:0
                   subtype:8
                     data1:(code << 16) | (isDown ? (0xa << 8) : (0xb << 8))
                     data2:-1];

    return CGEventCreateCopy([nsEvent CGEvent]);
  }

  void postKey(ushort vkCode, bool isKeyDown, bool isRepeat = false) {
    auto eventSource = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    auto event =
        CGEventCreateKeyboardEvent(eventSource, (CGKeyCode)vkCode, isKeyDown);
    setModifierFlags(event, vkCode);

    if (isRepeat)
      CGEventSetIntegerValueField(event, kCGKeyboardEventAutorepeat, 1);
    postEvent(event);
    CFRelease(eventSource);
    CFRelease(event);
  }

  void postMediaKey(ushort vkCode) {
    auto downEvent = createMediaKeyEvent(vkCode, true);
    auto upEvent = createMediaKeyEvent(vkCode, false);

    postEvent(downEvent);
    postEvent(upEvent);
    CFRelease(downEvent);
    CFRelease(upEvent);
  }

  // On the main queue. Without interval it fires once. The handlers capture
  // this, so every timer is stopped in stop(), before this object goes away
  static dispatch_source_t startTimer(int delayMs, int intervalMs,
                                      dispatch_block_t handler) {
    auto timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0,
                                        dispatch_get_main_queue());
    dispatch_source_set_timer(
        timer, dispatch_time(DISPATCH_TIME_NOW, delayMs * NSEC_PER_MSEC),
        intervalMs > 0 ? intervalMs * NSEC_PER_MSEC : DISPATCH_TIME_FOREVER,
        0);
    dispatch_source_set_event_handler(timer, handler);
    dispatch_resume(timer);
    return timer;
  }

  static void stopTimer(__strong dispatch_source_t& timer) {
    if (!timer) return;
    dispatch_source_cancel(timer);
    timer = nil;
  }

  // Like a keyboard, a key event stops the repeat of the previous key
  void handleKeyRepeat(ushort vkCode, bool shouldRepeat) {
    stopTimer(keyRepeatTimer);
    if (!shouldRepeat) return;

    keyRepeatTimer = startTimer(delayUntilRepeat, keyRepeatInterval, ^{
      if (Keys::isMedia(vkCode))
        postMediaKey(vkCode);
      else
        postKey(vkCode, true, true);
    });
  }

  // For the hold of the key that was just pressed. Any key event
  // before it fires cancels it
  void startHoldTimer() {
    auto delay = keyRemapper->getHoldDelay();
    if (delay < 0) return;

    holdTimer = startTimer(delay, 0, ^{
      stopTimer(holdTimer);
      try {
        postKeyEvents(keyRemapper->applyHold());
      } catch (const std::exception& err) {
        fail("ApplyHoldError: " + std::string(err.what()));
      } catch (...) {
        fail("ApplyHoldError: Unknown error");
      }
    });
  }
};
