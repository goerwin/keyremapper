# Notes
- Key Codes App very useful to debug event keys and flags sent to the system


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

# TODO:
- detect current application
- move option value keyPressDelay in keyDispatcher to main file
- toggle caps
- trigger media keys
- mouse clicks/drags/etc
- simple GUI
