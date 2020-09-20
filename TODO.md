# TODO
- CLICKS
- handle remaps
- keybindings per app
- handle double press modifiers
- validate if keycode doesnt exist
- add builds for both x86 and x64 archs
- provide a doublePress modifier (eg. for the capslock with double shift press)

# DONE
- move all files to src (no dirs allowed to avoid issues when importing header files)
- you should only test key cases, not entire flows (like vim mode)
- rethink alghoritm to be more general so it can be used in any OS

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

