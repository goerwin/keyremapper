import Foundation
import AppKit

func setFrontmostAppNameToKeyRemapper() {
  let frontmostApp = NSWorkspace.shared.frontmostApplication
  let bundleId = frontmostApp?.bundleIdentifier
  let localizedName = frontmostApp?.localizedName

  Global.appBridge?.setAppName((bundleId != nil) ? bundleId : (localizedName != nil) ? localizedName : "Unknown")
}

func handleAppChange(notification: Notification) {
  setFrontmostAppNameToKeyRemapper()
}

var i = 0; // TODO: DELETE
func quitIfNoClientRunning() {
  print("noice")
  i = i + 1;
  if (i > 5) {
    exit(0);
  }
}

let listener = NSXPCListener(machServiceName: MACH_SERVICE_NAME)
let delegate = ServiceDelegateXPC()
listener.delegate = delegate;
listener.resume()

func start() {
//  listener = NSXPCListener(machServiceName: MACH_SERVICE_NAME)
//  delegate = ServiceDelegateXPC()
  

  // Interval function to determine whether this process should be terminated
  Timer.scheduledTimer(withTimeInterval: 5, repeats: true) { _ in
    quitIfNoClientRunning();
  }
  
  // Listen for frontmost app changes
  NSWorkspace.shared.notificationCenter.addObserver(forName: NSWorkspace.didActivateApplicationNotification, object: nil, queue: nil, using: handleAppChange)
}

start()
CFRunLoopRun()
