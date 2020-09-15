- add builds for both x86 and x64 archs
- move all files to src (no dirs allowed to avoid issues when importing header files)
- rethink alghoritm to be more general so it can be used in any OS
- provide a doublePress modifier (eg. for the capslock with double shift press)
<!--
altdown capsdown altup capsup is leaving the ctrl click pressed

Key: string | string[] - matches currentKey, it can be an [] for ORs
Condition: { [key]: [value] } - multiple keys will represent ANDs
Fire: [string | null, string | null];


Rule {
  oneOfOrBoth [Key, Condition]
  (rules: Rule[] | fire: Fire)
} | { fire: Fire }

KeyBinding {
  modifier: 'Caps' | 'CapsAlt', 'CtrlShift', 'etc';
  ...Rule
}

// This is a keybinding only for modifiers, and you enter it when the "key":
// - "was just pressed/released, it is still pressed down
// Once a rule is met, the Keybinding will fire it, stop looking and subsequent keybindings will be skipped
// If no fire was returned, let the key go through
// "fire" is a tuple representing the keys to fire for when the key is pressed or released

KeyBinding[]
-->



g_isKeyDown => to reference if the non modifier key is down
g_isSKeyDown => for vimmode, also can be anothermodifierkey

g_isMouseClickDown
g_isCtrlAsAlt
g_isAltAsCtrl

g_isCapslockKeyDown = false;
g_isShiftKeyDown = false;
g_isCtrlKeyDown = false;
g_isWinKeyDown = false;
g_isAltKeyDown = false;
g_isEscKeyDown = false;

