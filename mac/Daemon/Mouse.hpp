#pragma once

#include <ApplicationServices/ApplicationServices.h>

#include <chrono>
#include <functional>

#include "./Keys.hpp"

// Posts the remapped clicks and, since real mouse events don't see the
// modifiers posted by the remapper, adds them to every native mouse event
class Mouse {
 public:
  double doubleClickSpeed = 500;

  Mouse(const Modifiers& modifiers,
        const std::function<void(CGEventRef)>& postEvent)
      : modifiers(modifiers), postEvent(postEvent) {}
  ~Mouse() { stop(); }

  // 1: no Accessibility permission, 2: couldn't add the event tap
  int start() {
    stop();

    // NOTE: kCGEventTapOptionListenOnly does not fail when clicking the app's
    // menu bar but it doesnt let me modify the event
    eventTap = CGEventTapCreate(
        kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
        CGEventMaskBit(kCGEventLeftMouseDown) |
            CGEventMaskBit(kCGEventLeftMouseUp) |
            CGEventMaskBit(kCGEventLeftMouseDragged) |
            CGEventMaskBit(kCGEventRightMouseDown) |
            CGEventMaskBit(kCGEventRightMouseUp) |
            CGEventMaskBit(kCGEventRightMouseDragged) |
            CGEventMaskBit(kCGEventMouseMoved),
        eventTapCb, this);

    if (!eventTap) return 1;

    runLoopSource =
        CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    if (!runLoopSource) return 2;

    // NOTE: kCFRunLoopDefaultMode has issues with clicking the app's menubar
    CFRunLoopAddSource(CFRunLoopGetMain(), runLoopSource,
                       kCFRunLoopCommonModes);
    return 0;
  }

  void stop() {
    if (eventTap) {
      CFMachPortInvalidate(eventTap);
      CFRelease(eventTap);
      eventTap = NULL;
    }

    if (runLoopSource) {
      CFRunLoopRemoveSource(CFRunLoopGetMain(), runLoopSource,
                            kCFRunLoopCommonModes);
      CFRelease(runLoopSource);
      runLoopSource = NULL;
    }
  }

  void postClick(bool isMouseDown, bool isRight = false) {
    CGEventRef locationEvent = CGEventCreate(NULL);
    CGPoint newLocation = CGEventGetLocation(locationEvent);
    CFRelease(locationEvent);

    if (isMouseDown) {
      double now = std::chrono::system_clock::now().time_since_epoch() /
                   std::chrono::milliseconds(1);

      if (isRightButton != isRight || newLocation.x != location.x ||
          newLocation.y != location.y || clickCount == 0 ||
          now - lastPressTime > doubleClickSpeed)
        clickCount = 1;
      else
        clickCount++;

      isRightButton = isRight;
      location = newLocation;
      lastPressTime = now;
    }

    status = isMouseDown ? Status::down : Status::up;

    auto eventType = isMouseDown
                         ? (isRight ? kCGEventRightMouseDown : kCGEventLeftMouseDown)
                         : (isRight ? kCGEventRightMouseUp : kCGEventLeftMouseUp);
    CGEventRef event = CGEventCreateMouseEvent(
        NULL, eventType, newLocation,
        isRight ? kCGMouseButtonRight : kCGMouseButtonLeft);
    CGEventSetIntegerValueField(event, kCGMouseEventClickState, clickCount);
    CGEventSetDoubleValueField(event, kCGEventSourceUserData, postedEventMark);
    CGEventSetFlags(event, modifiers.flags());
    postEvent(event);
    CFRelease(event);
  }

 private:
  enum class Status { none, down, up };

  // Tells the clicks posted here apart from the native mouse events
  static constexpr double postedEventMark = 69;

  const Modifiers& modifiers;
  const std::function<void(CGEventRef)>& postEvent;
  CFMachPortRef eventTap = NULL;
  CFRunLoopSourceRef runLoopSource = NULL;

  int clickCount = 0;
  CGPoint location = {};
  double lastPressTime = 0;
  bool isRightButton = false;
  Status status = Status::none;

  static CGEventRef eventTapCb(CGEventTapProxy proxy, CGEventType type,
                               CGEventRef event, void* refcon) {
    auto self = (Mouse*)refcon;

    // macOS disables the tap when the main thread is too slow to answer and
    // it doesn't come back on its own
    if (type == kCGEventTapDisabledByTimeout ||
        type == kCGEventTapDisabledByUserInput) {
      if (self->eventTap) CGEventTapEnable(self->eventTap, true);
      return event;
    }

    if (CGEventGetDoubleValueField(event, kCGEventSourceUserData) !=
        postedEventMark)
      self->updateNativeEvent(event, type);

    return event;
  }

  // While a remapped click is held, moving the mouse has to drag
  void updateNativeEvent(CGEventRef event, CGEventType type) {
    bool isMoving = type == kCGEventMouseMoved ||
                    type == kCGEventLeftMouseDragged ||
                    type == kCGEventRightMouseDragged;

    if (isMoving && status == Status::down) {
      CGEventSetType(event, isRightButton ? kCGEventRightMouseDragged
                                          : kCGEventLeftMouseDragged);
    } else if (isMoving && status == Status::up) {
      CGEventSetType(event, kCGEventMouseMoved);
      status = Status::none;
    }

    CGEventSetFlags(event, modifiers.flags());
  }
};
