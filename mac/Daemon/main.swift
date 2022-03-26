import Foundation
import AppKit

var i = 0; // TODO: DELETE
func quitIfNoClientRunning() {

  i = i + 1;
  if (i > 1000) {
//    print("red light")
//    Global.appBridge!.stop()
//    exit(0);
  }
}

func start() {
  let listener = NSXPCListener(machServiceName: MACH_SERVICE_NAME)
  let delegate = ServiceDelegateXPC()
  listener.delegate = delegate;
  listener.resume()

  // Interval function to determine whether this process should be terminated
 Timer.scheduledTimer(withTimeInterval: Global.CHECK_CLIENT_INTERVAL, repeats: true) { _ in
   quitIfNoClientRunning();
 }

  // Listen for frontmost app changes
  NSWorkspace.shared.notificationCenter.addObserver(forName: NSWorkspace.didActivateApplicationNotification, object: nil, queue: nil) { _ in
    Global.setFrontmostAppNameToKeyRemapper()
  }
}

start()
CFRunLoopRun()
