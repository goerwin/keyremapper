// All about XPC
// https://rderik.com/blog/xpc-services-on-macos-apps-using-swift/#the-idea-behind-xpc-and-its-uses

import Foundation

@objc class ServiceProviderXPC: NSObject, ServiceProviderXPCProtocol {
  func start(
    configPath: String, symbolsPath: String, profileIdx: Int,
    withReply reply: @escaping (Int) -> Void
  ) {
    if GlobalSwift.appBridge == nil {
      GlobalSwift.appBridge = AppBridge()
    }

    GlobalSwift.appBridge?.stop()
    let startResult = GlobalSwift.appBridge?.start(
      configPath, withSymbolsPath: symbolsPath,
      withProfileIdx: Int32(profileIdx),
      withAppName: GlobalSwift.getFrontmostAppName())

    return reply(Int(startResult ?? -1))
  }

  func stop() {
    GlobalSwift.stop()
  }

  func kill() {
    GlobalSwift.kill()
  }

  func uninstall() {
    GlobalSwift.uninstall()
  }

  func startLogging() {
    GlobalSwift.appBridge?.startLogging()
  }

  func stopLogging() {
    GlobalSwift.appBridge?.stopLogging()
  }
  
  func setAppName(_ appName: String) {
    GlobalSwift.appBridge?.setAppName(appName)
  }

  func getVersion(withReply reply: @escaping (String) -> Void) {
    reply(GlobalSwift.VERSION)
  }
}
