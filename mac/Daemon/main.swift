import Foundation
import AppKit

func start() {
  let listener = NSXPCListener(machServiceName: MACH_SERVICE_NAME)
  let delegate = ServiceDelegateXPC()
  listener.delegate = delegate;
  listener.resume()

  // Listen for frontmost app changes
  NSWorkspace.shared.notificationCenter.addObserver(forName: NSWorkspace.didActivateApplicationNotification, object: nil, queue: nil) { _ in
    GlobalSwift.appBridge?.setAppName(GlobalSwift.getFrontmostAppName())
  }
}

start()
CFRunLoopRun()
