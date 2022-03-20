import SwiftUI

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

    func applicationDidFinishLaunching(_ notification: Notification) {
      if let window = NSApplication.shared.windows.first { window.close() }
      createMenuBar()
      startBgProcess()
    }

  func startBgProcess() {
    DispatchQueue.global(qos: .background).async {
        self.background = Background()
      }
  }

  func createMenuBar() {
    statusBarMenu = NSMenu(title: "Cap Status Bar Menu")

      statusBarItem = NSStatusBar.system.statusItem(withLength: NSStatusItem.squareLength)
      statusBarItem?.button?.title = "🎹"

    statusBarItem?.menu = statusBarMenu

    statusBarMenu.addItem(
        withTitle: "Open Config Folder",
        action: #selector(AppDelegate.openConfigFolder),
        keyEquivalent: "")

    statusBarMenu.addItem(.separator())
    statusBarMenu.addItem(
        withTitle: "Quit",
        action: #selector(AppDelegate.quit),
        keyEquivalent: "")
  }

  @objc func openConfigFolder() {
    NSWorkspace.shared.selectFile(nil, inFileViewerRootedAtPath: "/Users/erwin.gaitan/keyRemapperMac")
  }

  @objc func quit() {
    NSApplication.shared.terminate(self)
  }

}
