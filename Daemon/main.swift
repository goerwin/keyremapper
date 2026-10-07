import Foundation

// Globals, so they live as long as the daemon. The listener doesn't retain its delegate
let delegate = ServiceDelegateXPC()
let listener = NSXPCListener(machServiceName: Constants.MACH_SERVICE_NAME)
listener.delegate = delegate
listener.resume()

CFRunLoopRun()
