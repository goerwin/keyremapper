// All about XPC
// https://rderik.com/blog/xpc-services-on-macos-apps-using-swift/#the-idea-behind-xpc-and-its-uses

import Foundation

@objc class ServiceProviderXPC: NSObject, ServiceProviderXPCProtocol {
  func start(configPath: String, symbolsPath: String, withReply reply: @escaping (Int32) -> Void) {
    // TODO: if start returns 3, version mismatch

    guard let appBridge = Global.appBridge else {
      let appBridge = AppBridge()
      Global.appBridge = appBridge
//         TODO: Pass the appName in start
      return reply(appBridge.start(configPath, withSymbolsPath: symbolsPath))
    }

    appBridge.stop()
    return reply(appBridge.start(configPath, withSymbolsPath: symbolsPath))
  }


  func stop() {
    Global.appBridge?.stop()
  }
  
  func version(withReply reply: @escaping (String) -> Void) {
    reply(Global.VERSION)
  }
}
