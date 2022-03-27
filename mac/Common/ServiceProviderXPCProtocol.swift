import Foundation

@objc(ServiceProviderXPCProtocol) protocol ServiceProviderXPCProtocol {
  func start(configPath: String, symbolsPath: String, withReply reply: @escaping (Int32) -> Void)
  func stop()
  func kill()
  func uninstall()
  func getVersion(withReply reply: @escaping (String) -> Void)
}
