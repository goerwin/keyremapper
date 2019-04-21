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
F1↓↑ = BRIGHTNESSDOWN↕
F2↓↑ = BRIGHTNESSUP↕
F10↓↑ = MUTE↕
F11↓↑ = VOLUMEDOWN↕↕↕↕
F12↓↑ = VOLUMEUP↕↕↕↕
C↓↑ = C↓↑
```

### Vim Mode
```
Capslock↓ H↓↑ Capslock↑ = Left↕
Capslock↓ L↓↑ Capslock↑ = Right↕
Capslock↓ J↓↑ Capslock↑ = Down↕
Capslock↓ K↓↑ Capslock↑ = Up↕
Capslock↓ H↓↑↓↑ Capslock↑ = Left↕↕
Capslock↓ L↓↑↓↑ Capslock↑ = Right↕↕
Capslock↓ J↓↑↓↑ Capslock↑ = Down↕↕
Capslock↓ K↓↑↓↑ Capslock↑ = Up↕↕
Capslock↓ H↓↑ Capslock↑ = Left↕ // AppSwitcher
Capslock↓ L↓↑ Capslock↑ = Right↕ // AppSwitcher
Capslock↓ J↓↑ Capslock↑ = Down↕ // AppSwitcher
Capslock↓ K↓↑ Capslock↑ = Up↕ // AppSwitcher
Capslock↓ H↓↑↓↑ Capslock↑ = Left↕↕ // AppSwitcher
Capslock↓ L↓↑↓↑ Capslock↑ = Right↕↕ // AppSwitcher
Capslock↓ J↓↑↓↑ Capslock↑ = Down↕↕ // AppSwitcher
Capslock↓ K↓↑↓↑ Capslock↑ = Up↕↕ // AppSwitcher
Capslock↓ S↓ H↓↑ S↑ Capslock↑ = Shift↓ Left↕ Shift↑
Capslock↓ Shift↓ H↓↑ Shift↑ Capslock↑ = Shift↓ Left↕ Shift↑
Capslock↓ S↓ L↓↑ S↑ Capslock↑ = Shift↓ Right↕ Shift↑
Capslock↓ Shift↓ L↓↑ Shift↑ Capslock↑ = Shift↓ Right↕ Shift↑
Capslock↓ S↓ J↓↑ S↑ Capslock↑ = Shift↓ Down↕ Shift↑
Capslock↓ Shift↓ J↓↑ Shift↑ Capslock↑ = Shift↓ Down↕ Shift↑
Capslock↓ S↓ K↓↑ S↑ Capslock↑ = Shift↓ Up↕ Shift↑
Capslock↓ Shift↓ K↓↑ Shift↑ Capslock↑ = Shift↓ Up↕ Shift↑
Capslock↓ S↓ H↓↑↓↑ S↑ Capslock↑ = Shift↓ Left↕↕ Shift↑
Capslock↓ Shift↓ H↓↑↓↑ Shift↑ Capslock↑ = Shift↓ Left↕↕ Shift↑
Capslock↓ S↓ L↓↑↓↑ S↑ Capslock↑ = Shift↓ Right↕↕ Shift↑
Capslock↓ Shift↓ L↓↑↓↑ Shift↑ Capslock↑ = Shift↓ Right↕↕ Shift↑
Capslock↓ S↓ J↓↑↓↑ S↑ Capslock↑ = Shift↓ Down↕↕ Shift↑
Capslock↓ Shift↓ J↓↑↓↑ Shift↑ Capslock↑ = Shift↓ Down↕↕ Shift↑
Capslock↓ S↓ K↓↑↓↑ S↑ Capslock↑ = Shift↓ Up↕↕ Shift↑
Capslock↓ Shift↓ K↓↑↓↑ Shift↑ Capslock↑ = Shift↓ Up↕↕ Shift↑
Capslock↓ Win↓ H↓↑ Win↑ Capslock↑ = Ctrl↓ Left↕ Ctrl↑
Capslock↓ Win↓ L↓↑ Win↑ Capslock↑ = Ctrl↓ Right↕ Ctrl↑
Capslock↓ Win↓ J↓↑ Win↑ Capslock↑ = Ctrl↓ Down↕ Ctrl↑
Capslock↓ Win↓ K↓↑ Win↑ Capslock↑ = Ctrl↓ Up↕ Ctrl↑
Capslock↓ Win↓ H↓↑↓↑ Win↑ Capslock↑ = Ctrl↓ Left↕ Ctrl↑↓ Left↕ Ctrl↑
Capslock↓ Win↓ L↓↑↓↑ Win↑ Capslock↑ = Ctrl↓ Right↕ Ctrl↑↓ Right↕ Ctrl↑
Capslock↓ Win↓ J↓↑↓↑ Win↑ Capslock↑ = Ctrl↓ Down↕ Ctrl↑↓ Down↕ Ctrl↑
Capslock↓ Win↓ K↓↑↓↑ Win↑ Capslock↑ = Ctrl↓ Up↕ Ctrl↑↓ Up↕ Ctrl↑
Capslock↓ S↓ Win↓ H↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Left↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ H↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Left↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ L↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Right↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ L↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Right↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ J↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Down↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ J↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Down↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ K↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Up↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ K↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Up↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ H↓↑↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Left↕ Ctrl↑↓ Left↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ H↓↑↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Left↕ Ctrl↑↓ Left↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ L↓↑↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Right↕ Ctrl↑↓ Right↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ L↓↑↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Right↕ Ctrl↑↓ Right↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ J↓↑↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Down↕ Ctrl↑↓ Down↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ J↓↑↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Down↕ Ctrl↑↓ Down↕ Ctrl↑ Shift↑
Capslock↓ S↓ Win↓ K↓↑↓↑ Win↑ S↑ Capslock↑ = Shift↓ Ctrl↓ Up↕ Ctrl↑↓ Up↕ Ctrl↑ Shift↑
Capslock↓ Shift↓ Win↓ K↓↑↓↑ Win↑ Shift↑ Capslock↑ = Shift↓ Ctrl↓ Up↕ Ctrl↑↓ Up↕ Ctrl↑ Shift↑
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

### LAlt
```
Alt↓ `↓↑ Tab↓↑ Alt↑ = Ctrl↓↑ Alt↓ `↕ Alt↑ Ctrl↓↑ Alt↓ Tab↕ Alt↑
Alt↓ Tab↓↑ Q↓↑ Alt↑ = Ctrl↓↑ Alt↓ Tab↕ Supr↕ Alt↑
Alt↓ Tab↓↑ Esc↓↑ Alt↑ = Ctrl↓↑ Alt↓ Tab↕ Esc↕ Alt↑ Ctrl↓↑
Alt↓ Shift↓ Tab↓↑↓↑ Shift↑ Alt↑ = Ctrl↓ Shift↓ Ctrl↑ Alt↓ Tab↕ Alt↓ Tab↕ Shift↑ Alt↑
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
