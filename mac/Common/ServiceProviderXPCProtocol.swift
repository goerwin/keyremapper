import Foundation

@objc(ServiceProviderXPCProtocol) protocol ServiceProviderXPCProtocol {
  func start(
    configPath: String, symbolsPath: String, profileIdx: Int,
    withReply reply: @escaping (Int) -> Void)
  func stop()
  func kill()
  func uninstall()
  func startLogging()
  func stopLogging()
  func getVersion(withReply reply: @escaping (String) -> Void)
}

@objc(AppProviderXPCProtocol) protocol AppProviderXPCProtocol {
  func logKeyEvents(_ log: String)
  func notifyClientErrorInDaemon(_ err: String)
}
