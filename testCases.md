# Test Cases

### Mouse
```
Win↓ C↓↑ Win↑ = MOUSELEFT↓↑
Win↓ Capslock↓ C↓↑ Capslock↑ Win↑ = MOUSERIGHT↓↑
Win↓ C↓↓↓↓↑ Win↑ = MOUSELEFT↓↑
Win↓ Capslock↓ C↓↓↓↓↑ Capslock↓ Win↑ = MOUSERIGHT↓↑
Win↓ Shift↓ C↓↑ Shift↑ Win↑ = Shift↓ MOUSELEFT↓↑ Shift↑
Win↓ Alt↓ C↓↑ Alt↑ Win↑ = Ctrl↓ MOUSELEFT↓↑ Ctrl↑
```

### Key
```
H↓↑ = H↓↑
H↓↓↓ = H↓↓↓
H↓↓↑ = H↓↓↑
H↓ J↓↑ H↑ = H↓ J↓↑ H↑
H↓ J↓ H↑ J↑ = H↓ J↓ H↑ J↑
H↓ J↓ K↓↑ J↑ H↑ = H↓ J↓ K↓↑ J↑ H↑
H↓ J↓ K↓↑ H↑ J↑ = H↓ J↓ K↓↑ H↑ J↑
F3↓↑ = Ctrl↓ Shift↓ Tab↕ Shift↑ Ctrl↑ // chrome.exe
F4↓↑ = Ctrl↓ Tab↕ Ctrl↑ // chrome.exe
F5↓↑ = Alt↓ M↕ Alt↑ // chrome.exe
F6↓↑ = Alt↓ T↕ Alt↑ // chrome.exe
C↓↑ = C↓↑
```

### Vim Mode
```
Capslock↓ H↓↑ Capslock↑ = LEFT↕
Capslock↓ L↓↑ Capslock↑ = RIGHT↕
Capslock↓ J↓↑ Capslock↑ = DOWN↕
Capslock↓ K↓↑ Capslock↑ = UP↕
Capslock↓ H↓↑↓↑ Capslock↑ = LEFT↕↕
Capslock↓ L↓↑↓↑ Capslock↑ = RIGHT↕↕
Capslock↓ J↓↑↓↑ Capslock↑ = DOWN↕↕
Capslock↓ K↓↑↓↑ Capslock↑ = UP↕↕
Capslock↓ H↓↑ Capslock↑ = LEFT↕ // AppSwitcher
Capslock↓ L↓↑ Capslock↑ = RIGHT↕ // AppSwitcher
Capslock↓ J↓↑ Capslock↑ = DOWN↕ // AppSwitcher
Capslock↓ K↓↑ Capslock↑ = UP↕ // AppSwitcher
Capslock↓ H↓↑↓↑ Capslock↑ = LEFT↕↕ // AppSwitcher
Capslock↓ L↓↑↓↑ Capslock↑ = RIGHT↕↕ // AppSwitcher
Capslock↓ J↓↑↓↑ Capslock↑ = DOWN↕↕ // AppSwitcher
Capslock↓ K↓↑↓↑ Capslock↑ = UP↕↕ // AppSwitcher
Capslock↓ S↓ H↓↑ S↑ Capslock↑ = Shift↓ LEFT↕ Shift↑
Capslock↓ Shift↓ H↓↑ Shift↑ Capslock↑ = Shift↓ LEFT↕ Shift↑
Capslock↓ S↓ L↓↑ S↑ Capslock↑ = Shift↓ RIGHT↕ Shift↑
Capslock↓ Shift↓ L↓↑ Shift↑ Capslock↑ = Shift↓ RIGHT↕ Shift↑
Capslock↓ S↓ J↓↑ S↑ Capslock↑ = Shift↓ DOWN↕ Shift↑
Capslock↓ Shift↓ J↓↑ Shift↑ Capslock↑ = Shift↓ DOWN↕ Shift↑
Capslock↓ S↓ K↓↑ S↑ Capslock↑ = Shift↓ UP↕ Shift↑
Capslock↓ Shift↓ K↓↑ Shift↑ Capslock↑ = Shift↓ UP↕ Shift↑
Capslock↓ S↓ H↓↑↓↑ S↑ Capslock↑ = Shift↓ LEFT↕↕ Shift↑
Capslock↓ Shift↓ H↓↑↓↑ Shift↑ Capslock↑ = Shift↓ LEFT↕↕ Shift↑
Capslock↓ S↓ L↓↑↓↑ S↑ Capslock↑ = Shift↓ RIGHT↕↕ Shift↑
Capslock↓ Shift↓ L↓↑↓↑ Shift↑ Capslock↑ = Shift↓ RIGHT↕↕ Shift↑
Capslock↓ S↓ J↓↑↓↑ S↑ Capslock↑ = Shift↓ DOWN↕↕ Shift↑
Capslock↓ Shift↓ J↓↑↓↑ Shift↑ Capslock↑ = Shift↓ DOWN↕↕ Shift↑
Capslock↓ S↓ K↓↑↓↑ S↑ Capslock↑ = Shift↓ UP↕↕ Shift↑
Capslock↓ Shift↓ K↓↑↓↑ Shift↑ Capslock↑ = Shift↓ UP↕↕ Shift↑
Capslock↓ Win↓ H↓↑ Win↑ Capslock↑ = Ctrl↓ LEFT↕ Ctrl↑
Capslock↓ Win↓ L↓↑ Win↑ Capslock↑ = Ctrl↓ RIGHT↕ Ctrl↑
Capslock↓ Win↓ J↓↑ Win↑ Capslock↑ = Ctrl↓ DOWN↕ Ctrl↑
Capslock↓ Win↓ K↓↑ Win↑ Capslock↑ = Ctrl↓ UP↕ Ctrl↑
Capslock↓ Win↓ H↓↑↓↑ Win↑ Capslock↑ = Ctrl↓ LEFT↕ Ctrl↑↓ LEFT↕ Ctrl↑
Capslock↓ Win↓ L↓↑↓↑ Win↑ Capslock↑ = Ctrl↓ RIGHT↕ Ctrl↑↓ RIGHT↕ Ctrl↑
Capslock↓ Win↓ J↓↑↓↑ Win↑ Capslock↑ = Ctrl↓ DOWN↕ Ctrl↑↓ DOWN↕ Ctrl↑
Capslock↓ Win↓ K↓↑↓↑ Win↑ Capslock↑ = Ctrl↓ UP↕ Ctrl↑↓ UP↕ Ctrl↑
Capslock↓ S↓ Win↓ H↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ LEFT↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ H↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ LEFT↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ L↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ RIGHT↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ L↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ RIGHT↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ J↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ DOWN↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ J↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ DOWN↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ K↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ UP↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ K↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ UP↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ H↓↑↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ LEFT↕ Ctrl↑↓ LEFT↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ H↓↑↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ LEFT↕ Ctrl↑↓ LEFT↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ L↓↑↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ RIGHT↕ Ctrl↑↓ RIGHT↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ L↓↑↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ RIGHT↕ Ctrl↑↓ RIGHT↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ J↓↑↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ DOWN↕ Ctrl↑↓ DOWN↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ J↓↑↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ DOWN↕ Ctrl↑↓ DOWN↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ K↓↑↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ UP↕ Ctrl↑↓ UP↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ K↓↑↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ UP↕ Ctrl↑↓ UP↕ Ctrl↑ Shift↑
Capslock↓ Alt↓ H↓↑ Alt↑ Capslock↑ = Ctrl↓↑ Home↕ Ctrl↓↑
Capslock↓ Alt↓ L↓↑ Alt↑ Capslock↑ = Ctrl↓↑ End↕ Ctrl↓↑
Capslock↓ Alt↓ H↓↑↓↑ Alt↑ Capslock↑ = Ctrl↓↑ Home↕ Ctrl↓↑ Home↕ Ctrl↓↑
Capslock↓ Alt↓ L↓↑↓↑ Alt↑ Capslock↑ = Ctrl↓↑ End↕ Ctrl↓↑ End↕ Ctrl↓↑
Capslock↓ S↓ Alt↓ H↓↑ Alt↑ S↑ Capslock↑ = Shift↓ Ctrl↓↑ Home↕ Ctrl↓↑ Shift↑
Capslock↓ Shift↓ Alt↓ H↓↑ Alt↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓↑ Home↕ Ctrl↓↑ Shift↑
Capslock↓ S↓ Alt↓ L↓↑ Alt↑ S↑ Capslock↑ = Shift↓ Ctrl↓↑ End↕ Ctrl↓↑ Shift↑
Capslock↓ Shift↓ Alt↓ L↓↑ Alt↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓↑ End↕ Ctrl↓↑ Shift↑
Capslock↓ S↓ Alt↓ H↓↑↓↑ Alt↑ S↑ Capslock↑ = Shift↓ Ctrl↓↑ Home↕ Ctrl↓↑ Home↕ Ctrl↓↑ Shift↑
Capslock↓ Shift↓ Alt↓ H↓↑↓↑ Alt↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓↑ Home↕ Ctrl↓↑ Home↕ Ctrl↓↑ Shift↑
Capslock↓ S↓ Alt↓ L↓↑↓↑ Alt↑ S↑ Capslock↑ = Shift↓ Ctrl↓↑ End↕ Ctrl↓↑ End↕ Ctrl↓↑ Shift↑
Capslock↓ Shift↓ Alt↓ L↓↑↓↑ Alt↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓↑ End↕ Ctrl↓↑ End↕ Ctrl↓↑ Shift↑
Capslock↓ Alt↓ J↓↑ Alt↑ Capslock↑ = Ctrl↓ End↕ Ctrl↑
Capslock↓ Alt↓ K↓↑ Alt↑ Capslock↑ = Ctrl↓ Home↕ Ctrl↑
Capslock↓ Alt↓ J↓↑↓↑ Alt↑ Capslock↑ = Ctrl↓ End↕↕ Ctrl↑
Capslock↓ Alt↓ K↓↑↓↑ Alt↑ Capslock↑ = Ctrl↓ Home↕↕ Ctrl↑
Capslock↓ S↓ Alt↓ J↓↑ Alt↑ S↑ Capslock↑ = Shift↓ Ctrl↓ End↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Alt↓ J↓↑ Alt↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ End↕ Ctrl↑ Shift↑
Capslock↓ S↓ Alt↓ K↓↑ Alt↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Home↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Alt↓ K↓↑ Alt↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Home↕ Ctrl↑ Shift↑
Capslock↓ S↓ Alt↓ J↓↑↓↑ Alt↑ S↑ Capslock↑ = Shift↓ Ctrl↓ End↕↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Alt↓ J↓↑↓↑ Alt↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ End↕↕ Ctrl↑ Shift↑
Capslock↓ S↓ Alt↓ K↓↑↓↑ Alt↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Home↕↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Alt↓ K↓↑↓↑ Alt↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Home↕↕ Ctrl↑ Shift↑
```

### LCtrl
```
Ctrl↓ Tab↓↑ Ctrl↑ = Alt↓↑ Ctrl↓ Tab↕ Ctrl↑
```

### LCtrl or LAlt
```
Ctrl↓ C↓↑ Ctrl↑ = Alt↓ C↓↑ Alt↑
Ctrl↓ C↓ = Alt↓ C↓
Alt↓ C↓↑ Alt↑ = Ctrl↓ C↓↑ Ctrl↑
Alt↓ C↓ = Ctrl↓ C↓
Ctrl↓ Shift↓↑ Ctrl↑ = Alt↓ Shift↓↑ Alt↑
Alt↓ Shift↓↑ Alt↑ = Ctrl↓ Shift↓↑ Ctrl↑
Ctrl↓↑ = Alt↓↑
Ctrl↓ = Alt↓
Alt↓↑ = Ctrl↓↑
Alt↓ = Ctrl↓
```

### LWin
```
Win↓ 1↓↑ Win↑ = Alt↓ 1↕ Alt↑ // SC2_x64.exe
Win↓ ⌫↓↑ Win↑ = Ctrl↓ W↕ Ctrl↑ // mintty.exe
Win↓ H↓↑ Win↑ = Alt↓ LEFT↕ Alt↑
Win↓ L↓↑ Win↑ = Alt↓ RIGHT↕ Alt↑
Win↓ ⌫↓↑ Win↑ = Ctrl↓ ⌫↕ Ctrl↑
Win↓ D↓↑ Win↑ = Win↓ D↕ Win↑
Win↓↑ = 
```

### LAlt
```
Alt↓ Esc↓↑ Alt↑ = Ctrl↓↑
Alt↓ Q↓↑ Alt↑ = Ctrl↓↑ Alt↓ F4↕ Alt↑ Ctrl↓↑
Alt↓ C↓ = Ctrl↓ C↓
Alt↓ C↓↑ Alt↑ = Ctrl↓ C↓↑ Ctrl↑
Alt↓ = Ctrl↓
Alt↓↑ = Ctrl↓↑
```

### Shift
```
Shift↓ C↓↑ Shift↑ = Shift↓ C↓↑ Shift↑
Shift↓↑ = Shift↓↑
```
