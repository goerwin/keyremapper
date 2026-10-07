import Foundation

class State: ObservableObject {
  // Newest first. The oldest are dropped, the Logger only shows the first ones anyway
  static let MAX_LOG_LENGTH = 20_000

  @Published private(set) var loggerStr: String = ""

  func log(_ str: String) {
    loggerStr = String("\(str)\(loggerStr)".prefix(State.MAX_LOG_LENGTH))
  }

  func resetLogStr() {
    loggerStr = ""
  }
}
