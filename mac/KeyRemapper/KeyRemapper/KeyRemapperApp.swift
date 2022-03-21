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
    var background: Background!
    var rootPath: String?

    func applicationDidFinishLaunching(_ notification: Notification) {
      if let window = NSApplication.shared.windows.first { window.close() }

      createMenuBar()
      
      guard let rootPath = getRootPath() else {
        return showCloseAlert("Error", "Config Folder invalid")
      }
      
      if (!isAppAccesibilityEnabled()) {
        showCloseAlert("Enable Accesibility", "Enable Accesibility for this app")
        IOHIDRequestAccess(kIOHIDRequestTypePostEvent)
        return;
      }
      
      let symbolsPath = "\(rootPath)/symbols.json"
      if (!fileExists(symbolsPath)) {
        return showCloseAlert("File not found", "\(symbolsPath) not found")
      }
      
      let config1Path = "\(rootPath)/config1.json"
      if (!fileExists(config1Path)) {
        return showCloseAlert("File not found", "\(config1Path) not found")
      }
      
      showNotification("Config loaded from: \(config1Path)", nil)
//      sendNotification(selectedMode + " selected" + testsResultsMsg + "\n" + "Config files in: " + rootPathStr);
      self.rootPath = rootPath
//      startBgProcess(configPath: config1Path, symbolsPath: symbolsPath)
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
  
    let trigger = UNTimeIntervalNotificationTrigger(timeInterval: 1, repeats: false);

          let request = UNNotificationRequest(identifier: UUID().uuidString, content: content, trigger: trigger)
  
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

  func startBgProcess(configPath: String, symbolsPath: String) {
    DispatchQueue.global(qos: .background).async {
      self.background = Background(configPath: configPath, symbolsPath: symbolsPath)
      }
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
    menuButton.title = " Config 1"
    menuButton.imagePosition = NSControl.ImagePosition.imageLeft
    menuButton.image = resizeImage(image: NSImage(named: "MenuBarIcon")!, w: 18, h: 18)
    menuButton.frame = CGRect(x: 0.0, y: 3, width: menuButton.frame.width, height: menuButton.frame.height)

    statusBarItem.menu = statusBarMenu

    statusBarMenu.addItem(
        withTitle: "Open Config Folder",
        action: #selector(AppDelegate.openConfigFolder),
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
