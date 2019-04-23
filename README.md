# Interception KeyRemapper
Powerful Key Remapper for windows using [Interception](https://github.com/oblitum/Interception) library underneath. This is even better than my previous one since this one works at the Driver level which is even lower level than the LowLevelKeyboardWindowHook, avoiding annoying issues presented by the latter.

## Requirements
- For this snapping windows function you have to disable the option "When I Snap a window, show what I can snap next to it", under "Multitasking" settings in control panel
- Remove ctrl + shift shortcut to change keyboard layouts: Settings -> Language -> Spelling, typing & keyboard settings -> Advanced keyboard settings -> Language bar options -> Advanced Key Settings -> Action: Between Input Languages -> Change Key Secuence -> Not Asigned

## Notes
- [coreHotKeys](./InterceptionKeyRemapper/coreHotKeys.md)
- [customHotKeys](./InterceptionKeyRemapper/customHotKeys.md)
- [testCases](./KeyEventsTest/testCases.md)
- https://www.key-shortcut.com/en/writing-systems/35-symbols/arrows/

## Issues
- alt + c (click) in mechanical keyboard keeps repeating itself. (FIXED)
  It was because rapid a fire setting that was set to increase key repeat speed. (FIXED)
- alt + tab, release tab, caps + h/l. Does not work properly (FIXED)
- while in explorer, the lwin+lalt+c doesn't select multiple files/folders (FIXED)
- alt + tab to `Git Bash` + C triggers a ^C command instead of just typing C (FIXED)
- RAlt acting weird (having the value of LCtrl) (FIXED)
- Caps + LAlt + v + enter = copy something from clipboard not working propery (FIXED)
- LWin + D weird behavior when pressed twice (FIXED)
- get a text selected, press down capslock, see how the selection is lost (FIXED)
- how to handle multiple single key pressed? If you try to play games that rely on
  keyup events, some of then may not never be released. Check https://www.retrogames.cz/play_910-SNES.php?language=EN for example (FIXED)
- lwin + c click does not work in Task Manager (You have to run the program with admin rights) (FIXED)
- drag a file with win + c and open alt + tab doesnt work
- lalt + capslock + s triggers switch of input language (FIXED)
- caps + (shift) + tab to ctrl + (shift) + tab (FIXED)
- in chrome, lalt↓ + `↕ + L↕ + tab↕ + lalt(up)  (FIXED)
- lalt + capslock + v not working (FIXED)
- lalt ` not working properly when more than 2 windows (NOT RELATED THOUGH)