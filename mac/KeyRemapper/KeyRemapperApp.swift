import SwiftUI
import UserNotifications

@main
struct KeyRemapperApp: App {
  @NSApplicationDelegateAdaptor(AppDelegate.self) fileprivate var appDelegate

    var body: some Scene {
      WindowGroup {}
    }
}

fileprivate class AppDelegate: NSObject, NSApplicationDelegate {
  // TODO: Once you move all config to one file, u wont need this
  var configIdx = 0

  var daemonStarted = false
  
  lazy var statusBarItem: NSStatusItem? = { NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)
  }()
  
  lazy private var aboutBoxWindowController: NSWindowController? = {
    let aboutView = AboutView()
      let styleMask: NSWindow.StyleMask = [.closable, .miniaturizable, .titled]
      let window = NSWindow()
      window.styleMask = styleMask
      window.title = "About \(BUNDLE_NAME)"
      window.contentView = NSHostingView(rootView: aboutView)
      return NSWindowController(window: window)
  }()
  
  var _daemonRemoteObject: ServiceProviderXPCProtocol?
  var daemonRemoteObject: ServiceProviderXPCProtocol? {
    get {
      if _daemonRemoteObject != nil {
        return _daemonRemoteObject
      }
      
      let connection = NSXPCConnection(machServiceName: MACH_SERVICE_NAME, options: [.privileged])

          connection.remoteObjectInterface = NSXPCInterface(with: ServiceProviderXPCProtocol.self)
          connection.resume()

      connection.interruptionHandler = {
          print("Connection with Daemon interrupted")
          self._daemonRemoteObject = nil
      }
      connection.invalidationHandler = {
          print("Connection with Daemon invalidated")
          self._daemonRemoteObject = nil
      }
      
      _daemonRemoteObject = connection.synchronousRemoteObjectProxyWithErrorHandler { error in
        print("Error:", error)
      } as? ServiceProviderXPCProtocol
      return _daemonRemoteObject
    }
  }

    func applicationDidFinishLaunching(_ notification: Notification) {
      if let window = NSApplication.shared.windows.first { window.close() }

      startDaemon(configIdx: 0)
    }


  func showNotification(_ description: String, _ title: String?) {
    let center = UNUserNotificationCenter.current()
    let options: UNAuthorizationOptions = [.alert, .badge]
    center.requestAuthorization(options: options) { (granted, error) in
        if granted {
          let content = UNMutableNotificationContent()
          content.title = title ?? BUNDLE_NAME
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

  @objc func reloadMenuBar() {
    guard let statusBarItem = statusBarItem else { return }
    guard let menuButton = statusBarItem.button else { return }
    guard let iconImage = NSImage(named: "MenuBarIcon") else { return }

    menuButton.imagePosition = NSControl.ImagePosition.imageLeft
    menuButton.image = Util.resizeImage(image: iconImage, w: 16, h: 16)
    menuButton.image?.isTemplate = true
    menuButton.frame = CGRect(x: 0.0, y: 3, width: menuButton.frame.width, height: menuButton.frame.height)


    let statusBarItemMenu = NSMenu(title: "Status Bar Item Menu")
    statusBarItem.menu = statusBarItemMenu

    statusBarItemMenu.removeAllItems()
    statusBarItemMenu.addItem(
        withTitle: "Open Config Folder",
        action: #selector(AppDelegate.openConfigFolder),
        keyEquivalent: "")
    
    statusBarItemMenu.addItem(.separator())
    statusBarItemMenu.addItem(
        withTitle: daemonStarted ? "Stop Daemon" : "Start Daemon",
        action: #selector(AppDelegate.startOrStopDaemon),
        keyEquivalent: "")
    statusBarItemMenu.addItem(
        withTitle: "Uninstall Daemon",
        action: #selector(AppDelegate.uninstallDaemon),
        keyEquivalent: "")

    statusBarItemMenu.addItem(.separator())
    for i in 0...8 {
      guard let configPath = Util.getConfigPath(configIdx: i) else { break }

      if !Util.fileExists(configPath) { break }
      let configIdxName = String(i + 1)
      var configName = Util.getConfigName(configPath: configPath) ?? "Config \(configIdxName)"
      configName = i == configIdx ? "✔  \(configName)" : configName

      let menuItem = NSMenuItem(title: configName, action: #selector(AppDelegate.switchConfig(sender:)), keyEquivalent: configIdxName)
      menuItem.tag = i
      statusBarItemMenu.addItem(menuItem)
    }

    statusBarItemMenu.addItem(.separator())
    statusBarItemMenu.addItem(
        withTitle: "About \(BUNDLE_NAME)",
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

    startDaemon(configIdx: menuItem.tag)
  }

  func updateMenuBarTitle(_ title: String?) {
    if let title = title {
      statusBarItem?.button?.title = " \(title)"
    } else {
      statusBarItem?.button?.title = ""
    }
  }

  
  var _authRef: AuthorizationRef?
  var authRef: AuthorizationRef? {
    get {
      if _authRef != nil {
        return _authRef
      }
      
      _authRef = Util.getPrivilegedHelperAuth()
      return _authRef
    }
  }
  
  func startDaemon(configIdx: Int) {
    if (daemonStarted == true) {
      stopDaemon()
    } else {
      reloadMenuBar()
    }
    
      guard let authRef = authRef else {
        return showCloseAlert("Error", "Authorization required for Daemon")
      }
      
      let blessed = Util.blessHelper(label: MACH_SERVICE_NAME, authRef: authRef)

      if (!blessed) {
        return showCloseAlert("Error", "Not Blessed to run the Daemon")
      }

    self.configIdx = configIdx
    guard let symbolsPath = Util.getSymbolsPath() else { return }

    if (!Util.fileExists(symbolsPath)) {
      return showCloseAlert("File not found", "\(symbolsPath) not found")
    }

    guard let configPath = Util.getConfigPath(configIdx: configIdx) else {
      return showCloseAlert("No config", "No config path constructed")
    }

    if (!Util.fileExists(configPath)) {
      return showCloseAlert("File not found", "\(configPath) not found")
    }

    guard let daemonRemoteObject = self.daemonRemoteObject else {
      return showCloseAlert("Error", "No Daemon service remote object running")
    }

   var daemonVersion: String?
   daemonRemoteObject.getVersion { version in daemonVersion = version }
    
   if (daemonVersion != VERSION) {
     return showCloseAlert("Wrong Daemon version", "App \(VERSION) and Daemon \(daemonVersion ?? "(unknown)") Version Mismatch, Restart the Daemon")
   }

    var daemonStartResult: Int32?
    daemonRemoteObject.start(configPath: configPath, symbolsPath: symbolsPath) {
      result in daemonStartResult = result
    }

    if (daemonStartResult == 1) {
      self.showCloseAlert("Enable Accesibility", "Enable Accesibility for this app")

      IOHIDRequestAccess(kIOHIDRequestTypePostEvent)
      return
    }

    if (daemonStartResult != 0) {
      return self.showCloseAlert("Error", "Couldn't start Daemon process")
    }

    self.daemonStarted = true
    statusBarItem?.button?.appearsDisabled = false
    self.reloadMenuBar()
  }

  func stopDaemon() {
    if (daemonStarted != true) { return }
    statusBarItem?.button?.appearsDisabled = true
    daemonStarted = false
    daemonRemoteObject?.kill()
    reloadMenuBar()
  }
  
  @objc func uninstallDaemon() {
    stopDaemon()
    daemonRemoteObject?.uninstall()
  }

  @objc func startOrStopDaemon() {
    if (daemonStarted == true) {
      return stopDaemon()
    }

    startDaemon(configIdx: configIdx)
  }
    
  @objc func openConfigFolder() {
    guard let rootPath = Util.getRootPath() else { return }
    
    guard let resourcePath = Bundle.main.resourcePath else {
      return
    }
    
    do {
      guard let configPath = Util.getConfigPath(configIdx: 0) else { return }
      let configFileExists = Util.fileExists(configPath)
      
      if (!configFileExists) {
        try Util.copyFile(srcPath: "\(resourcePath)/config.json", to: configPath)
      }

      guard let symbolsPath = Util.getSymbolsPath() else { return }
      let symbolFileExists = Util.fileExists(symbolsPath)

      if (!symbolFileExists) {
        try Util.copyFile(srcPath: "\(resourcePath)/symbols.json", to: symbolsPath)
      }
    } catch let error {
      print("Error creating files: ", error)
    }
    
    NSWorkspace.shared.selectFile(nil, inFileViewerRootedAtPath: rootPath)
  }

  @objc func openAboutWindow() {
    NSApp.activate(ignoringOtherApps: true)
    aboutBoxWindowController?.window?.orderFrontRegardless()
    aboutBoxWindowController?.window?.center()
      aboutBoxWindowController?.showWindow(aboutBoxWindowController?.window)
  }

  @objc func quit() {
    daemonRemoteObject?.kill()
    NSApplication.shared.terminate(self)
  }
}
