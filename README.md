## Installation

- Mac
  - Download the latest version from `Releases`
  - On first time, it will ask you to install the Helper (Service that will listen to Keyboard input via IOKit)
  - Click the app icon in the menu bar and click `Open Config folder`. You'll find the config file `~/KeyRemapperMac/config.json`
  - Config usage in [Usage section](#user-content-usage)
  - Reselect the profile from the app menu to reload it
- Windows
  - Install Interception driver following https://github.com/oblitum/Interception (it requires restart)
  - Download the latest version from `Releases`
  - Copy the `keyRemapperWin` folder to your home directory (eg. `C:\Users\me`)
  - Run the .exe with Administrator Rights (It is not necessary but to be able to run inside eg. Task Admin or some other privileged app, admin rights are needed)
  - Config usage in [Usage section](#user-content-usage)

## Usage

Your initial `config.json` will be similar to the one below

```jsonc
{
  "profiles": [
    {
      "remaps": [],
      "keyPresses": [],
      "keybindings": []
    }
  ]
}
```

### Remaps

One to one remaps of keys, as soon a remap meets criteria, others are skipped

```jsonc
[
  // examples
  { "from": "A", "to": "B" },
  {
    "from": "C",
    "to": "D"

    // optionals:

    // "if"
  }
]
```

### KeyPresses

```jsonc
[
  {
    "key": "Shift",
    "ifPressedNTimes": 2,

    // "send": keyEvents
    // more examples of keyEvents down below
    "send": "Cmd:down C Cmd:up"

    // optionals:

    // "set"
    // "if"
    // "afterKeyUp"
  }
]
```

### Keybindings

Used for complex flows, once a keybinding meets criteria, others are skipped

```jsonc
[
  {
    /*
      Listen for any of these keys
      - you can find a list of these in the (win|mac symbols.json file
      - eg. listen for A or B
    */
    "keys": ["A", "B"],

    // Tuple, first item will fire on key down, second item on key up
    // - "send": [keyEvents, keyEvents]
    // Examples:

    // these are the same but :down/:up gives more granular control
    "send": ["C", null],
    "send": ["C:down C:up", null],

    "send": ["CmdL:down C CmdL:up", null],

    // currentKey is the key that activated this
    "send": ["currentKey:down", "currentKey:up"],

    "send": ["LeftClick", null],
    "send": ["LeftClick:down", "LeftClick:up"],

    // Special key Delay SK:Delay:{ms}
    "send": ["CmdL:down Tab SK:Delay:250 Tab CmdL:up", null]

    // optionals:

    // "if"
    // "set"
    // "setOnKeyUp"
    // "afterKeyUp"
  }
]
```

### Shared

`keyEvents` referenced in `send` properties can have keys found in `symbols.json` (see [reference](#user-content-reference)) and other reserved keys.

Common key properties used above

```jsonc
{
  // conditions that should be met
  "if": {
    "CmdL": true,
    "AltL": true,
    "MY_CUSTOM_VAR": false,

    // Reserved keys
    "isKeyDown": false, // useful to check if current key was just released
    "appName": "com.google.Chrome",
    "keyboard": "1234"
  },

  // fired on the next key event after this keyup
  // "afterKeyUp": keyEvents
  "afterKeyUp": "C:down C:up O O L",

  // Set custom variables
  // - don't use reserved keys
  // - set works on keydown, setOnKeyUp on keyup
  "set": { "MY_CUSTOM_VAR": true },
  "setOnKeyUp": { "MY_CUSTOM_VAR": false }
}
```

Also, sharing arrays and objects inside the `config.json` file can be achieved using the special helpers `%array`, `%dotdotdotArray`, `%object` and `%dotdotdotObject`. Check my personal config or the tests for this functionality (see [reference](#user-content-reference))

```jsonc
{
  "profiles": [
    {
      "name": "Profile 1",
      "remaps": "%array(_remaps.json)",
      "keyBindings": [
        "%dotdotdotArray(_vimMode.json)",
        "%object(_sharedKeybinding)",
        {
          "%dotdotdotObject": "(_sharedKeybinding2.json)"
        }
      ]
    },

    {
      "name": "Profile 2",
      "remaps": "%array(_remaps.json)",
      "keyBindings": [
        "%dotdotdotArray(_vimMode.json)",
        "%object(_sharedKeybinding)",
        {
          "%dotdotdotObject": "(_sharedKeybinding2.json)"
        }
      ]
    }
  ]
}
```

## Tests

- Mac
  ```sh
    $ g++ -o Tests/output -std=c++17 Tests/index.cpp && ./Tests/output
  ```
- Windows
  ```sh
    # open `Developer PowerShell for VS 2022` or similar
    $ cl .\Tests\index.cpp /std:c++17 /Fe"Tests/output.exe" /Fo"Tests/output.obj" | .\Tests\output.exe
  ```

## Build

- Mac
  - Create a `mac/.env` file using `mac/.env.example` and fill up the values
  - Open project `~mac/KeyRemapper.xcodeproj` with XCode
  - Sign the 2 targets for development (KeyRemapper, Daemon)
  - You should be able to run it on Xcode
- Windows
  - You should be able to run it in Visual Studio 2019+

## Deploy and Release

To trigger a release, you should push a tag. Eg:

```sh
$ TAG=v1.0.0 && git tag $TAG && git push origin $TAG
```

For completeness, to remove a tag:

```sh
$ TAG=v1.0.0 && git tag -d $TAG && git push origin --delete $TAG
```

## Reference

- Current keyboard and app names can be found using the `Logger`
- Mac's [symbols.json](mac/KeyRemapper/Resources/symbols.json)
- Windows' [symbols.json](win/src/files/symbols.json)
- My personal Mac's [config.json](https://github.com/goerwin/dotfiles/blob/master/src/keyRemapperMac/config.json)
- My personal Window's [config.json](https://github.com/goerwin/dotfiles/blob/master/src/keyRemapperWin/config.json)
- [Json Helpers test example](Tests/imports.json)

<details>
  <summary>Notes</summary>

### TODO

- Windows

  - [ ] Embed interception.dll in .exe
  - [ ] keybindings for toggling app and changing modes should be global from the system and not the app
  - [ ] memory leaks
  - [x] Add versioning
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
  - [x] better error log for debug/release
  - [x] afterKeyUp in keypresses

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

### Snippets

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

# path to msbuild (use a dot at begining to run it on powershell)
. "C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe" ...args
# to run it from bash, the path changes to
"/mnt/c/Program Files/Microsoft Visual Studio/2022/Community/Msbuild/Current/Bin/MSBuild.exe" ...args
```

### Troubleshooting

To run a clean build on XCode, you need to:
  - Kill the App
  - Kill the he deamon process (co.goerwin.KeyRemapperDaemon)
  - Go to: Product -> Clean Build Folder (Cmd + Shift + K)
  - Go to: Product -> Build (Cmd + Shift + B)

</details>
