# Notes

## TODO

- Mac
  - [ ] global shortcuts
  - [ ] Memory leaks
  - [x] prettify code
  - [x] handle daemon crash
  - [x] logging support
  - [x] update readme
  - [x] use only one file for configs
  - [x] development license via command line?
  - [x] easy way for versioning
  - [x] automate build
  - [x] run the hidiomanager as a daemon (root process), follow https://github.com/CharlesJS/CSAuthSample or https://github.com/erikberglund/SwiftPrivilegedHelper
  - [ ] bug keyboard can become unresponsive after going to sleep/long time inactivity
  - [x] versioning
  - [x] multiple configs
  - [x] press enter to disable/enable and see if the keyrepeat continues forever
  - [x] mouse callback events stop after you click the app's top menu bar
  - [x] if you use the gui version, if you type in the menu bar when the app is focused (eg. Help -> type something, it will not register the keys)
  - [x] Move files that are shared between OSs to a common place
  - [x] detect current application
  - [x] shift + capslock = not triggering shift + esc at first time/ test on a youtube video
  - [x] toggle caps
  - [x] multiple modes
  - [x] mouse clicks/mousedown/mouseup/drags
  - [x] multiple keyboards
  - [x] trigger media keys
    - brightnessDown/up, keyboardIlluminationDown/up, rewind/playPause/fastForward, mute, volumeDown/up
  - [x] exiting app from terminal (cmd+q) doesnt remove the process (it's expected to kill the keyRemapper process)
  - [x] simple GUI
  - [x] Fn key not working when app is disabled
  - [ ] shortcuts for mission control, launchpad?
    - maybe via shell command
    ```sh
      $ open "/System/Applications/Launchpad.app"
      $ open "/System/Applications/Mission Control.app"
    ```
  - [x] implement tests with time delays to test multiple key presses
  - [x] Move tests out of the Google test framework so we can test on Mac too

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

- Mac
  - Key Codes App is very useful to debug event keys and flags sent to the system
  - Input Monitoring check is required by IOHIDKit to globally listen for keyboard input across the system
  - when you send a CGEventCreateKeyboardEvent, the system will automatically ask for you permission for Accessibility(Allow the app to control your computer)
  - When developing in XCode
    - make sure the schemas (debug/release) have the "Debug Process" as "root" instead so that it can actually capture all keyboard input from anywhere
    - Under properties of the project, `Signing & Capabilities -> Signing Certificate`, make sure is set to "Development" to avoid ticking/unticking the "Accesibility" and "Input Monitoring" permission checkboxes
    - If running the executable from terminal, you must grant _Input Monitoring_ and _Accesibility_ permissions to the terminal that runs it (eg. iTerm or Terminal). Also you should run it with root permissions (eg. `$ sudo ~/Library/Developer/Xcode/DerivedData/KeyRemapperTerminal-xxx/Build/Products/Debug/KeyRemapperTerminal ; exit;`
  - You can execute command line commands via system. eg.
    - `system("say hello world");`
    - `system("osascript -e \"set volume 5\"");`
  - Debug message sent to deallocated instance errors (EXC_BAD_INSTRUCTION)
    - Edit Schema -> Diagnostics -> Check: Zombie objects, Guard Malloc, Malloc Stack Logging (All allocations and Free History)
    - reproduce the error then in the llbd console:
      - (lldb) command script import lldb.macosx.heap
      - (lldb) malloc_info --stack-history 0xAAAAAAAAA
  - For the Swift UI Version:
    - `com.apple.security.app-sandbox` should be false in .entitlements

## Snippets

```cpp
// Capture media key events in init main.mm
auto myEventTap = CGEventTapCreate(kCGHIDEventTap, kCGTailAppendEventTap, kCGEventTapOptionDefault,
  CGEventMaskBit(NX_SYSDEFINED),
  //    CGEventMaskBit(kCGEventKeyDown), // this traps expose and launchpad keys
  [](CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    Helpers::print("NOICE");
      return event;
    }, NULL);

if (!myEventTap) {
  std::cout << "Accesibility disabled for this app";
}

auto myRunLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, myEventTap, 0);

if (!myEventTap) {
  std::cout << "Couldn't create runLoopSource";
}
CFRunLoopAddSource(CFRunLoopGetMain(), myRunLoopSource, kCFRunLoopCommonModes);
```

```cpp
// Test memory leaks in init main.mm
 std::thread threadObj([]() {
   int i = 0;
   while (i < 500000) {
     toggleAppEnabled();
     std::this_thread::sleep_for(std::chrono::milliseconds(15));
     i++;
   }
 });
 threadObj.detach();
```

```sh
# read info.plist of a command line app
otool -X -s __TEXT __info_plist /path/to/executable | xxd -r

# read info.plist of an .app
cat /path/to/app/Contents/Info.plist

# push a new version (change the tag variable)
tag=v1.1.1 && git tag $tag && git push origin $tag
```
