I was thinking about just adding a hook (https://gist.github.com/atr000/297924) to the cgevent keyboard events in order to see what flags, and in general, how the system sends the events.
This is to check for an issue when pressing (ctrl+down) which should show the exposé but I'm having too keep the down key a little longer to see the trigger.
- it also happens when pressing F11 to show desktop (kCGEventFlagsChanged???)
https://stackoverflow.com/questions/5840873/how-to-tell-the-difference-between-a-user-tapped-keyboard-event-and-a-generated


# Capslock handle
```
var ioConnect: io_connect_t = .init(0)
let ioService = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching(kIOHIDSystemClass))
IOServiceOpen(ioService, mach_task_self_, UInt32(kIOHIDParamConnectType), &ioConnect)

var modifierLockState = false
IOHIDGetModifierLockState(ioConnect, Int32(kIOHIDCapsLockState), &modifierLockState)

modifierLockState.toggle()
IOHIDSetModifierLockState(ioConnect, Int32(kIOHIDCapsLockState), modifierLockState)

IOServiceClose(ioConnect)
```
