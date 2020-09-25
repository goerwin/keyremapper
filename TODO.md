# TODO
- double press modifiers (eg. for the capslock with double shift press)
- use setOnDown, setOnUp, instead of set
- add builds for both x86 and x64 archs
- UI to debug events

# DONE
- CLICKS
- keybindings per app
- fix issue with numpad and arrows (VERSION 1 is working properly!)
- remaps
- move all files to src (no dirs allowed to avoid issues when importing header files)
- you should only test key cases, not entire flows (like vim mode)
- rethink alghoritm to be more general so it can be used in any OS
- validate if keycode doesnt exist

# NOTES
- FakeShiftL is triggered when some special keys (left, right) are used with NumLock on. So I'm ignoring it.
- Use Scan Codes to alter a specific hardware keyboard key. For example, the numeric keypad keys each have the same Scan Code, but different Virtual keys with NumLock either on or off (e.g. Numpad6 and NumpadRight). Using the Scan Code ignores the NumLock position. Use Scan Codes (if available) to alter that extra key on your keyboard. (While Scan Codes may be used to add missing keys, most likely the new Virtual Key will also be required.)
- For unicode (accents, tildes, etc)
  - http://www.fileformat.info/tip/microsoft/enter_unicode.htm
- Numlock
  - https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-keybd_event
