// All about XPC
// https://rderik.com/blog/xpc-services-on-macos-apps-using-swift/#the-idea-behind-xpc-and-its-uses

import Foundation

// XPC calls arrive on a background queue, but the runtime (keyboard and mouse callbacks)
// lives on the main run loop, so every call that touches it runs on main
@objc class ServiceProviderXPC: NSObject, ServiceProviderXPCProtocol {
  func start(
    config: String, symbols: String, profileIdx: Int,
    withReply reply: @escaping (StartResult) -> Void
  ) {
    let startResult = DispatchQueue.main.sync {
      GlobalSwift.appBridge.start(
        config, withSymbols: symbols,
        withProfileIdx: Int32(profileIdx),
        withAppName: GlobalSwift.getFrontmostAppName())
    }

    return reply(startResult)
  }

  func stop() {
    DispatchQueue.main.sync { GlobalSwift.stop() }
  }

  func kill() {
    DispatchQueue.main.async { GlobalSwift.kill() }
  }

  func startLogging() {
    DispatchQueue.main.sync { GlobalSwift.appBridge.startLogging() }
  }

  func stopLogging() {
    DispatchQueue.main.sync { GlobalSwift.appBridge.stopLogging() }
  }

  func getVersion(withReply reply: @escaping (String) -> Void) {
    reply(GlobalSwift.VERSION)
  }
}
