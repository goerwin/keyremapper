#pragma once

#import "AppKit/AppKit.h"
#import "Foundation/Foundation.h"

#import "IOKit/hidsystem/ev_keymap.h"

#import "../../common/Helpers.hpp"
#import "../../common/KeyRemapper.hpp"

#import "./Keyboards.hpp"
#import "./Keys.hpp"
#import "./Mouse.hpp"

#include <memory>
#include <thread>
#include <unordered_map>

// Remapping runtime shared by the daemon and the dev runner: listens to the
// keyboards via IOKit, applies the KeyRemapper rules and posts the results.
// Everything runs on the main run loop
class Runtime {
 public:
  std::function<void(std::string)> onError = [](std::string) {};

  // Replaceable so the tests can record what would reach the OS
  std::function<void(CGEventRef)> postEvent = [](CGEventRef event) {
    CGEventPost(kCGHIDEventTap, event);
  };
  std::function<void(bool)> setCapslock = Capslock::setState;

  ~Runtime() { stop(); }

  // 1: no Accessibility permission, 2: couldn't listen to the mouse,
  // 3: error sent to onError (eg. invalid config), 5: invalid profiles,
  // 6: invalid profile. It stays stopped when it fails
  int start(std::string configPath, std::string symbolsPath, int profileIdx,
            std::string appName) {
    try {
      auto result = load(configPath, symbolsPath, profileIdx, appName);
      if (result == 0) result = mouse.start();
      if (result != 0) {
        stop();
        return result;
      }

      capslock = Capslock::getState();
      keyboards.onInput = [this](ushort scancode, bool isKeyDown, int vendorId,
                                 int productId, std::string manufacturer,
                                 std::string product) {
        handleInput(scancode, isKeyDown, vendorId, productId, manufacturer,
                    product);
      };
      keyboards.start();
      return 0;
    } catch (const std::exception& err) {
      onError("StartError: " + std::string(err.what()));
    } catch (...) {
      onError("StartError: Unknown error");
    }

    stop();
    return 3;
  }

  // Loads the profile without touching the keyboards or the mouse. Throws on
  // invalid files
  int load(std::string configPath, std::string symbolsPath, int profileIdx,
           std::string appName) {
    stop();

    auto config = Helpers::getJsonFile(configPath);
    auto symbols = Helpers::getJsonFile(symbolsPath);
    auto profiles = config["profiles"];

    if (!profiles.is_array()) return 5;
    auto profile = profiles.at(profileIdx);
    if (!profile.is_object()) return 6;

    // { "keyName": [scanCode, keyDownState, keyUpState, vkCode] }
    for (auto& [key, value] : symbols.items())
      vkCodes[value[0].get<ushort>()] = value[3].get<ushort>();

    keyRemapper = std::make_unique<KeyRemapper>(profile, symbols);
    keyRemapper->setAppName(appName);

    delayUntilRepeat = profile.value("delayUntilRepeat", 250);
    keyRepeatInterval = profile.value("keyRepeatInterval", 25);
    mouse.doubleClickSpeed = profile.value("doubleClickSpeed", 500.0);
    return 0;
  }

  void stop() {
    stopKeyRepeat();
    keyboards.stop();
    mouse.stop();
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
      auto keyEvents = keyRemapper->applyKeys(
          {{"", scancode, ushort(isKeyDown ? 0 : 1), false}});

      for (auto& keyEvent : keyEvents) {
        if (keyEvent.name == "SK:Delay") {
          std::this_thread::sleep_for(
              std::chrono::milliseconds(keyEvent.state));
          continue;
        }

        auto isKeyDown = keyEvent.isKeyDown;
        auto vkCode = getVkCode(keyEvent.code);

        if (!isKeyDown) stopKeyRepeat();

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
          handleKeyRepeat(vkCode, isKeyDown);
        } else {
          postKey(vkCode, isKeyDown);
          handleKeyRepeat(vkCode, isKeyDown);
        }
      }
    } catch (const std::exception& err) {
      onError("ApplyKeysError: " + std::string(err.what()));
    } catch (...) {
      onError("ApplyKeysError: Unknown error");
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
  Mouse mouse{modifiers, postEvent};
  Keyboards keyboards;

  ushort getVkCode(ushort scancode) {
    auto it = vkCodes.find(scancode);
    return it == vkCodes.end() ? 0 : it->second;
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

  void stopKeyRepeat() {
    if (!keyRepeatTimer) return;
    dispatch_source_cancel(keyRepeatTimer);
    keyRepeatTimer = nil;
  }

  void handleKeyRepeat(ushort vkCode, bool isKeyDown) {
    stopKeyRepeat();
    if (!isKeyDown) return;

    keyRepeatTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0,
                                            dispatch_get_main_queue());
    dispatch_source_set_timer(
        keyRepeatTimer,
        dispatch_time(DISPATCH_TIME_NOW, delayUntilRepeat * NSEC_PER_MSEC),
        keyRepeatInterval * NSEC_PER_MSEC, 0);
    // The timer is cancelled in stop(), before this object goes away
    dispatch_source_set_event_handler(keyRepeatTimer, ^{
      if (Keys::isMedia(vkCode))
        postMediaKey(vkCode);
      else
        postKey(vkCode, true, true);
    });
    dispatch_resume(keyRepeatTimer);
  }
};
