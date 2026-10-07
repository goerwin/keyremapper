import Foundation

@objc class AppProviderXPC: NSObject, AppProviderXPCProtocol {
  private let onError: (String) -> Void

  init(onError: @escaping (String) -> Void) {
    self.onError = onError
  }

  func logKeyEvents(_ log: String) {
    DispatchQueue.main.async {
      Global.state.log("\n" + log)
    }
  }

  func notifyClientErrorInDaemon(_ err: String) {
    DispatchQueue.main.async { self.onError(err) }
  }
}
