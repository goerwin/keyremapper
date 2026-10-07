import Foundation

@objc class GlobalSwift: NSObject {
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

  static func stop() {
    appBridge.stop()
  }

  static func kill() {
    stop()
    CFRunLoopStop(CFRunLoopGetMain())
  }
}
