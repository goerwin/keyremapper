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
    // Without an action, so it's disabled
    menu.addItem(
      withTitle: "\(Constants.BUNDLE_NAME) \(Constants.VERSION)", action: nil, keyEquivalent: "")

    menu.addItem(.separator())
    if state.isRunning {
      addItem(to: menu, "Pause", "pause.circle", #selector(toggle))
    } else {
      addItem(to: menu, "Resume", "play.circle", #selector(toggle))
    }

    if !state.profileNames.isEmpty {
      menu.addItem(.separator())
      menu.addItem(.sectionHeader(title: "Profiles"))
      for (idx, name) in state.profileNames.enumerated() {
        let item = addItem(
          to: menu, name ?? "Profile \(idx + 1)", "keyboard", #selector(selectProfile),
          String(idx + 1))
        item.tag = idx
        item.state = idx == state.activeProfileIdx ? .on : .off
      }
    }

    menu.addItem(.separator())
    addItem(to: menu, "Open Config Folder", "folder", #selector(openConfigFolder))
    addItem(to: menu, "Logger", "doc.text.magnifyingglass", #selector(openLogger))
    addItem(to: menu, "Launch at Login", "power", #selector(toggleLaunchAtLogin)).state =
      state.launchesAtLogin ? .on : .off

    menu.addItem(.separator())
    addItem(to: menu, "About \(Constants.BUNDLE_NAME)", "info.circle", #selector(openAbout))
    addItem(to: menu, "Check for Updates…", "arrow.down.circle", #selector(checkForUpdates))

    menu.addItem(.separator())
    addItem(to: menu, "Uninstall \(Constants.BUNDLE_NAME)…", "trash", #selector(uninstall))
    addItem(to: menu, "Quit", "xmark.circle", #selector(quit), "q")

    statusItem.menu = menu
  }

  // symbol is the name of an SF Symbol
  @discardableResult
  private func addItem(
    to menu: NSMenu, _ title: String, _ symbol: String, _ action: Selector,
    _ keyEquivalent: String = ""
  ) -> NSMenuItem {
    let item = menu.addItem(withTitle: title, action: action, keyEquivalent: keyEquivalent)
    item.target = self
    item.image = NSImage(systemSymbolName: symbol, accessibilityDescription: nil)
    // macOS 27 hides menu item symbol images by default, so opt back in
    if #available(macOS 27.0, *) {
      item.preferredImageVisibility = .visible
    }
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
