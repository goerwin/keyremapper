# Interception KeyRemapper
Powerful Key Remapper for windows using (interception)[https://github.com/oblitum/Interception] library underneath. This is even better than my previous one since this one works at the Driver level which is even lower level than the LowLevelKeyboardWindowHook, avoiding annoying issues presented by the latter.

## Requirements
-

## LWin + LAlt
- LWin + LAlt + H/J/K/L = LWin + arrowKeys

## Caps
- Caps + (Shift/S) + H/J/K/L = (Shift) + ArrowKeys
- Caps + LWin + (Shift/S) + H/J/K/L = (Shift) + Ctrl + ArrowKeys
- Caps + LAlt + H/J/K/L = ArrowKeys [NotIn-LAltAsLCtrl]
- Caps + LAlt + (Shift/S) + H/L = (Shift) + Home/End
- Caps + LAlt + (Shift/S) + K/J = (Shift) + Ctrl + Home/End
- Caps↕ = ESC [IfNeeded-LAltAsLCtrl]
- Caps + LWin + C = RightClick
- Caps + LAlt + V = LWin + V
- Caps + Key = _

## LCtrl
### General
- LCtrl + (Shift) + TAB = LCtrl +  (Shift) + TAB
- LCtrl + Shift = LAlt + Shift
- LCtrl + Key = LAlt + Key
- LCtrl = LAlt

## LWin
### General
- LWin + H = Alt + Left
- LWin + L = Alt + Right
- LWin + BACK = Ctrl + BACK
- LWin + (Shift/LAlt) + C = (Shift/Ctrl) + LeftClick
- LWin + D = LWin + D
- LWin + SPACE = LWin
- LWin + Key = _

## LAlt
### Google Chrome
- LAlt + RETURN = Alt + RETURN
- LAlt + H = Ctrl + Shift + TAB
- LAlt + L = Ctrl + TAB
- LAlt + ; = Ctrl + L
- LAlt + (Shift) + F{n} = (Shift) + F{n}
- LAlt↕↕ = AppCleanUp

### Slack
- LAlt + K = Ctrl + P (TODO:)

### General
- LAlt + ESC = ESC [NotIn-LAltAsLCtrl|Set-LAltAsLCtrl]
- LAlt + ESC = _
- LAlt + Q = DELETE [NotIn-LAltAsLCtrl]
- LAlt + Q = Alt + F4
- LAlt + TAB = Alt + TAB [Unset-LAltAsCtrl]
- LAlt + (Shift) + J/K = (Shift) + PageDown/PageUp
- LAlt + BACK = Shift + Home + BACK
- LAlt + SPACE = Ctrl + F12
- LAlt + Key = _ [NotIn-LAltAsLCtrl]
- LAlt + Shift = LCtrl + Shift
- LAlt + Key = LCtrl + Key
- LAlt = LCtrl

## Shift
- Shift↕↕ = Caps On
- Shift↕ = Caps Off
- Shift + Key = Shift + Key
- Shift = Shift

## Key
### Google Chrome
- F3 = Ctrl + Shift + TAB
- F4 = Ctrl + TAB
- F5 = Alt + M (Mute Tab)
- F6 = Alt + T (Twitch Theather Mode)
- Key = Key

### General
- F1 = BRIGHTNESSDOWN
- F2 = BRIGHTNESSUP
- F10 = MUTE
- F11 = VOLUMEDOWN
- F12 = VOLUMEUP

## Test Cases
https://www.key-shortcut.com/en/writing-systems/35-symbols/arrows/
- TODO:

## Issues
- alt + c (click) in mechanical keyboard keeps repeating itself.
  It was because rapid a fire setting that was set to increase key repeat speed. (FIXED)
- alt + tab, release tab, caps + h/l. Does not work properly (FIXED)
- simulated drag then R_option + tab to switch tabs. Doesnt work
- while in explorer, the lwin+lalt+c doesn't select multiple files/folders (FIXED)
- alt + tab to `Git Bash` + C triggers a ^C command instead of just typing C (FIXED)
- RAlt acting weird (having the value of LCtrl) (FIXED)
- Caps + LAlt + v + enter = copy something from clipboard not working propery (FIXED)
- drag a file with win + c and open alt + tab doesnt work
- LWin + D weird behavior when pressed twice (FIXED)
- ctrl + alt + supr, open the task manager, luego alt + tab no funciona
- get a text selected, press down capslock, see how the selection is lost (FIXED)
