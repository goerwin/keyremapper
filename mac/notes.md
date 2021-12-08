# Notes
- Key Codes App is very useful to debug event keys and flags sent to the system
- Input Monitoring check is required by IOHIDKit to globally listen for keyboard input across the system
- when you send a CGEventCreateKeyboardEvent, the system will automatically ask for you permission for Accessibility(Allow the app to control your computer)
- When developing in XCode
  - make sure the schemas (debug/release) have the "Debug Process" as "root" instead so that it can actually capture all keyboard input from anywhere
  - Under properties of the project, `Signing & Capabilities -> Signing Certificate`, make sure is set to "Development" to avoid ticking/unticking the "Accesibility" and "Input Monitoring" permission checkboxes
  - If running the executable from terminal, you must grant *Input Monitoring* and *Accesibility* permissions to the terminal that runs it (eg. iTerm or Terminal). Also you should run it with root permissions (eg. `/Users/erwin.gaitan/Library/Developer/Xcode/DerivedData/KeyRemapperTerminal-xxxx/Build/Products/Debug/KeyRemapperTerminal)`

# TODO:
- [ ] detect current application (probably need to work with Swift)
- [ ] simple GUI
- [ ] shift + capslock = not triggering shift + esc at first time/ test on a youtube video
- [ ] move option value keyPressDelay in keyDispatcher to main file
- [ ] bug keyboard can become unresponsive after going to sleep/long time inactivity?
- [ ] versioning
- [x] toggle caps
- [ ] trigger media keys
- [ ] mouse clicks/drags/etc
