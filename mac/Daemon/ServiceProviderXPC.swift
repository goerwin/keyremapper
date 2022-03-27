// All about XPC
// https://rderik.com/blog/xpc-services-on-macos-apps-using-swift/#the-idea-behind-xpc-and-its-uses

import Foundation

@objc class ServiceProviderXPC: NSObject, ServiceProviderXPCProtocol {
  func start(configPath: String, symbolsPath: String, withReply reply: @escaping (Int32) -> Void) {
    guard let appBridge = Global.appBridge else {
      let appBridge = AppBridge()
      Global.appBridge = appBridge
      return reply(appBridge.start(configPath, withSymbolsPath: symbolsPath, withAppName: Global.getFrontmostAppName()))
    }

    appBridge.stop()
    return reply(appBridge.start(configPath, withSymbolsPath: symbolsPath, withAppName: Global.getFrontmostAppName()))
  }


  func stop() {
    Global.appBridge?.stop()
  }

  func destroy() {
    stop()
    exit(0)
  }
  
  func uninstall() {
    Global.uninstall()
  }

  func getVersion(withReply reply: @escaping (String) -> Void) {
    reply(Global.VERSION)
  }
}
