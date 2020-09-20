- you should only test key cases, not entire flows (like vim mode)
- add builds for both x86 and x64 archs
- move all files to src (no dirs allowed to avoid issues when importing header files)
- rethink alghoritm to be more general so it can be used in any OS
- provide a doublePress modifier (eg. for the capslock with double shift press)

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

