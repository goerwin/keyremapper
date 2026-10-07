# Notes

## TODO

- [ ] global shortcuts
- [ ] Memory leaks
- [ ] bug keyboard can become unresponsive after going to sleep/long time inactivity
- [ ] shortcuts for mission control, launchpad?
  - maybe via shell command
  ```sh
    $ open "/System/Applications/Launchpad.app"
    $ open "/System/Applications/Mission Control.app"
  ```

## Text navigation/manipulation

How text manipulations work on Mac:

- <kbd>Alt + [Shift] + ←/→</kbd> → jump [select] to start/end of word
- <kbd>Cmd + [Shift] + ←/→</kbd> → jump [select] to start/end of line
- <kbd>Cmd + [Shift] + ↓/↑</kbd> → jump [select] to start/end of file
- <kbd>Alt + BackSpace</kbd> → Delete to start of word
- <kbd>Cmd + BackSpace</kbd> → delete to start of line

VIM mode:

- <kbd>Caps + [F] + H/L/K/J</kbd> → move [select] Left/Right/Up/Down
- <kbd>Caps + Alt + [F] + H/L</kbd> → jump [select] to start/end of word
- <kbd>Caps + Cmd + [F] + H/L</kbd> → jump [select] to start/end of line
- <kbd>Caps + Cmd + [F] + K/J</kbd> → jump [select] to start/end of file

## Notes

- Key Codes App is very useful to debug event keys and flags sent to the system
- Input Monitoring check is required by IOHIDKit to globally listen for keyboard input across the system
- when you send a CGEventCreateKeyboardEvent, the system will automatically ask for you permission for Accessibility(Allow the app to control your computer)
- Under properties of the project, `Signing & Capabilities -> Signing Certificate`, make sure is set to "Development" to avoid ticking/unticking the "Accesibility" and "Input Monitoring" permission checkboxes
- `com.apple.security.app-sandbox` should be false in .entitlements
- Debug message sent to deallocated instance errors (EXC_BAD_INSTRUCTION)
  - Edit Schema -> Diagnostics -> Check: Zombie objects, Guard Malloc, Malloc Stack Logging (All allocations and Free History)
  - reproduce the error then in the llbd console:
    - (lldb) command script import lldb.macosx.heap
    - (lldb) malloc_info --stack-history 0xAAAAAAAAA

```sh
# read info.plist of a command line app
otool -X -s __TEXT __info_plist /path/to/executable | xxd -r

# read info.plist of an .app
cat /path/to/app/Contents/Info.plist
```
