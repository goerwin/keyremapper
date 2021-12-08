- Install Interception driver following https://github.com/oblitum/Interception (it requires restart)
- Download `KeyRemapper.zip` and there will be versions for x86 and x64 Windows.
- Run the .exe with Administrator Rights (It is not necessary but to be able to run inside eg. Task Admin or some other privileged app, admin rights are needed)
- Rules (remaps, keybindings, keyPresses, tests) can be found in `mode1.json`

## Build

- Windows
  - In Visual Studio 2019+, run a batch build and mark all (non-tests) projects, they all should build successfully
  - Run `$ bash release.sh` in terminal to generate the KeyRemapper.zip file with .exes for x86 and x64. (You need zip installed in terminal, also not sure if this works with Powershell, I use this from the WSL Ubuntu's terminal)
- Mac
  - Open Xcode project

## Tests

Run the tests in terminal
```
$ g++ -o Tests/output -std=c++17 Tests/index.cpp && ./Tests/output
```

### TODO

- [ ] Add versioning
- [ ] MacOS Version (Not viable as of now, use karabiner-elements instead)
- [ ] keybindings per keyboard
- [x] Tests via popup message
- [x] UI to debug events
- [x] multiple modes
- [x] multiple configs with json inheritance
- [x] alt tab enter issue
- [x] add builds for both x86 and x64 archs
- [x] enable/disable with double esc
- [x] logo
- [x] brightness
- [x] double press modifiers (eg. for the capslock with double shift press)
- [x] CLICKS
- [x] keybindings per app
- [x] fix issue with numpad and arrows (VERSION 1 is working properly!)
- [x] remaps
- [x] move all files to src (no dirs allowed to avoid issues when importing header files)
- [x] you should only test key cases, not entire flows (like vim mode)
- [x] rethink alghoritm to be more general so it can be used in any OS
- [x] validate if keycode doesnt exist
- [x] general Key (TOO COMPLEX TO IMPLEMENT)

### Text navigation/manipulation

Since I prefer how text manipulations work in Mac over windows, the following shows how it works on Mac so I can replicate it on Windows:

- <kbd>Cmd + ←/→</kbd> → jump to start/end of line
- <kbd>Cmd + Shift + ←/→</kbd> → select to start/end of line
- <kbd>Cmd + BackSpace</kbd> → delete to start of line
- <kbd>Alt + ←/→</kbd> → jump to beginning/end of word
- <kbd>Alt + Shift + ←/→</kbd> → select to beginning/end of word
- <kbd>Alt + BackSpace</kbd> → Delete to beginning of word

### NOTES

- FakeShiftL is triggered when some special keys (left, right) are used with NumLock on. So I'm ignoring it.
- Use Scan Codes to alter a specific hardware keyboard key. For example, the numeric keypad keys each have the same Scan Code, but different Virtual keys with NumLock either on or off (e.g. Numpad6 and NumpadRight). Using the Scan Code ignores the NumLock status. Use Scan Codes (if available) to alter that extra key on your keyboard. (While Scan Codes may be used to add missing keys, most likely the new Virtual Key will also be required.)
- For unicode (accents, tildes, etc)
  - http://www.fileformat.info/tip/microsoft/enter_unicode.htm
- Numlock
  - https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-keybd_event
