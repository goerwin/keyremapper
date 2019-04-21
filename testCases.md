# Test Cases

### Mouse
```
Win↓ C↓↑ Win↑ = MOUSELEFT↓↑
Win↓ Caps↓ C↓↑ Caps↑ Win↑ = MOUSERIGHT↓↑
Win↓ C↓↓↓↓↑ Win↑ = MOUSELEFT↓↑
Win↓ Caps↓ C↓↓↓↓↑ Caps↓ Win↑ = MOUSERIGHT↓↑
Win↓ Shift↓ C↓↑ Shift↑ Win↑ = Shift↓ MOUSELEFT↓↑ Shift↑
Win↓ Alt↓ C↓↑ Alt↑ Win↑ = Ctrl↓ MOUSELEFT↓↑ Ctrl↑
Alt↓ Win↓ C↓↑ Alt↑ Win↑ = Ctrl↓ MOUSELEFT↓↑ Ctrl↑
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
F1↓↑ = BRIGHTNESSDOWN↕
F2↓↑ = BRIGHTNESSUP↕
F10↓↑ = MUTE↕
F11↓↑ = VOLUMEDOWN↕↕↕↕
F12↓↑ = VOLUMEUP↕↕↕↕
C↓↑ = C↓↑
```

### Vim Mode
```
Caps↓ H↓↑ Caps↑ = Left↕
Caps↓ L↓↑ Caps↑ = Right↕
Caps↓ J↓↑ Caps↑ = Down↕
Caps↓ K↓↑ Caps↑ = Up↕
Caps↓ H↓↑↓↑ Caps↑ = Left↕↕
Caps↓ L↓↑↓↑ Caps↑ = Right↕↕
Caps↓ J↓↑↓↑ Caps↑ = Down↕↕
Caps↓ K↓↑↓↑ Caps↑ = Up↕↕
Caps↓ H↓↑ Caps↑ = Left↕ // AppSwitcher
Caps↓ L↓↑ Caps↑ = Right↕ // AppSwitcher
Caps↓ J↓↑ Caps↑ = Down↕ // AppSwitcher
Caps↓ K↓↑ Caps↑ = Up↕ // AppSwitcher
Caps↓ H↓↑↓↑ Caps↑ = Left↕↕ // AppSwitcher
Caps↓ L↓↑↓↑ Caps↑ = Right↕↕ // AppSwitcher
Caps↓ J↓↑↓↑ Caps↑ = Down↕↕ // AppSwitcher
Caps↓ K↓↑↓↑ Caps↑ = Up↕↕ // AppSwitcher
Caps↓ S↓ H↓↑ S↑ Caps↑ = Shift↓ Left↕ Shift↑
Caps↓ Shift↓ H↓↑ Shift↑ Caps↑ = Shift↓ Left↕ Shift↑
Caps↓ S↓ L↓↑ S↑ Caps↑ = Shift↓ Right↕ Shift↑
Caps↓ Shift↓ L↓↑ Shift↑ Caps↑ = Shift↓ Right↕ Shift↑
Caps↓ S↓ J↓↑ S↑ Caps↑ = Shift↓ Down↕ Shift↑
Caps↓ Shift↓ J↓↑ Shift↑ Caps↑ = Shift↓ Down↕ Shift↑
Caps↓ S↓ K↓↑ S↑ Caps↑ = Shift↓ Up↕ Shift↑
Caps↓ Shift↓ K↓↑ Shift↑ Caps↑ = Shift↓ Up↕ Shift↑
Caps↓ S↓ H↓↑↓↑ S↑ Caps↑ = Shift↓ Left↕↕ Shift↑
Caps↓ Shift↓ H↓↑↓↑ Shift↑ Caps↑ = Shift↓ Left↕↕ Shift↑
Caps↓ S↓ L↓↑↓↑ S↑ Caps↑ = Shift↓ Right↕↕ Shift↑
Caps↓ Shift↓ L↓↑↓↑ Shift↑ Caps↑ = Shift↓ Right↕↕ Shift↑
Caps↓ S↓ J↓↑↓↑ S↑ Caps↑ = Shift↓ Down↕↕ Shift↑
Caps↓ Shift↓ J↓↑↓↑ Shift↑ Caps↑ = Shift↓ Down↕↕ Shift↑
Caps↓ S↓ K↓↑↓↑ S↑ Caps↑ = Shift↓ Up↕↕ Shift↑
Caps↓ Shift↓ K↓↑↓↑ Shift↑ Caps↑ = Shift↓ Up↕↕ Shift↑
Caps↓ Win↓ H↓↑ Win↑ Caps↑ = Ctrl↓ Left↕ Ctrl↑
Caps↓ Win↓ L↓↑ Win↑ Caps↑ = Ctrl↓ Right↕ Ctrl↑
Caps↓ Win↓ J↓↑ Win↑ Caps↑ = Ctrl↓ Down↕ Ctrl↑
Caps↓ Win↓ K↓↑ Win↑ Caps↑ = Ctrl↓ Up↕ Ctrl↑
Caps↓ Win↓ H↓↑↓↑ Win↑ Caps↑ = Ctrl↓ Left↕ Ctrl↑↓ Left↕ Ctrl↑
Caps↓ Win↓ L↓↑↓↑ Win↑ Caps↑ = Ctrl↓ Right↕ Ctrl↑↓ Right↕ Ctrl↑
Caps↓ Win↓ J↓↑↓↑ Win↑ Caps↑ = Ctrl↓ Down↕ Ctrl↑↓ Down↕ Ctrl↑
Caps↓ Win↓ K↓↑↓↑ Win↑ Caps↑ = Ctrl↓ Up↕ Ctrl↑↓ Up↕ Ctrl↑
Caps↓ S↓ Win↓ H↓↑ Win↑ S↑ Caps↑ = Shift↓ Ctrl↓ Left↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Win↓ H↓↑ Win↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Left↕ Ctrl↑ Shift↑
Caps↓ S↓ Win↓ L↓↑ Win↑ S↑ Caps↑ = Shift↓ Ctrl↓ Right↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Win↓ L↓↑ Win↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Right↕ Ctrl↑ Shift↑
Caps↓ S↓ Win↓ J↓↑ Win↑ S↑ Caps↑ = Shift↓ Ctrl↓ Down↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Win↓ J↓↑ Win↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Down↕ Ctrl↑ Shift↑
Caps↓ S↓ Win↓ K↓↑ Win↑ S↑ Caps↑ = Shift↓ Ctrl↓ Up↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Win↓ K↓↑ Win↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Up↕ Ctrl↑ Shift↑
Caps↓ S↓ Win↓ H↓↑↓↑ Win↑ S↑ Caps↑ = Shift↓ Ctrl↓ Left↕ Ctrl↑↓ Left↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Win↓ H↓↑↓↑ Win↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Left↕ Ctrl↑↓ Left↕ Ctrl↑ Shift↑
Caps↓ S↓ Win↓ L↓↑↓↑ Win↑ S↑ Caps↑ = Shift↓ Ctrl↓ Right↕ Ctrl↑↓ Right↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Win↓ L↓↑↓↑ Win↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Right↕ Ctrl↑↓ Right↕ Ctrl↑ Shift↑
Caps↓ S↓ Win↓ J↓↑↓↑ Win↑ S↑ Caps↑ = Shift↓ Ctrl↓ Down↕ Ctrl↑↓ Down↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Win↓ J↓↑↓↑ Win↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Down↕ Ctrl↑↓ Down↕ Ctrl↑ Shift↑
Caps↓ S↓ Win↓ K↓↑↓↑ Win↑ S↑ Caps↑ = Shift↓ Ctrl↓ Up↕ Ctrl↑↓ Up↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Win↓ K↓↑↓↑ Win↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Up↕ Ctrl↑↓ Up↕ Ctrl↑ Shift↑
Caps↓ Alt↓ H↓↑ Alt↑ Caps↑ = Ctrl↓↑ Home↕ Ctrl↓↑
Caps↓ Alt↓ L↓↑ Alt↑ Caps↑ = Ctrl↓↑ End↕ Ctrl↓↑
Caps↓ Alt↓ H↓↑↓↑ Alt↑ Caps↑ = Ctrl↓↑ Home↕ Ctrl↓↑ Home↕ Ctrl↓↑
Caps↓ Alt↓ L↓↑↓↑ Alt↑ Caps↑ = Ctrl↓↑ End↕ Ctrl↓↑ End↕ Ctrl↓↑
Caps↓ S↓ Alt↓ H↓↑ Alt↑ S↑ Caps↑ = Shift↓ Ctrl↓↑ Home↕ Ctrl↓↑ Shift↑
Caps↓ Shift↓ Alt↓ H↓↑ Alt↑ Shift↑ Caps↑ = Shift↓ Ctrl↓↑ Home↕ Ctrl↓↑ Shift↑
Caps↓ S↓ Alt↓ L↓↑ Alt↑ S↑ Caps↑ = Shift↓ Ctrl↓↑ End↕ Ctrl↓↑ Shift↑
Caps↓ Shift↓ Alt↓ L↓↑ Alt↑ Shift↑ Caps↑ = Shift↓ Ctrl↓↑ End↕ Ctrl↓↑ Shift↑
Caps↓ S↓ Alt↓ H↓↑↓↑ Alt↑ S↑ Caps↑ = Shift↓ Ctrl↓↑ Home↕ Ctrl↓↑ Home↕ Ctrl↓↑ Shift↑
Caps↓ Shift↓ Alt↓ H↓↑↓↑ Alt↑ Shift↑ Caps↑ = Shift↓ Ctrl↓↑ Home↕ Ctrl↓↑ Home↕ Ctrl↓↑ Shift↑
Caps↓ S↓ Alt↓ L↓↑↓↑ Alt↑ S↑ Caps↑ = Shift↓ Ctrl↓↑ End↕ Ctrl↓↑ End↕ Ctrl↓↑ Shift↑
Caps↓ Shift↓ Alt↓ L↓↑↓↑ Alt↑ Shift↑ Caps↑ = Shift↓ Ctrl↓↑ End↕ Ctrl↓↑ End↕ Ctrl↓↑ Shift↑
Caps↓ Alt↓ J↓↑ Alt↑ Caps↑ = Ctrl↓ End↕ Ctrl↑
Caps↓ Alt↓ K↓↑ Alt↑ Caps↑ = Ctrl↓ Home↕ Ctrl↑
Caps↓ Alt↓ J↓↑↓↑ Alt↑ Caps↑ = Ctrl↓ End↕↕ Ctrl↑
Caps↓ Alt↓ K↓↑↓↑ Alt↑ Caps↑ = Ctrl↓ Home↕↕ Ctrl↑
Caps↓ S↓ Alt↓ J↓↑ Alt↑ S↑ Caps↑ = Shift↓ Ctrl↓ End↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Alt↓ J↓↑ Alt↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ End↕ Ctrl↑ Shift↑
Caps↓ S↓ Alt↓ K↓↑ Alt↑ S↑ Caps↑ = Shift↓ Ctrl↓ Home↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Alt↓ K↓↑ Alt↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Home↕ Ctrl↑ Shift↑
Caps↓ S↓ Alt↓ J↓↑↓↑ Alt↑ S↑ Caps↑ = Shift↓ Ctrl↓ End↕↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Alt↓ J↓↑↓↑ Alt↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ End↕↕ Ctrl↑ Shift↑
Caps↓ S↓ Alt↓ K↓↑↓↑ Alt↑ S↑ Caps↑ = Shift↓ Ctrl↓ Home↕↕ Ctrl↑ Shift↑
Caps↓ Shift↓ Alt↓ K↓↑↓↑ Alt↑ Shift↑ Caps↑ = Shift↓ Ctrl↓ Home↕↕ Ctrl↑ Shift↑
```

### LAlt
```
Alt↓ `↓↑ Tab↓↑ Alt↑ = Ctrl↓↑ Alt↓ `↕ Alt↑ Ctrl↓↑ Alt↓ Tab↕ Alt↑
Alt↓ Tab↓↑ Q↓↑ Alt↑ = Ctrl↓↑ Alt↓ Tab↕ Supr↕ Alt↑
Alt↓ Tab↓↑ Esc↓↑ Alt↑ = Ctrl↓↑ Alt↓ Tab↕ Esc↕ Alt↑ Ctrl↓↑
Alt↓ Shift↓ Tab↓↑ Shift↑ Alt↑ = Ctrl↓ Shift↓ Ctrl↑ Alt↓ Tab↕ Shift↑ Alt↑
Alt↓ Esc↓↑ Alt↑ = Ctrl↓↑
Alt↓ Q↓↑ Alt↑ = Ctrl↓↑ Alt↓ F4↕ Alt↑ Ctrl↓↑
Alt↓ Back↓↑ Alt↑ = Ctrl↓↑ Shift↓ Home↕ Shift↑ Back↕ Ctrl↓↑
Alt↓ J↓↑ Alt↑ = Ctrl↓↑ Next↕ Ctrl↓↑
Alt↓ Tab↓↑ C↓↑ = Ctrl↓↑ Alt↓ Tab↕
Alt↓ Tab↓↑ = Ctrl↓↑ Alt↓ Tab↕
Alt↓ C↓ = Ctrl↓ C↓
Alt↓ C↓↑ Alt↑ = Ctrl↓ C↓↑ Ctrl↑
Alt↓ = Ctrl↓
Alt↓↑ = Ctrl↓↑
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
Win↓ Back↓↑ Win↑ = Ctrl↓ W↕ Ctrl↑ // mintty.exe
Win↓ H↓↑ Win↑ = Alt↓ Left↕ Alt↑
Win↓ L↓↑ Win↑ = Alt↓ Right↕ Alt↑
Win↓ Back↓↑ Win↑ = Ctrl↓ Back↕ Ctrl↑
Win↓ D↓↑ Win↑ = Win↓ D↕ Win↑
Win↓↑ = 
```

### Shift
```
Shift↓ C↓↑ Shift↑ = Shift↓ C↓↑ Shift↑
Shift↓↑ = Shift↓↑
```
