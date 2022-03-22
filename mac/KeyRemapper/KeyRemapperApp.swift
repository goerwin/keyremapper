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
  var statusBarItem: NSStatusItem?
    var rootPath: String?
  var rootPathWithTilde: String?
    var configIdx = 1
  var keyRemapperStarted: Bool?
  var keyRemapperWrapper: KeyRemapperWrapper?


    func applicationDidFinishLaunching(_ notification: Notification) {
      if let window = NSApplication.shared.windows.first { window.close() }

      reloadMenuBar()

      guard let rootPathInfo = getRootPathInfo() else {
        return showCloseAlert("Error", "Config Folder invalid")
      }
      
      rootPath = rootPathInfo[0]
      rootPathWithTilde = rootPathInfo[1]

      if (!isAppAccesibilityEnabled()) {
        showCloseAlert("Enable Accesibility", "Enable Accesibility for this app")
        IOHIDRequestAccess(kIOHIDRequestTypePostEvent)
        return;
      }

      NSWorkspace.shared.notificationCenter.addObserver(forName: NSWorkspace.didActivateApplicationNotification, object: nil, queue: nil, using: handleAppChange)
      
      startKeyRemapper(configIdx: 0)
      reloadMenuBar()
    }
  
  func getConfigPathInfo(configIdx: Int) -> [String]? {
    guard let rootPath = rootPath, let rootPathWithTilde = rootPathWithTilde else { return nil }
    
    let idxStr = configIdx == 0 ? "" : String(configIdx + 1)

    return [
      "\(rootPath)/config\(idxStr).json",
      "\(rootPathWithTilde)/config\(idxStr).json"
    ]
  }
  
  func startKeyRemapper(configIdx: Int) {
    if (keyRemapperStarted == true) { return }
    guard let rootPath = rootPath else { return }
    
    self.configIdx = configIdx
    let symbolsPath = "\(rootPath)/symbols.json"
    
    if (!fileExists(symbolsPath)) {
      return showCloseAlert("File not found", "\(symbolsPath) not found")
    }

    guard let configPathInfo = getConfigPathInfo(configIdx: configIdx) else {
      return showCloseAlert("No config", "No config path constructed")
    }
    
    let configPath = configPathInfo[0]
    let configPathWithTilde = configPathInfo[1]
    
    if (!fileExists(configPath)) {
      return showCloseAlert("File not found", "\(configPath) not found")
    }

    keyRemapperWrapper = KeyRemapperWrapper(configPath, withSymbolsPath: symbolsPath)
    guard let keyRemapperWrapper = keyRemapperWrapper else { return }
 
    guard let testsResults = keyRemapperWrapper.runTests(configPath, withSymbolsPath: symbolsPath) else { return }
    
    let configName = getConfigName(configPath: configPath)
        
    showNotification("\(testsResults)\nConfig name: \(configName ?? "No name")\nConfig file: \(configPathWithTilde)", nil)
    setFrontmostAppNameToKeyRemapper()
    updateMenuBarTitle(configName)
    
    // NOTE: Run HIDManager in background to not block the main thread
    DispatchQueue.global(qos: .background).async {
      MyHIDManager.start()
      MyHIDManager.onIOHIDKeyboardInput = self.handleIOHIDKeyboardInput
    }

    keyRemapperStarted = true
    statusBarItem?.button?.appearsDisabled = false
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

  @objc func reloadMenuBar() {
    if (statusBarItem == nil) {
      statusBarItem = NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)
    }
    
    guard let statusBarItem = statusBarItem else { return }
    guard let menuButton = statusBarItem.button else { return }
    guard let iconImage = NSImage(named: "MenuBarIcon") else { return }

    menuButton.imagePosition = NSControl.ImagePosition.imageLeft
    menuButton.image = resizeImage(image: iconImage, w: 16, h: 16)
    menuButton.image?.isTemplate = true
    menuButton.frame = CGRect(x: 0.0, y: 3, width: menuButton.frame.width, height: menuButton.frame.height)

    let statusBarItemMenu = NSMenu(title: "Status Bar Item Menu")
    statusBarItem.menu = statusBarItemMenu
    
    statusBarItemMenu.removeAllItems()
    statusBarItemMenu.addItem(
        withTitle: "Open Config Folder",
        action: #selector(AppDelegate.openConfigFolder),
        keyEquivalent: "")
    statusBarItemMenu.addItem(
        withTitle: "Disable/Enable",
        action: #selector(AppDelegate.disableEnableKeyRemapper),
        keyEquivalent: "")
    statusBarItemMenu.addItem(
        withTitle: "Reload Menu",
        action: #selector(AppDelegate.reloadMenuBar),
        keyEquivalent: "")

    statusBarItemMenu.addItem(.separator())
    for i in 0...8 {
      guard let configPathInfo = getConfigPathInfo(configIdx: i) else { break }
      
      let configPath = configPathInfo[0]
      if !fileExists(configPath) { break }
      let configIdxName = String(i + 1)
      let configName = getConfigName(configPath: configPath) ?? "Config \(configIdxName)"
      
      let menuItem = NSMenuItem(title: configName, action: #selector(AppDelegate.switchConfig(sender:)), keyEquivalent: configIdxName)
      menuItem.tag = i
      statusBarItemMenu.addItem(menuItem)
    }
    
    statusBarItemMenu.addItem(.separator())
    statusBarItemMenu.addItem(
        withTitle: "About \(bundleName)",
        action: #selector(AppDelegate.openAboutWindow),
        keyEquivalent: "")

    statusBarItemMenu.addItem(.separator())
    statusBarItemMenu.addItem(
        withTitle: "Quit",
        action: #selector(AppDelegate.quit),
        keyEquivalent: "q")
  }
  
  @objc func switchConfig(sender: Any) {
    guard let menuItem = sender as? NSMenuItem else { return }
    
    stopKeyRemapper()
    startKeyRemapper(configIdx: menuItem.tag)
  }
  
  func updateMenuBarTitle(_ title: String?) {
    if let title = title {
      statusBarItem?.button?.title = " \(title)"
    } else {
      statusBarItem?.button?.title = ""
    }
  }


  func getCurLoggedInUserFromRoot() -> String? {
    var uid: uid_t = 0
    var gid: gid_t = 0

    guard let name = SCDynamicStoreCopyConsoleUser(nil, &uid, &gid) else {
      return nil
    }

    return name as String;
  }

  func getRootPathInfo() -> [String]? {
    // Since we are running from root, the current user is root so
    // I have to use this to get the actual logged in user
    guard let currentLoggedInUser = getCurLoggedInUserFromRoot() else { return nil }
    let folderName = "keyRemapperMac"

    return [
      "/Users/\(currentLoggedInUser)/\(folderName)",
      "~/\(folderName)"
    ]
  }
  
  func getConfigName(configPath: String) -> String? {
    do {
        let data = try Data(contentsOf: URL(fileURLWithPath: configPath), options: .mappedIfSafe)
      let jsonResult = try JSONSerialization.jsonObject(with: data, options: .mutableLeaves)
      if let jsonResult = jsonResult as? Dictionary<String, AnyObject>, let name = jsonResult["name"] as? String {
          return name
      }
    } catch {}
    
    return nil
  }
  
  func stopKeyRemapper() {
    if (keyRemapperStarted != true) { return }
    keyRemapperWrapper?.terminate()
    MyHIDManager.stop();
    keyRemapperWrapper = nil
    statusBarItem?.button?.appearsDisabled = true
    keyRemapperStarted = false
  }
  
  @objc func disableEnableKeyRemapper() {
    if (keyRemapperStarted == true) {
      return stopKeyRemapper()
    }

    startKeyRemapper(configIdx: configIdx)
  }

  @objc func openConfigFolder() {
    guard let rootPath = rootPath else { return }
    NSWorkspace.shared.selectFile(nil, inFileViewerRootedAtPath: rootPath)
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
