import AppKit

class Background {
  var keyRemapperWrapper: KeyRemapperWrapper?

  init() {
    // TODO: KappaPride
    let rootPath = "/Users/erwin.gaitan/keyRemapperMac";
    setFrontmostAppNameToKeyRemapper()

    keyRemapperWrapper = KeyRemapperWrapper(rootPath, withMode: 0);
    MyHIDManager.start()
    MyHIDManager.onIOHIDKeyboardInput = handleIOHIDKeyboardInput

    NSWorkspace.shared.notificationCenter.addObserver(forName: NSWorkspace.didActivateApplicationNotification, object: nil, queue: nil, using: handleAppChange)

  }

  func handleIOHIDKeyboardInput(_ scancode: UInt32, _ isKeyDown: Bool, _ vendorId: Int, _ productId: Int, _ manufacturer: String, _ product: String) {
    let keyboard = String(productId) + ":" + String(vendorId)

    keyRemapperWrapper?.applyKeyEvent(
      Int32(scancode),
      state: isKeyDown ? 0 : 1,
      keyboard: keyboard,
      keyboardDescription: manufacturer + " | " + product
    );
  }

  func setFrontmostAppNameToKeyRemapper() {
    let frontmostApp = NSWorkspace.shared.frontmostApplication
    let bundleId = frontmostApp?.bundleIdentifier
    let localizedName = frontmostApp?.localizedName

    keyRemapperWrapper?.setAppName((bundleId != nil) ? bundleId : (localizedName != nil) ? localizedName : "Unknown")
  }

  func handleAppChange(notification: Notification) {
    setFrontmostAppNameToKeyRemapper()
  }
}
