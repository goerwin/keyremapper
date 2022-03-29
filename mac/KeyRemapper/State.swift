import Foundation

class State: ObservableObject {
  @Published private(set) var loggerStr: String = ""
    
  func log(_ str: String) {
      loggerStr = "\(str)\(loggerStr)"
  }
  
  func resetLogStr() {
    loggerStr = ""
  }
}
