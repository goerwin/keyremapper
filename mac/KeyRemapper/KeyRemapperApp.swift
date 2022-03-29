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
  var daemonStarted = false
  
  lazy var statusBarItem: NSStatusItem? = { NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)
  }()
  
  var activeProfileIdx: Int?
  lazy private var aboutViewController: NSWindowController? = {
    let view = AboutView()
      let styleMask: NSWindow.StyleMask = [.closable, .miniaturizable, .titled]
      let window = NSWindow()
      window.styleMask = styleMask
      window.title = "About \(BUNDLE_NAME)"
      window.contentView = NSHostingView(rootView: view)
      return NSWindowController(window: window)
  }()
  
  lazy var loggerViewController: LoggerWindowController? = {
    let view = LoggerView(state: Global.state)
    let window = NSWindow()
    let styleMask: NSWindow.StyleMask = [.closable, .miniaturizable, .titled, .resizable]
    window.styleMask = styleMask
    window.title = "Logger"
    window.contentView = NSHostingView(rootView: view)
    let controller = LoggerWindowController(window: window)

    controller.onWindowDidBecomeMain = {
      self.daemonRemoteObject?.startLogging()
    }
    
    controller.onWindowWillClose = {
      self.daemonRemoteObject?.stopLogging()
      Global.state.resetLogStr()
    }
        
    window.delegate = controller
    return controller
  }()
  
  private var _daemonRemoteObject: ServiceProviderXPCProtocol?
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
      
      connection.exportedInterface = NSXPCInterface(with: AppProviderXPCProtocol.self)
      connection.exportedObject = AppProviderXPC()
      
      _daemonRemoteObject = connection.synchronousRemoteObjectProxyWithErrorHandler { error in
        print("Error:", error)
      } as? ServiceProviderXPCProtocol
      return _daemonRemoteObject
    }
  }

    func applicationDidFinishLaunching(_ notification: Notification) {
      if let window = NSApplication.shared.windows.first { window.close() }

      activeProfileIdx = Global.getJsonConfigActiveProfileIdx()
      startDaemon()
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

  @objc func reloadMenuBar() {
    guard let statusBarItem = statusBarItem else { return }
    guard let menuButton = statusBarItem.button else { return }
    guard let iconImage = NSImage(named: "MenuBarIcon") else { return }

    menuButton.imagePosition = NSControl.ImagePosition.imageLeft
    menuButton.image = Global.resizeImage(image: iconImage, w: 16, h: 16)
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
        withTitle: "Logger",
        action: #selector(AppDelegate.openLoggerWindow),
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

    // add profiles to menu
    if let jsonConfig = Global.getJsonConfig() {
      let activeProfileIdx = activeProfileIdx ?? 0
      let profiles = jsonConfig["profiles"] as? [Dictionary<String, AnyObject>] ?? []
      
      statusBarItemMenu.addItem(.separator())
      for (idx, profile) in profiles.enumerated() {
        let name = profile["name"] as? String
        let itemName = name ?? "Profile \(idx + 1)"
        let menuItem = NSMenuItem(
          title: idx == activeProfileIdx ? "✔  \(itemName)" : itemName,
          action: #selector(AppDelegate.switchProfile(sender:)),
          keyEquivalent: String(idx + 1)
        )
        menuItem.tag = idx
        
        if activeProfileIdx == idx {
          statusBarItem.button?.title = name != nil ? " \(itemName)" : ""
        }
        statusBarItemMenu.addItem(menuItem)
      }
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

  @objc func switchProfile(sender: Any) {
    guard let menuItem = sender as? NSMenuItem else { return }

    activeProfileIdx = menuItem.tag
    startDaemon(profileIdx: menuItem.tag)
  }
  
  var _authRef: AuthorizationRef?
  var authRef: AuthorizationRef? {
    get {
      if _authRef != nil {
        return _authRef
      }
      
      _authRef = Global.getPrivilegedHelperAuth()
      return _authRef
    }
  }
  
  func startDaemon(profileIdx: Int? = nil) {
    if (daemonStarted == true) {
      stopDaemon()
    } else {
      reloadMenuBar()
    }
    
      guard let authRef = authRef else {
        return Global.showCloseAlert("Error", "Authorization required for Daemon")
      }
      
      let blessed = Global.blessHelper(label: MACH_SERVICE_NAME, authRef: authRef)

      if (!blessed) {
        return Global.showCloseAlert("Error", "Not Blessed to run the Daemon")
      }
    
    guard let configPath = Global.getConfigPath() else {
      return Global.showCloseAlert("No config", "No config path constructed")
    }

    if (!Global.fileExists(configPath)) {
      return Global.showCloseAlert("File not found", "\(configPath) not found")
    }

    guard let daemonRemoteObject = self.daemonRemoteObject else {
      return Global.showCloseAlert("Error", "No Daemon service remote object running")
    }

   var daemonVersion: String?
   daemonRemoteObject.getVersion { version in daemonVersion = version }
    
   if (daemonVersion != VERSION) {
     return Global.showCloseAlert("Wrong Daemon version", "App \(VERSION) and Daemon \(daemonVersion ?? "(unknown)") Version Mismatch, Restart the Daemon")
   }

    guard let symbolsPath = Global.getResourceSymbolsPath() else { return }

    var daemonStartResult: Int?
    daemonRemoteObject.start(configPath: configPath, symbolsPath: symbolsPath, profileIdx: profileIdx ?? activeProfileIdx ?? 0) {
      result in daemonStartResult = result
    }

    if (daemonStartResult == 1) {
      Global.showCloseAlert("Enable Accesibility", "Enable Accesibility for this app")

      IOHIDRequestAccess(kIOHIDRequestTypePostEvent)
      return
    }

    if (daemonStartResult != 0) {
      return Global.showCloseAlert("Error", "Couldn't start Daemon process. Error \(daemonStartResult ?? -1)")
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

    startDaemon()
  }
  
  @objc func openConfigFolder() {
    guard let rootPath = Global.getRootPath() else { return }
    guard let resourcePath = Bundle.main.resourcePath else { return }
    
    do {
      guard let configPath = Global.getConfigPath() else { return }
      let configFileExists = Global.fileExists(configPath)
      
      if (!configFileExists) {
        try Global.copyFile(srcPath: "\(resourcePath)/config.json", to: configPath)
      }

    } catch let error {
      print("Error config file: ", error)
    }
    
    NSWorkspace.shared.selectFile(nil, inFileViewerRootedAtPath: rootPath)
  }

  @objc func openAboutWindow() {
    NSApp.activate(ignoringOtherApps: true)
    aboutViewController?.window?.orderFrontRegardless()
    aboutViewController?.window?.center()
    aboutViewController?.showWindow(aboutViewController?.window)
  }
  
  @objc func openLoggerWindow() {
    NSApp.activate(ignoringOtherApps: true)
    loggerViewController?.window?.orderFrontRegardless()
    loggerViewController?.window?.center()
    loggerViewController?.showWindow(loggerViewController?.window)
    
  }

  @objc func quit() {
    daemonRemoteObject?.kill()
    NSApplication.shared.terminate(self)
  }
}
