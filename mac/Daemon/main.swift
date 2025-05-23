import AppKit
import Foundation

func start() {
  let listener = NSXPCListener(machServiceName: Constants.MACH_SERVICE_NAME)
  let delegate = ServiceDelegateXPC()
  listener.delegate = delegate
  listener.resume()
}

start()
CFRunLoopRun()
