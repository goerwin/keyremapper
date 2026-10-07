import AppKit
import Foundation

@objc class GlobalSwift: NSObject {
  static let CHECK_CLIENT_INTERVAL = 1.0
  static let VERSION =
    Bundle.main.infoDictionary?["CFBundleShortVersionString"] as! String
  static let appBridge = AppBridge()

  static var connection: NSXPCConnection?

  @objc static func sendKeyEventLogsToClient(_ keyEventLogs: String) {
    guard
      let remoteObject = GlobalSwift.connection?.remoteObjectProxy()
        as? AppProviderXPCProtocol
    else { return }
    remoteObject.logKeyEvents(keyEventLogs)
  }

  @objc static func notifyError(_ err: String) {
    guard
      let remoteObject = GlobalSwift.connection?.remoteObjectProxy()
        as? AppProviderXPCProtocol
    else { return }
    remoteObject.notifyClientErrorInDaemon(err)
  }

  static func getFrontmostAppName() -> String {
    let frontmostApp = NSWorkspace.shared.frontmostApplication

    if let bundleId = frontmostApp?.bundleIdentifier {
      return bundleId
    }

    if let localizedName = frontmostApp?.localizedName {
      return localizedName
    }

    return "Unknown"
  }

  static func stop() {
    appBridge.stop()
  }

  static func kill() {
    stop()
    CFRunLoopStop(CFRunLoopGetMain())
  }
}
