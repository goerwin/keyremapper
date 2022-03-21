import SwiftUI
import SystemConfiguration
import UserNotifications

// TODO: MOVE THIS BRO
let bundleName = Bundle.main.infoDictionary?["CFBundleName"] as! String
let version = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as! String

@main
struct KeyRemapperApp: App {
  @NSApplicationDelegateAdaptor(AppDelegate.self) fileprivate var appDelegate

    var body: some Scene {
      WindowGroup {}
    }
}

fileprivate class AppDelegate: NSObject, NSApplicationDelegate {
    var popover = NSPopover.init()
    var statusBarItem: NSStatusItem!
    var statusBarMenu: NSMenu!
    var rootPath: String?
    var configPath: String!
    var configIdx = 1
  var keyRemapperStarted: Bool?
  var keyRemapperWrapper: KeyRemapperWrapper?


    func applicationDidFinishLaunching(_ notification: Notification) {
      if let window = NSApplication.shared.windows.first { window.close() }

      createMenuBar()

      guard let rootPath = getRootPath() else {
        return showCloseAlert("Error", "Config Folder invalid")
      }
      
      self.rootPath = rootPath

      if (!isAppAccesibilityEnabled()) {
        showCloseAlert("Enable Accesibility", "Enable Accesibility for this app")
        IOHIDRequestAccess(kIOHIDRequestTypePostEvent)
        return;
      }

      NSWorkspace.shared.notificationCenter.addObserver(forName: NSWorkspace.didActivateApplicationNotification, object: nil, queue: nil, using: handleAppChange)
      
      startKeyRemapper()
    }
  
  func startKeyRemapper(configIdx: Int = 1) {
    if (keyRemapperStarted == true) {
      return
    }
    
    self.configIdx = configIdx
    let symbolsPath = "\(rootPath!)/symbols.json"
    
    if (!fileExists(symbolsPath)) {
      return showCloseAlert("File not found", "\(symbolsPath) not found")
    }

    configPath = "\(rootPath!)/config\(configIdx == 1 ? "" : String(configIdx)).json"
    if (!fileExists(configPath)) {
      return showCloseAlert("File not found", "\(configPath!) not found")
    }

    keyRemapperWrapper = KeyRemapperWrapper(configPath, withSymbolsPath: symbolsPath)
    let testsResults = keyRemapperWrapper!.runTests(configPath, withSymbolsPath: symbolsPath)!
    
    let configName = getConfigName(configPath: configPath!)
    
    showNotification("\(testsResults)\nConfig name: \(configName)\nConfig file: \(configPath!)", nil)
    
    setFrontmostAppNameToKeyRemapper()
    updateMenuBarTitle(configName)
    
    // NOTE: Run HIDManager in background to not block the main thread
    DispatchQueue.global(qos: .background).async {
      MyHIDManager.start()
      MyHIDManager.onIOHIDKeyboardInput = self.handleIOHIDKeyboardInput
    }

    keyRemapperStarted = true
    statusBarItem.button?.appearsDisabled = false
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
  
  func fileExists(_ filePath: String) -> Bool {
      let fileManager = FileManager.default
      if fileManager.fileExists(atPath: filePath) {
        return true
      }

      return false
  }

  func showNotification(_ description: String, _ title: String?) {
    let center = UNUserNotificationCenter.current()
    let options: UNAuthorizationOptions = [.alert, .badge]
    center.requestAuthorization(options: options) { (granted, error) in
        if granted {
          let content = UNMutableNotificationContent()
          content.title = title ?? bundleName
          content.body = description
          let request = UNNotificationRequest(identifier: UUID().uuidString, content: content, trigger: nil)

          center.add(request)
        }
    }

  }

  func showCloseAlert(_ title: String, _ description: String) {
          let alert = NSAlert()
          alert.messageText = title
          alert.informativeText = description
        alert.alertStyle = NSAlert.Style.warning
          alert.addButton(withTitle: "OK")
    alert.runModal()
   }

  func isAppAccesibilityEnabled() -> Bool {
    let options = [kAXTrustedCheckOptionPrompt.takeUnretainedValue() as NSString: false]
    return AXIsProcessTrustedWithOptions(options as CFDictionary)
  }

  func resizeImage(image: NSImage, w: Int, h: Int) -> NSImage? {
    let frame = NSRect(x: 0, y: 0, width: w, height: h)
    let newSize = NSSize(width: w, height: h)

    guard let representation = image.bestRepresentation(for: frame, context: nil, hints: nil) else {
        return nil
    }
    let image = NSImage(size: newSize, flipped: false, drawingHandler: { (_) -> Bool in
        return representation.draw(in: frame)
    })

    return image
  }

  private var aboutBoxWindowController: NSWindowController?

  func createMenuBar() {
    statusBarMenu = NSMenu(title: "Cap Status Bar Menu")
    statusBarItem = NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)

    let menuButton = statusBarItem.button!
    menuButton.imagePosition = NSControl.ImagePosition.imageLeft
    menuButton.image = resizeImage(image: NSImage(named: "MenuBarIcon")!, w: 18, h: 18)
    menuButton.frame = CGRect(x: 0.0, y: 3, width: menuButton.frame.width, height: menuButton.frame.height)

    statusBarItem.menu = statusBarMenu

    statusBarMenu.addItem(
        withTitle: "Open Config Folder",
        action: #selector(AppDelegate.openConfigFolder),
        keyEquivalent: "")
    statusBarMenu.addItem(
        withTitle: "Disable/Enable",
        action: #selector(AppDelegate.disableEnableKeyRemapper),
        keyEquivalent: "")

    statusBarMenu.addItem(.separator())
    statusBarMenu.addItem(
        withTitle: "Config 1",
        action: #selector(AppDelegate.switchConfig(sender:)),
        keyEquivalent: "")
    
    statusBarMenu.addItem(
        withTitle: "Config 2",
        action: #selector(AppDelegate.switchConfig),
        keyEquivalent: "")
    
    statusBarMenu.addItem(.separator())
    statusBarMenu.addItem(
        withTitle: "About \(bundleName)",
        action: #selector(AppDelegate.openAboutWindow),
        keyEquivalent: "")

    statusBarMenu.addItem(.separator())
    statusBarMenu.addItem(
        withTitle: "Quit",
        action: #selector(AppDelegate.quit),
        keyEquivalent: "q")
  }
  
  @objc func switchConfig(sender: Any) {
    stopKeyRemapper()
    startKeyRemapper(configIdx: 1)
  }
  
  func updateMenuBarTitle(_ title: String) {
    statusBarItem.button?.title = title
  }


  func getCurrentLoggedInUserFromRoot() -> String? {
    var uid: uid_t = 0
    var gid: gid_t = 0

    if let name = SCDynamicStoreCopyConsoleUser(nil, &uid, &gid) {
      return name as String;
    }

    return nil
  }

  func getRootPath() -> String? {
    // Since we are running from root, the current user is root so
    // I have to use this to get the actual logged in user
    let currentLoggedInUser = getCurrentLoggedInUserFromRoot();

    if currentLoggedInUser == nil { return nil }

    return "/Users/\(currentLoggedInUser!)/keyRemapperMac"
  }
  
  func getConfigName(configPath: String) -> String {
    do {
        let data = try Data(contentsOf: URL(fileURLWithPath: configPath), options: .mappedIfSafe)
      let jsonResult = try JSONSerialization.jsonObject(with: data, options: .mutableLeaves)
      if let jsonResult = jsonResult as? Dictionary<String, AnyObject>, let name = jsonResult["name"] as? String {

          return name
                    
      }
    } catch {}
    
    return ""
  }
  
  func stopKeyRemapper() {
    if (keyRemapperStarted != true) { return }
    keyRemapperWrapper!.terminate()
    MyHIDManager.stop();
    keyRemapperWrapper = nil
    statusBarItem.button?.appearsDisabled = true
    keyRemapperStarted = false
  }
  
  @objc func disableEnableKeyRemapper() {
    if (keyRemapperStarted == true) {
      return stopKeyRemapper()
    }

    startKeyRemapper(configIdx: configIdx)
  }

  @objc func openConfigFolder() {
    if (rootPath == nil) { return }
    NSWorkspace.shared.selectFile(nil, inFileViewerRootedAtPath: rootPath!)
  }

  @objc func openAboutWindow() {
          if aboutBoxWindowController == nil {
            let aboutView = AboutView();
              let styleMask: NSWindow.StyleMask = [.closable, .miniaturizable, .titled]
              let window = NSWindow()
              window.styleMask = styleMask
              window.title = "About \(bundleName)"
              window.contentView = NSHostingView(rootView: aboutView)
              aboutBoxWindowController = NSWindowController(window: window)
          }

    NSApp.activate(ignoringOtherApps: true)
    aboutBoxWindowController?.window?.orderFrontRegardless()
    aboutBoxWindowController?.window?.center()

          aboutBoxWindowController?.showWindow(aboutBoxWindowController?.window)
      }

  @objc func quit() {
    NSApplication.shared.terminate(self)
  }
}
