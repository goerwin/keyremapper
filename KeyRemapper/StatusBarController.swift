import AppKit

// The menu bar icon and its menu. It only renders the state it's given, actions go to the callbacks
final class StatusBarController: NSObject {
  struct State {
    var isRunning = false
    var profileNames: [String?] = []
    var activeProfileIdx = 0
    var launchesAtLogin = false
  }

  var onToggle: () -> Void = {}
  var onSelectProfile: (Int) -> Void = { _ in }
  var onOpenConfigFolder: () -> Void = {}
  var onOpenLogger: () -> Void = {}
  var onToggleLaunchAtLogin: () -> Void = {}
  var onOpenAbout: () -> Void = {}
  var onCheckForUpdates: () -> Void = {}
  var onUninstall: () -> Void = {}
  var onQuit: () -> Void = {}

  private let statusItem = NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)

  func update(_ state: State) {
    guard let button = statusItem.button else { return }
    guard let iconImage = NSImage(named: "MenuBarIcon") else { return }

    button.imagePosition = .imageLeft
    button.image = Global.resizeImage(image: iconImage, w: 16, h: 16)
    button.image?.isTemplate = true
    // Grayed out whenever it isn't remapping (paused, missing permissions, errors)
    button.appearsDisabled = !state.isRunning
    button.frame = CGRect(x: 0.0, y: 3, width: button.frame.width, height: button.frame.height)

    let activeName = state.profileNames.indices.contains(state.activeProfileIdx)
      ? state.profileNames[state.activeProfileIdx] : nil
    button.title = activeName.map { " \($0)" } ?? ""

    let menu = NSMenu(title: "Status Bar Item Menu")
    addItem(to: menu, state.isRunning ? "Pause" : "Resume", #selector(toggle))

    menu.addItem(.separator())
    for (idx, name) in state.profileNames.enumerated() {
      let itemName = name ?? "Profile \(idx + 1)"
      let item = addItem(
        to: menu, idx == state.activeProfileIdx ? "✔  \(itemName)" : itemName,
        #selector(selectProfile), String(idx + 1))
      item.tag = idx
    }

    menu.addItem(.separator())
    addItem(to: menu, "Open Config Folder", #selector(openConfigFolder))
    addItem(to: menu, "Logger", #selector(openLogger))

    menu.addItem(.separator())
    addItem(to: menu, "Launch at Login", #selector(toggleLaunchAtLogin)).state =
      state.launchesAtLogin ? .on : .off

    menu.addItem(.separator())
    addItem(to: menu, "About \(Constants.BUNDLE_NAME)", #selector(openAbout))
    addItem(to: menu, "Check for Updates…", #selector(checkForUpdates))

    menu.addItem(.separator())
    addItem(to: menu, "Uninstall \(Constants.BUNDLE_NAME)…", #selector(uninstall))
    addItem(to: menu, "Quit", #selector(quit), "q")

    statusItem.menu = menu
  }

  @discardableResult
  private func addItem(
    to menu: NSMenu, _ title: String, _ action: Selector, _ keyEquivalent: String = ""
  ) -> NSMenuItem {
    let item = menu.addItem(withTitle: title, action: action, keyEquivalent: keyEquivalent)
    item.target = self
    return item
  }

  @objc private func toggle() { onToggle() }
  @objc private func selectProfile(_ sender: NSMenuItem) { onSelectProfile(sender.tag) }
  @objc private func openConfigFolder() { onOpenConfigFolder() }
  @objc private func openLogger() { onOpenLogger() }
  @objc private func toggleLaunchAtLogin() { onToggleLaunchAtLogin() }
  @objc private func openAbout() { onOpenAbout() }
  @objc private func checkForUpdates() { onCheckForUpdates() }
  @objc private func uninstall() { onUninstall() }
  @objc private func quit() { onQuit() }
}
