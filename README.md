## Installation

- Windows
  - Install Interception driver following https://github.com/oblitum/Interception (it requires restart)
  - Download `win/output/KeyRemapper.zip` and there will be versions for x86 and x64 Windows.
  - Run the .exe with Administrator Rights (It is not necessary but to be able to run inside eg. Task Admin or some other privileged app, admin rights are needed)
  - Rules (remaps, keybindings, tests) can be found in `mode1.json`

## Build

- Windows
  - In Visual Studio 2019+, run a batch build and mark all projects, they all should REBUILD successfully
  - Navigate to project's root folder and run `$ bash win/release.sh` in WSL Ubuntu's terminal to generate the KeyRemapper.zip file with .exes for x86 and x64. (You need zip installed in terminal, also not sure if this works with Powershell)
  - File `win/output/KeyRemapper.zip` is generated
- Mac
  - Open project with Xcode
  - Product -> Archive -> Distribute Content -> Built Products

## Tests

- For Unix env run the tests in terminal
  ```
  $ g++ -o Tests/output -std=c++17 Tests/index.cpp && ./Tests/output
  ```
- For Windows, open `Developer PowerShell for VS 2022` or similar run
  ```
  $ cl .\Tests\index.cpp /std:c++17 /Fe"Tests/output.exe" /Fo"Tests/output.obj" | .\Tests\output.exe
  ```

### TODO

- Windows

  - [ ] Add versioning
  - [x] Tests via popup message
  - [x] UI to debug events
  - [x] multiple modes
  - [x] configs with json array/object replacement helpers
  - [x] alt tab enter issue
  - [x] add builds for both x86 and x64 archs
  - [x] enable/disable with double esc
  - [x] logo
  - [x] brightness
  - [x] double press modifiers (eg. for the capslock with double shift press)
  - [x] mouse clicks/mousedown/mouseup/drags
  - [x] keybindings per app
  - [x] fix issue with numpad and arrows (VERSION 1 is working properly!)
  - [x] remaps
  - [x] multiple keyboards
  - [x] you should only test key cases, not entire flows (like vim mode)
  - [x] rethink alghoritm to be more general so it can be used in any OS
  - [x] validate if keycode doesnt exist
  - [x] general Key (currentKey)
  - [ ] better error log for debug/release
  - [ ] keybindings for toggling app and changing modes should be global from the system and not the app
  - [x] afterKeyUp in keypresses

- Mac
  - [x] Move files that are shared between OSs to a common place
  - [x] detect current application
  - [x] shift + capslock = not triggering shift + esc at first time/ test on a youtube video
  - [x] toggle caps
  - [x] multiple modes
  - [x] mouse clicks/mousedown/mouseup/drags
  - [x] multiple keyboards
  - [x] trigger media keys
    - brightnessDown/up, keyboardIlluminationDown/up, rewind/playPause/fastForward, mute, volumeDown/up
  - [ ] bug keyboard can become unresponsive after going to sleep/long time inactivity
  - [x] exiting app from terminal (cmd+q) doesnt remove the process (it's expected to kill the keyRemapper process)
  - [ ] simple GUI
  - [ ] Fn key not working when app is disabled
  - [ ] Memory leaks
  - [ ] shortcuts for mission control, launchpad?
    - maybe via shell command
    ```sh
      $ open "/System/Applications/Launchpad.app"
      $ open "/System/Applications/Mission Control.app"
    ```
  - [x] implement tests with time delays to test multiple key presses
  - [x] Move tests out of the Google test framework so we can test on Mac too

### Text navigation/manipulation

Since I prefer how text manipulations work in Mac over windows, the following shows how it works on Mac so I can replicate it on Windows:

- <kbd>Alt + [Shift] + ←/→</kbd> → jump [select] to start/end of word
- <kbd>Cmd + [Shift] + ←/→</kbd> → jump [select] to start/end of line
- <kbd>Cmd + [Shift] + ↓/↑</kbd> → jump [select] to start/end of file
- <kbd>Alt + BackSpace</kbd> → Delete to start of word
- <kbd>Cmd + BackSpace</kbd> → delete to start of line

VIM mode should work like this for both win/mac (To match same layout keyboaord with Windows, replace Cmd/Alt with Ctrl/Win):

- <kbd>Caps + [F] + H/L/K/J</kbd> → move [select] Left/Right/Up/Down
- <kbd>Caps + Alt + [F] + H/L</kbd> → jump [select] to start/end of word
- <kbd>Caps + Cmd + [F] + H/L</kbd> → jump [select] to start/end of line
- <kbd>Caps + Cmd + [F] + K/J</kbd> → jump [select] to start/end of file

### Notes

- Windows

  - FakeShiftL is triggered when some special keys (left, right) are used with NumLock on. So I'm ignoring it.
  - Use Scan Codes to alter a specific hardware keyboard key. For example, the numeric keypad keys each have the same Scan Code, but different Virtual keys with NumLock either on or off (e.g. Numpad6 and NumpadRight). Using the Scan Code ignores the NumLock status. Use Scan Codes (if available) to alter that extra key on your keyboard. (While Scan Codes may be used to add missing keys, most likely the new Virtual Key will also be required.)
  - For unicode (accents, tildes, etc)
    - http://www.fileformat.info/tip/microsoft/enter_unicode.htm
  - Numlock
    - https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-keybd_event

- Mac
  - Key Codes App is very useful to debug event keys and flags sent to the system
  - Input Monitoring check is required by IOHIDKit to globally listen for keyboard input across the system
  - when you send a CGEventCreateKeyboardEvent, the system will automatically ask for you permission for Accessibility(Allow the app to control your computer)
  - When developing in XCode
    - make sure the schemas (debug/release) have the "Debug Process" as "root" instead so that it can actually capture all keyboard input from anywhere
    - Under properties of the project, `Signing & Capabilities -> Signing Certificate`, make sure is set to "Development" to avoid ticking/unticking the "Accesibility" and "Input Monitoring" permission checkboxes
    - If running the executable from terminal, you must grant _Input Monitoring_ and _Accesibility_ permissions to the terminal that runs it (eg. iTerm or Terminal). Also you should run it with root permissions (eg. `$ sudo /Users/erwin.gaitan/Library/Developer/Xcode/DerivedData/KeyRemapperTerminal-xxx/Build/Products/Debug/KeyRemapperTerminal ; exit;`
  - You can execute command line commands via system. eg.
    - `system("say hello world");`
    - `system("osascript -e \"set volume 5\"");`
  - Debug message sent to deallocated instance errors (EXC_BAD_INSTRUCTION)
    - Edit Schema -> Diagnostics -> Check: Zombie objects, Guard Malloc, Malloc Stack Logging (All allocations and Free History)
    - reproduce the error then in the llbd console:
      - (lldb) command script import lldb.macosx.heap
      - (lldb) malloc_info --stack-history 0xAAAAAAAAA

### Snippets Mac

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
CFRunLoopAddSource(CFRunLoopGetMain(), myRunLoopSource, kCFRunLoopDefaultMode);
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
