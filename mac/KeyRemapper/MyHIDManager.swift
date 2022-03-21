import Foundation
import IOKit

@objc class MyHIDManager: NSObject {
  @objc static var capslockState = false
  private static var ioHidManager: IOHIDManager?

  private static let inputValueCb: IOHIDValueCallback = {
      (context, result, sender, value) in
      let elem: IOHIDElement = IOHIDValueGetElement(value)
      let scancode = IOHIDElementGetUsage(elem)

      let pressed: CLong = IOHIDValueGetIntegerValue(value);
      let isKeyDown = pressed == 1;

      if (scancode < 3 || scancode > 235) {
        return;
      }

    let ioHIDDeviceRef = sender;
    var vendorId: Int = 0, productId: Int = 0;
    var manufacturer = "-", product = "-";
    let device = IOHIDElementGetDevice(elem)

    let vendorIdRef = IOHIDDeviceGetProperty(device, kIOHIDVendorIDKey as CFString);
    if (vendorIdRef != nil) { vendorId = vendorIdRef as! Int }

    let productIdRef = IOHIDDeviceGetProperty(device, kIOHIDProductIDKey as CFString);
    if (productIdRef != nil) { productId = productIdRef as! Int }

    let manufacturerRef = IOHIDDeviceGetProperty(device, kIOHIDManufacturerKey as CFString);
    if (manufacturerRef != nil) { manufacturer = manufacturerRef as! String }

    let productRef = IOHIDDeviceGetProperty(device, kIOHIDProductKey as CFString);
    if (productRef != nil) { product = productRef as! String }

    onIOHIDKeyboardInput(scancode, isKeyDown, vendorId, productId, manufacturer, product);
  }

  static var onIOHIDKeyboardInput: (_ scancode: UInt32, _ isKeyDown: Bool, _ vendorId: Int, _ productId: Int, _ manufacturer: String, _ product: String) -> Void = {_,_,_,_,_,_ in };

  static func start() {
    ioHidManager = IOHIDManagerCreate(kCFAllocatorDefault, IOOptionBits(kIOHIDOptionsTypeNone))
    
    if (CFGetTypeID(ioHidManager) != IOHIDManagerGetTypeID()) {
      return;
    }
    
    let keyboard = [kIOHIDDeviceUsagePageKey: 0x01, kIOHIDDeviceUsageKey: 6] as CFDictionary
    let keypad = [kIOHIDDeviceUsagePageKey: 0x01, kIOHIDDeviceUsageKey: 7] as CFDictionary

    let devices = [keyboard, keypad] as CFArray

    IOHIDManagerSetDeviceMatchingMultiple(ioHidManager!, devices)

    IOHIDManagerRegisterInputValueCallback(ioHidManager!, inputValueCb, nil);

    IOHIDManagerScheduleWithRunLoop(ioHidManager!, CFRunLoopGetMain(), CFRunLoopMode.commonModes.rawValue)
    IOHIDManagerOpen(ioHidManager!, IOOptionBits(kIOHIDOptionsTypeSeizeDevice));

    // Capslock state
    capslockState = getCapslockState()
  }

  static func stop() {
    IOHIDManagerRegisterInputValueCallback(ioHidManager!, nil, nil);
    IOHIDManagerSetDeviceMatchingMultiple(ioHidManager!, nil);
    IOHIDManagerUnscheduleFromRunLoop(ioHidManager!, CFRunLoopGetMain(), CFRunLoopMode.commonModes.rawValue);
    IOHIDManagerClose(ioHidManager!, IOOptionBits(kIOHIDOptionsTypeSeizeDevice));
  }

  static func getCapslockState() -> Bool {
    var kr: kern_return_t = 0
    var ioc: io_connect_t = 0

    let mdict: CFMutableDictionary = IOServiceMatching(kIOHIDSystemClass)
    let ios = IOServiceGetMatchingService(kIOMasterPortDefault, mdict)

    kr = IOServiceOpen(ios, mach_task_self_, UInt32(kIOHIDParamConnectType), &ioc);
          IOObjectRelease(ios);
          if (kr != KERN_SUCCESS) {
            print("IOServiceOpen() failed: " + String(kr))
            return false;
          }

        var state: Bool = false;

    kr = IOHIDGetModifierLockState(ioc, Int32(kIOHIDCapsLockState), &state);
          if (kr != KERN_SUCCESS) {
            IOServiceClose(ioc);
            print("IOHIDGetModifierLockState() failed: " + String(kr))
            return false;
          }

          return state;
    }

  @objc static func toggleCapslockState() {
  var ioc: io_connect_t = 0
    let mdict: CFMutableDictionary = IOServiceMatching(kIOHIDSystemClass)
    let ios = IOServiceGetMatchingService(kIOMasterPortDefault, mdict)

      // I had to keep track of capslock on a global variable.
      // When I was calling getModifierLockState and then toggling via setModifierLockState, it was working the first time, but subsequent get calls return the same state
      capslockState = !capslockState;

      IOServiceOpen(ios, mach_task_self_, UInt32(kIOHIDParamConnectType), &ioc);
    IOHIDSetModifierLockState(ioc, Int32(kIOHIDCapsLockState), capslockState);
      IOObjectRelease(ios);
    IOServiceClose(ioc);
  }
}
