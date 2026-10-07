import ServiceManagement
import SwiftUI

@main
struct KeyRemapperApp: App {
  @NSApplicationDelegateAdaptor(AppDelegate.self) fileprivate var appDelegate
  // It lives in the menu bar, its windows are opened by AppDelegate
  var body: some Scene {
    Settings { EmptyView() }
  }
}

private class AppDelegate: NSObject, NSApplicationDelegate {
  private let daemon = DaemonClient()
  private let config = ConfigStore()
  private lazy var statusBar = StatusBarController()
  private var isRunning = false
  // Only by the user. It can also stop running because of errors (eg. invalid config)
  private var isPaused = false
  private var activeProfileIdx = 0
  private var approvalTimer: Timer?

  private lazy var aboutWindowController: NSWindowController = {
    let window = NSWindow()
    window.styleMask = [.closable, .miniaturizable, .titled]
    window.title = "About \(Constants.BUNDLE_NAME)"
    window.contentView = NSHostingView(rootView: AboutView())
    return NSWindowController(window: window)
  }()

  private lazy var loggerWindowController: LoggerWindowController = {
    let window = NSWindow()
    window.styleMask = [.closable, .miniaturizable, .titled, .resizable]
    window.title = "Logger"
    window.contentView = NSHostingView(rootView: LoggerView(state: Global.state))
    let controller = LoggerWindowController(window: window)

    controller.onWindowDidBecomeMain = { [unowned self] in self.daemon.startLogging() }
    controller.onWindowWillClose = { [unowned self] in
      self.daemon.stopLogging()
      Global.state.resetLogStr()
    }

    window.delegate = controller
    return controller
  }()

  func applicationDidFinishLaunching(_ notification: Notification) {
    statusBar.onToggle = { [unowned self] in self.isRunning ? self.pause() : self.start() }
    statusBar.onSelectProfile = { [unowned self] idx in
      self.activeProfileIdx = idx
      self.start()
    }
    statusBar.onOpenConfigFolder = { [unowned self] in self.openConfigFolder() }
    statusBar.onOpenLogger = { [unowned self] in self.show(self.loggerWindowController) }
    statusBar.onToggleLaunchAtLogin = { [unowned self] in self.toggleLaunchAtLogin() }
    statusBar.onOpenAbout = { [unowned self] in self.show(self.aboutWindowController) }
    statusBar.onUninstall = { [unowned self] in self.uninstall() }
    statusBar.onQuit = { [unowned self] in self.quit() }

    daemon.onDisconnect = { [unowned self] in
      self.isRunning = false
      self.updateMenu()
    }
    daemon.onError = { [unowned self] err in
      self.isRunning = false
      self.updateMenu()
      Global.showCloseAlert("Error in Daemon", err)
    }

    // Saving the config reloads the active profile, also after errors so fixing them resumes it
    config.onChange = { [unowned self] in
      let isWaitingForApproval = self.approvalTimer?.isValid == true
      self.isPaused || isWaitingForApproval ? self.updateMenu() : self.start()
    }
    config.startWatching()

    activeProfileIdx = (try? config.load())?.activeProfileIdx ?? 0
    start()
  }

  private func start() {
    isPaused = false
    stop()

    switch daemon.register() {
    case .ready: break
    case .needsApproval: return waitForApproval()
    case .failed(let message): return Global.showCloseAlert("Error", message)
    }

    if !config.exists {
      return Global.showCloseAlert("File not found", "\(config.configPath) not found")
    }
    guard let symbols = Global.getResourceSymbols() else { return }

    let loadedConfig: ConfigStore.Config
    do {
      loadedConfig = try config.load()
    } catch {
      return Global.showCloseAlert("Invalid config", error.localizedDescription)
    }

    // The active profile could have been removed from the config. No profiles means the config
    // is invalid, the daemon reports it
    let profileCount = loadedConfig.profileNames.count
    if profileCount > 0 && activeProfileIdx >= profileCount { activeProfileIdx = 0 }

    do {
      try daemon.start(
        config: loadedConfig.json, symbols: symbols, profileIdx: activeProfileIdx)
      isRunning = true
      updateMenu()
    } catch DaemonClient.DaemonError.noAccessibility {
      Global.showCloseAlert("Enable Accessibility", "Enable Accessibility for this app")
      IOHIDRequestAccess(kIOHIDRequestTypePostEvent)
    } catch DaemonClient.DaemonError.reported {
    } catch {
      Global.showCloseAlert("Error", error.localizedDescription)
    }
  }

  private func pause() {
    isPaused = true
    stop()
  }

  private func stop() {
    if isRunning { daemon.stop() }
    isRunning = false
    updateMenu()
  }

  // The first time, the daemon has to be allowed in System Settings
  private func waitForApproval() {
    Global.showCloseAlert(
      "Allow \(Constants.BUNDLE_NAME)",
      "Turn on \(Constants.BUNDLE_NAME) in System Settings > General > Login Items & Extensions")
    SMAppService.openSystemSettingsLoginItems()

    approvalTimer?.invalidate()
    approvalTimer = Timer.scheduledTimer(withTimeInterval: 1, repeats: true) {
      [unowned self] timer in
      guard self.daemon.service.status == .enabled else { return }
      timer.invalidate()
      self.start()
    }
  }

  private func updateMenu() {
    statusBar.update(
      .init(
        isRunning: isRunning, profileNames: (try? config.load())?.profileNames ?? [],
        activeProfileIdx: activeProfileIdx,
        launchesAtLogin: SMAppService.mainApp.status == .enabled))
  }

  private func toggleLaunchAtLogin() {
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

    updateMenu()
  }

  // Leaves the app as if it was never opened (the config folder is kept)
  private func uninstall() {
    NSApp.activate(ignoringOtherApps: true)
    let alert = NSAlert()
    alert.messageText = "Uninstall \(Constants.BUNDLE_NAME)?"
    alert.informativeText =
      "This removes its background service, login item and permissions, and quits. Your config folder is kept. Then move the app to the Trash, or open it again to set it up from scratch."
    alert.addButton(withTitle: "Uninstall")
    alert.addButton(withTitle: "Cancel")
    if alert.runModal() != .alertFirstButtonReturn { return }

    pause()
    approvalTimer?.invalidate()

    do {
      try daemon.unregister()
      let loginItem = SMAppService.mainApp
      if loginItem.status != .notRegistered && loginItem.status != .notFound {
        try loginItem.unregister()
      }
    } catch {
      return Global.showCloseAlert("Error", "Couldn't uninstall: \(error.localizedDescription)")
    }

    for bundleId in [Bundle.main.bundleIdentifier ?? "", Constants.MACH_SERVICE_NAME] {
      Global.runProcess("/usr/bin/tccutil", args: ["reset", "All", bundleId])
    }

    NSApplication.shared.terminate(self)
  }

  private func openConfigFolder() {
    do {
      try config.createIfMissing()
    } catch {
      Global.showCloseAlert("Error", "Couldn't create the config: \(error.localizedDescription)")
    }

    NSWorkspace.shared.selectFile(nil, inFileViewerRootedAtPath: config.folderPath)
  }

  private func show(_ controller: NSWindowController) {
    NSApp.activate(ignoringOtherApps: true)
    controller.window?.orderFrontRegardless()
    controller.window?.center()
    controller.showWindow(controller.window)
  }

  // The daemon quits when the connection closes. Killing it before would make the connection,
  // still open, launch a new one that nobody stops
  private func quit() {
    NSApplication.shared.terminate(self)
  }
}
