import ServiceManagement
import SwiftUI
import UserNotifications

@main
struct KeyRemapperApp: App {
  @NSApplicationDelegateAdaptor(AppDelegate.self) fileprivate var appDelegate
  var body: some Scene {
    WindowGroup {}
  }
}

private class AppDelegate: NSObject, NSApplicationDelegate {
  var daemonStarted = false

  lazy var statusBarItem: NSStatusItem? = {
    NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)
  }()

  var activeProfileIdx: Int?
  lazy private var aboutViewController: NSWindowController? = {
    let view = AboutView()
    let styleMask: NSWindow.StyleMask = [.closable, .miniaturizable, .titled]
    let window = NSWindow()
    window.styleMask = styleMask
    window.title = "About \(Constants.BUNDLE_NAME)"
    window.contentView = NSHostingView(rootView: view)
    return NSWindowController(window: window)
  }()

  lazy var loggerViewController: LoggerWindowController? = {
    let view = LoggerView(state: Global.state)
    let window = NSWindow()
    let styleMask: NSWindow.StyleMask = [
      .closable, .miniaturizable, .titled, .resizable,
    ]
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
    if _daemonRemoteObject != nil {
      return _daemonRemoteObject
    }

    let connection = NSXPCConnection(
      machServiceName: Constants.MACH_SERVICE_NAME, options: [.privileged])

    connection.remoteObjectInterface = NSXPCInterface(
      with: ServiceProviderXPCProtocol.self)
    connection.resume()

    connection.interruptionHandler = {
      print("Connection with Daemon interrupted")
      self._daemonRemoteObject = nil
    }
    connection.invalidationHandler = {
      print("Connection with Daemon invalidated")
      self._daemonRemoteObject = nil
    }

    connection.exportedInterface = NSXPCInterface(
      with: AppProviderXPCProtocol.self)
    connection.exportedObject = AppProviderXPC()

    _daemonRemoteObject =
      connection.synchronousRemoteObjectProxyWithErrorHandler { error in
        print("Error:", error)
      } as? ServiceProviderXPCProtocol
    return _daemonRemoteObject
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
        content.title = title ?? Constants.BUNDLE_NAME
        content.body = description
        let request = UNNotificationRequest(
          identifier: UUID().uuidString, content: content, trigger: nil)

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
    // Grayed out whenever it isn't remapping (paused, missing permissions, errors)
    menuButton.appearsDisabled = !daemonStarted
    menuButton.frame = CGRect(
      x: 0.0, y: 3, width: menuButton.frame.width,
      height: menuButton.frame.height)

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
      withTitle: daemonStarted ? "Pause" : "Resume",
      action: #selector(AppDelegate.startOrStopDaemon),
      keyEquivalent: "")

    // add profiles to menu
    if let jsonConfig = Global.getJsonConfig() {
      let activeProfileIdx = activeProfileIdx ?? 0
      let profiles = jsonConfig["profiles"] as? [[String: AnyObject]] ?? []

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
      withTitle: "Launch at Login",
      action: #selector(AppDelegate.toggleLaunchAtLogin),
      keyEquivalent: ""
    ).state = SMAppService.mainApp.status == .enabled ? .on : .off
    statusBarItemMenu.addItem(
      withTitle: "About \(Constants.BUNDLE_NAME)",
      action: #selector(AppDelegate.openAboutWindow),
      keyEquivalent: "")
    statusBarItemMenu.addItem(
      withTitle: "Uninstall \(Constants.BUNDLE_NAME)…",
      action: #selector(AppDelegate.uninstall),
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

  let daemonService = SMAppService.daemon(
    plistName: "\(Constants.MACH_SERVICE_NAME).plist")
  var approvalTimer: Timer?

  // Returns whether the daemon can run. The first time it has to be allowed in System Settings
  func registerDaemon(profileIdx: Int?) -> Bool {
    if !Global.removeLegacyDaemon() {
      Global.showCloseAlert("Error", "Couldn't remove the previous Daemon")
      return false
    }

    if daemonService.status != .enabled && daemonService.status != .requiresApproval {
      do {
        try daemonService.register()
      } catch {
        // register also throws when the daemon is registered but still needs approval
        if daemonService.status != .requiresApproval {
          Global.showCloseAlert(
            "Error", "Couldn't register the Daemon: \(error.localizedDescription)")
          return false
        }
      }
    }

    if daemonService.status == .requiresApproval {
      Global.showCloseAlert(
        "Allow \(Constants.BUNDLE_NAME)",
        "Turn on \(Constants.BUNDLE_NAME) in System Settings > General > Login Items & Extensions"
      )
      SMAppService.openSystemSettingsLoginItems()

      approvalTimer?.invalidate()
      approvalTimer = Timer.scheduledTimer(withTimeInterval: 1, repeats: true) {
        [weak self] timer in
        guard let self, self.daemonService.status == .enabled else { return }
        timer.invalidate()
        self.startDaemon(profileIdx: profileIdx)
      }
      return false
    }

    return true
  }

  // After an app update, the daemon from the previous version can still be running.
  // Once it quits, launchd starts the new one on the next connection
  func restartDaemon() {
    daemonRemoteObject?.kill()

    for _ in 0..<20 {
      _daemonRemoteObject = nil
      if getDaemonVersion() == Constants.VERSION { return }
      Thread.sleep(forTimeInterval: 0.1)
    }
  }

  func startDaemon(profileIdx: Int? = nil) {
    if daemonStarted == true {
      stopDaemon()
    } else {
      reloadMenuBar()
    }

    if !registerDaemon(profileIdx: profileIdx) { return }

    guard let configPath = Global.getConfigPath() else {
      return Global.showCloseAlert("No config", "No config path constructed")
    }

    if !Global.fileExists(configPath) {
      return Global.showCloseAlert("File not found", "\(configPath) not found")
    }

    if getDaemonVersion() != Constants.VERSION { restartDaemon() }

    guard let daemonRemoteObject = self.daemonRemoteObject else {
      return Global.showCloseAlert(
        "Error", "No Daemon service remote object running")
    }

    let daemonVersion = getDaemonVersion()

    if daemonVersion != Constants.VERSION {
      return Global.showCloseAlert(
        "Wrong Daemon version",
        "App \(Constants.VERSION) and Daemon \(daemonVersion ?? "(unknown)") Version Mismatch, Restart the Daemon"
      )
    }

    guard let symbolsPath = Global.getResourceSymbolsPath() else { return }

    var daemonStartResult: Int?
    daemonRemoteObject.start(
      configPath: configPath, symbolsPath: symbolsPath,
      profileIdx: profileIdx ?? activeProfileIdx ?? 0
    ) {
      result in daemonStartResult = result
    }

    if daemonStartResult == 1 {
      Global.showCloseAlert(
        "Enable Accesibility", "Enable Accesibility for this app")

      IOHIDRequestAccess(kIOHIDRequestTypePostEvent)
      return
    }

    if daemonStartResult != 0 {
      return Global.showCloseAlert(
        "Error",
        "Couldn't start Daemon process. Error \(daemonStartResult ?? -1)")
    }

    self.daemonStarted = true
    self.reloadMenuBar()
  }

  func getDaemonVersion() -> String? {
    var version: String?
    daemonRemoteObject?.getVersion { version = $0 }
    return version
  }

  func stopDaemon() {
    if daemonStarted != true { return }
    daemonStarted = false
    daemonRemoteObject?.stop()
    reloadMenuBar()
  }

  @objc func toggleLaunchAtLogin() {
    do {
      if SMAppService.mainApp.status == .enabled {
        try SMAppService.mainApp.unregister()
      } else {
        try SMAppService.mainApp.register()
      }
    } catch {
      Global.showCloseAlert(
        "Error", "Couldn't change Launch at Login: \(error.localizedDescription)")
    }

    reloadMenuBar()
  }

  // Leaves the app as if it was never opened (the config folder is kept)
  @objc func uninstall() {
    NSApp.activate(ignoringOtherApps: true)
    let alert = NSAlert()
    alert.messageText = "Uninstall \(Constants.BUNDLE_NAME)?"
    alert.informativeText =
      "This removes its background service, login item and permissions, and quits. Your config folder is kept. Then move the app to the Trash, or open it again to set it up from scratch."
    alert.addButton(withTitle: "Uninstall")
    alert.addButton(withTitle: "Cancel")
    if alert.runModal() != .alertFirstButtonReturn { return }

    stopDaemon()
    approvalTimer?.invalidate()

    for service in [daemonService, SMAppService.mainApp]
    where service.status != .notRegistered && service.status != .notFound {
      do {
        try service.unregister()
      } catch {
        Global.showCloseAlert("Error", "Couldn't uninstall: \(error.localizedDescription)")
        return
      }
    }

    for bundleId in [Bundle.main.bundleIdentifier ?? "", Constants.MACH_SERVICE_NAME] {
      Global.runProcess("/usr/bin/tccutil", args: ["reset", "All", bundleId])
    }

    NSApplication.shared.terminate(self)
  }

  @objc func startOrStopDaemon() {
    if daemonStarted == true {
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

      if !configFileExists {
        try Global.copyFile(
          srcPath: "\(resourcePath)/config.json", to: configPath)
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
