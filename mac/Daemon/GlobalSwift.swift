import AppKit
import Foundation

@objc class GlobalSwift: NSObject {
  static let CHECK_CLIENT_INTERVAL = 1.0
  static let VERSION =
    Bundle.main.infoDictionary?["CFBundleShortVersionString"] as! String
  static var appBridge: AppBridge?

  static func runProcess(_ path: String, args: [String]?) {
    let process = Process()
    process.launchPath = path
    process.arguments = args
    process.launch()
  }

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
    appBridge?.stop()
  }

  static func kill() {
    stop()
    runProcess(
      "/usr/bin/sudo", args: ["/bin/launchctl", "remove", MACH_SERVICE_NAME])
    CFRunLoopStop(CFRunLoopGetMain())
  }

  static func uninstall() {
    runProcess(
      "/usr/bin/sudo",
      args: [
        "/bin/rm", "-rf", "/Library/LaunchDaemons/\(MACH_SERVICE_NAME).plist",
      ])
    runProcess(
      "/usr/bin/sudo",
      args: [
        "/bin/rm", "-rf", "/Library/PrivilegedHelperTools/\(MACH_SERVICE_NAME)",
      ])
    runProcess(
      "/usr/bin/sudo", args: ["/bin/launchctl", "remove", MACH_SERVICE_NAME])
    kill()
  }
}
