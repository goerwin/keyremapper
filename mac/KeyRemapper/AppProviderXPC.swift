import Foundation

@objc class AppProviderXPC: NSObject, AppProviderXPCProtocol {
  func logKeyEvents(_ log: String) {
    DispatchQueue.main.async {
      Global.state.log("\n" + log)
    }
  }

  func notifyClientErrorInDaemon(_ err: String) {
    DispatchQueue.main.async {
      Global.showCloseAlert("Error in Daemon", err)
    }
  }
}
