import SwiftUI

class LoggerWindowController: NSWindowController, NSWindowDelegate {
  var onWindowDidBecomeMain: (() -> Void)?
  var onWindowWillClose: (() -> Void)?
  
  func windowDidBecomeMain(_ notification: Notification) {
    onWindowDidBecomeMain?()
  }
  
  func windowWillClose(_ notification: Notification) {
    onWindowWillClose?()
  }
}
