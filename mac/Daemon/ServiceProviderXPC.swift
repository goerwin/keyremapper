// All about XPC
// https://rderik.com/blog/xpc-services-on-macos-apps-using-swift/#the-idea-behind-xpc-and-its-uses

import Foundation

@objc class ServiceProviderXPC: NSObject, ServiceProviderXPCProtocol {
  func start(configPath: String, symbolsPath: String, profileIdx: Int, withReply reply: @escaping (Int) -> Void) {
    if Global.appBridge == nil {
      Global.appBridge = AppBridge()
    }

    Global.appBridge?.stop()
    let startResult = Global.appBridge?.start(configPath, withSymbolsPath: symbolsPath, withProfileIdx: Int32(profileIdx), withAppName: Global.getFrontmostAppName())

    return reply(Int(startResult ?? -1))
  }

  func stop() {
    Global.stop()
  }

  func kill() {
    Global.kill()
  }
  
  func uninstall() {
    Global.uninstall()
  }

  func getVersion(withReply reply: @escaping (String) -> Void) {
    reply(Global.VERSION)
  }
}
