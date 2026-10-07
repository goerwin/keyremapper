import Foundation

@objc(ServiceProviderXPCProtocol) protocol ServiceProviderXPCProtocol {
  // config and symbols are JSON, with the config imports already resolved
  func start(
    config: String, symbols: String, profileIdx: Int,
    withReply reply: @escaping (StartResult) -> Void)
  func stop()
  func kill()
  func startLogging()
  func stopLogging()
  func getVersion(withReply reply: @escaping (String) -> Void)
}

@objc(AppProviderXPCProtocol) protocol AppProviderXPCProtocol {
  func logKeyEvents(_ log: String)
  func notifyClientErrorInDaemon(_ err: String)
}
