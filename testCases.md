# Test Cases

### Mouse
```
❖↓ C↓ C↑ ❖↑ = LeftClick↓ LeftClick↑
❖↓ Caps↓ C↓ C↑ Caps↑ ❖↑ = RightClick↓ RightClick↑
❖↓ C↓ C↓ C↓ C↓ C↑ ❖↑ = LeftClick↓ LeftClick↑
❖↓ Caps↓ C↓ C↓ C↓ C↓ C↑ Caps↓ ❖↑ = RightClick↓ RightClick↑
❖↓ ⇧↓ C↓ C↑ ⇧↑ ❖↑ = ⇧↓ LeftClick↓ LeftClick↑ ⇧↑
❖↓ Alt↓ C↓ C↑ Alt↑ ❖↑ = Ctrl↓ LeftClick↓ LeftClick↑ Ctrl↑
```

### Key
```
H↓ H↑ = H↓ H↑
H↓ H↓ H↓ = H↓ H↓ H↓
H↓ H↓ H↑ = H↓ H↓ H↑
H↓ J↓ J↑ H↑ = H↓ J↓ J↑ H↑
H↓ J↓ H↑ J↑ = H↓ J↓ H↑ J↑
H↓ J↓ K↓ K↑ J↑ H↑ = H↓ J↓ K↓ K↑ J↑ H↑
H↓ J↓ K↓ K↑ H↑ J↑ = H↓ J↓ K↓ K↑ H↑ J↑
F3↓ F3↑ = Ctrl↓ ⇧↓ ⭾↕ ⇧↑ Ctrl↑ // chrome.exe
F4↓ F4↑ = Ctrl↓ ⭾↕ Ctrl↑ // chrome.exe
F5↓ F5↑ = Alt↓ M↕ Alt↑ // chrome.exe
F6↓ F6↑ = Alt↓ T↕ Alt↑ // chrome.exe
F1↓ F1↑ = F1↓
F2↓ F2↑ = F1↓
F10↓ F10↑ = Mute↕
F11↓ F11↑ = VolumeDown↕ VolumeDown↕ VolumeDown↕ VolumeDown↕
F12↓ F12↑ = VolumeUp↕ VolumeUp↕ VolumeUp↕ VolumeUp↕
C↓ C↑ = C↓ C↑
```

### Vim Mode
```
Caps↓ H↓ H↑ Caps↑ = ⇦↕
Caps↓ L↓ L↑ Caps↑ = ⇨↕
Caps↓ J↓ J↑ Caps↑ = ⇩↕
Caps↓ K↓ K↑ Caps↑ = ⇧↕
Caps↓ H↓ H↑ H↓ H↑ Caps↑ = ⇦↕ ⇦↕
Caps↓ L↓ L↑ L↓ L↑ Caps↑ = ⇨↕ ⇨↕
Caps↓ J↓ J↑ J↓ J↑ Caps↑ = ⇩↕ ⇩↕
Caps↓ K↓ K↑ K↓ K↑ Caps↑ = ⇧↕ ⇧↕
Caps↓ H↓ H↑ Caps↑ = ⇦↕ // AppSwitcher
Caps↓ L↓ L↑ Caps↑ = ⇨↕ // AppSwitcher
Caps↓ J↓ J↑ Caps↑ = ⇩↕ // AppSwitcher
Caps↓ K↓ K↑ Caps↑ = ⇧↕ // AppSwitcher
Caps↓ H↓ H↑ H↓ H↑ Caps↑ = ⇦↕ ⇦↕ // AppSwitcher
Caps↓ L↓ L↑ L↓ L↑ Caps↑ = ⇨↕ ⇨↕ // AppSwitcher
Caps↓ J↓ J↑ J↓ J↑ Caps↑ = ⇩↕ ⇩↕ // AppSwitcher
Caps↓ K↓ K↑ K↓ K↑ Caps↑ = ⇧↕ ⇧↕ // AppSwitcher
Caps↓ S↓ H↓ H↑ S↑ Caps↑ = ⇧↓ ⇦↕ ⇧↑
Caps↓ ⇧↓ H↓ H↑ ⇧↑ Caps↑ = ⇧↓ ⇦↕ ⇧↑
Caps↓ S↓ L↓ L↑ S↑ Caps↑ = ⇧↓ ⇨↕ ⇧↑
Caps↓ ⇧↓ L↓ L↑ ⇧↑ Caps↑ = ⇧↓ ⇨↕ ⇧↑
Caps↓ S↓ J↓ J↑ S↑ Caps↑ = ⇧↓ ⇩↕ ⇧↑
Caps↓ ⇧↓ J↓ J↑ ⇧↑ Caps↑ = ⇧↓ ⇩↕ ⇧↑
Caps↓ S↓ K↓ K↑ S↑ Caps↑ = ⇧↓ ⇧↕ ⇧↑
Caps↓ ⇧↓ K↓ K↑ ⇧↑ Caps↑ = ⇧↓ ⇧↕ ⇧↑
Caps↓ S↓ H↓ H↑ H↓ H↑ S↑ Caps↑ = ⇧↓ ⇦↕ ⇦↕ ⇧↑
Caps↓ ⇧↓ H↓ H↑ H↓ H↑ ⇧↑ Caps↑ = ⇧↓ ⇦↕ ⇦↕ ⇧↑
Caps↓ S↓ L↓ L↑ L↓ L↑ S↑ Caps↑ = ⇧↓ ⇨↕ ⇨↕ ⇧↑
Caps↓ ⇧↓ L↓ L↑ L↓ L↑ ⇧↑ Caps↑ = ⇧↓ ⇨↕ ⇨↕ ⇧↑
Caps↓ S↓ J↓ J↑ J↓ J↑ S↑ Caps↑ = ⇧↓ ⇩↕ ⇩↕ ⇧↑
Caps↓ ⇧↓ J↓ J↑ J↓ J↑ ⇧↑ Caps↑ = ⇧↓ ⇩↕ ⇩↕ ⇧↑
Caps↓ S↓ K↓ K↑ K↓ K↑ S↑ Caps↑ = ⇧↓ ⇧↕ ⇧↕ ⇧↑
Caps↓ ⇧↓ K↓ K↑ K↓ K↑ ⇧↑ Caps↑ = ⇧↓ ⇧↕ ⇧↕ ⇧↑
Caps↓ ❖↓ H↓ H↑ ❖↑ Caps↑ = Ctrl↓ ⇦↕ Ctrl↑
Caps↓ ❖↓ L↓ L↑ ❖↑ Caps↑ = Ctrl↓ ⇨↕ Ctrl↑
Caps↓ ❖↓ J↓ J↑ ❖↑ Caps↑ = Ctrl↓ ⇩↕ Ctrl↑
Caps↓ ❖↓ K↓ K↑ ❖↑ Caps↑ = Ctrl↓ ⇧↕ Ctrl↑
Caps↓ ❖↓ H↓ H↑ H↓ H↑ ❖↑ Caps↑ = Ctrl↓ ⇦↕ Ctrl↑ Ctrl↓ ⇦↕ Ctrl↑
Caps↓ ❖↓ L↓ L↑ L↓ L↑ ❖↑ Caps↑ = Ctrl↓ ⇨↕ Ctrl↑ Ctrl↓ ⇨↕ Ctrl↑
Caps↓ ❖↓ J↓ J↑ J↓ J↑ ❖↑ Caps↑ = Ctrl↓ ⇩↕ Ctrl↑ Ctrl↓ ⇩↕ Ctrl↑
Caps↓ ❖↓ K↓ K↑ K↓ K↑ ❖↑ Caps↑ = Ctrl↓ ⇧↕ Ctrl↑ Ctrl↓ ⇧↕ Ctrl↑
Caps↓ S↓ ❖↓ H↓ H↑ ❖↑ S↑ Caps↑ = ⇧↓ Ctrl↓ ⇦↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ ❖↓ H↓ H↑ ❖↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ ⇦↕ Ctrl↑ ⇧↑
Caps↓ S↓ ❖↓ L↓ L↑ ❖↑ S↑ Caps↑ = ⇧↓ Ctrl↓ ⇨↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ ❖↓ L↓ L↑ ❖↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ ⇨↕ Ctrl↑ ⇧↑
Caps↓ S↓ ❖↓ J↓ J↑ ❖↑ S↑ Caps↑ = ⇧↓ Ctrl↓ ⇩↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ ❖↓ J↓ J↑ ❖↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ ⇩↕ Ctrl↑ ⇧↑
Caps↓ S↓ ❖↓ K↓ K↑ ❖↑ S↑ Caps↑ = ⇧↓ Ctrl↓ ⇧↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ ❖↓ K↓ K↑ ❖↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ ⇧↕ Ctrl↑ ⇧↑
Caps↓ S↓ ❖↓ H↓ H↑ H↓ H↑ ❖↑ S↑ Caps↑ = ⇧↓ Ctrl↓ ⇦↕ Ctrl↑ Ctrl↓ ⇦↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ ❖↓ H↓ H↑ H↓ H↑ ❖↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ ⇦↕ Ctrl↑ Ctrl↓ ⇦↕ Ctrl↑ ⇧↑
Caps↓ S↓ ❖↓ L↓ L↑ L↓ L↑ ❖↑ S↑ Caps↑ = ⇧↓ Ctrl↓ ⇨↕ Ctrl↑ Ctrl↓ ⇨↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ ❖↓ L↓ L↑ L↓ L↑ ❖↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ ⇨↕ Ctrl↑ Ctrl↓ ⇨↕ Ctrl↑ ⇧↑
Caps↓ S↓ ❖↓ J↓ J↑ J↓ J↑ ❖↑ S↑ Caps↑ = ⇧↓ Ctrl↓ ⇩↕ Ctrl↑ Ctrl↓ ⇩↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ ❖↓ J↓ J↑ J↓ J↑ ❖↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ ⇩↕ Ctrl↑ Ctrl↓ ⇩↕ Ctrl↑ ⇧↑
Caps↓ S↓ ❖↓ K↓ K↑ K↓ K↑ ❖↑ S↑ Caps↑ = ⇧↓ Ctrl↓ ⇧↕ Ctrl↑ Ctrl↓ ⇧↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ ❖↓ K↓ K↑ K↓ K↑ ❖↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ ⇧↕ Ctrl↑ Ctrl↓ ⇧↕ Ctrl↑ ⇧↑
Caps↓ Alt↓ H↓ H↑ Alt↑ Caps↑ = Ctrl↓ Ctrl↑ Home↕ Ctrl↓ Ctrl↑
Caps↓ Alt↓ L↓ L↑ Alt↑ Caps↑ = Ctrl↓ Ctrl↑ End↕ Ctrl↓ Ctrl↑
Caps↓ Alt↓ H↓ H↑ H↓ H↑ Alt↑ Caps↑ = Ctrl↓ Ctrl↑ Home↕ Ctrl↓ Ctrl↑ Home↕ Ctrl↓ Ctrl↑
Caps↓ Alt↓ L↓ L↑ L↓ L↑ Alt↑ Caps↑ = Ctrl↓ Ctrl↑ End↕ Ctrl↓ Ctrl↑ End↕ Ctrl↓ Ctrl↑
Caps↓ S↓ Alt↓ H↓ H↑ Alt↑ S↑ Caps↑ = ⇧↓ Ctrl↓ Ctrl↑ Home↕ Ctrl↓ Ctrl↑ ⇧↑
Caps↓ ⇧↓ Alt↓ H↓ H↑ Alt↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ Ctrl↑ Home↕ Ctrl↓ Ctrl↑ ⇧↑
Caps↓ S↓ Alt↓ L↓ L↑ Alt↑ S↑ Caps↑ = ⇧↓ Ctrl↓ Ctrl↑ End↕ Ctrl↓ Ctrl↑ ⇧↑
Caps↓ ⇧↓ Alt↓ L↓ L↑ Alt↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ Ctrl↑ End↕ Ctrl↓ Ctrl↑ ⇧↑
Caps↓ S↓ Alt↓ H↓ H↑ H↓ H↑ Alt↑ S↑ Caps↑ = ⇧↓ Ctrl↓ Ctrl↑ Home↕ Ctrl↓ Ctrl↑ Home↕ Ctrl↓ Ctrl↑ ⇧↑
Caps↓ ⇧↓ Alt↓ H↓ H↑ H↓ H↑ Alt↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ Ctrl↑ Home↕ Ctrl↓ Ctrl↑ Home↕ Ctrl↓ Ctrl↑ ⇧↑
Caps↓ S↓ Alt↓ L↓ L↑ L↓ L↑ Alt↑ S↑ Caps↑ = ⇧↓ Ctrl↓ Ctrl↑ End↕ Ctrl↓ Ctrl↑ End↕ Ctrl↓ Ctrl↑ ⇧↑
Caps↓ ⇧↓ Alt↓ L↓ L↑ L↓ L↑ Alt↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ Ctrl↑ End↕ Ctrl↓ Ctrl↑ End↕ Ctrl↓ Ctrl↑ ⇧↑
Caps↓ Alt↓ J↓ J↑ Alt↑ Caps↑ = Ctrl↓ End↕ Ctrl↑
Caps↓ Alt↓ K↓ K↑ Alt↑ Caps↑ = Ctrl↓ Home↕ Ctrl↑
Caps↓ Alt↓ J↓ J↑ J↓ J↑ Alt↑ Caps↑ = Ctrl↓ End↕ End↕ Ctrl↑
Caps↓ Alt↓ K↓ K↑ K↓ K↑ Alt↑ Caps↑ = Ctrl↓ Home↕ Home↕ Ctrl↑
Caps↓ S↓ Alt↓ J↓ J↑ Alt↑ S↑ Caps↑ = ⇧↓ Ctrl↓ End↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ Alt↓ J↓ J↑ Alt↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ End↕ Ctrl↑ ⇧↑
Caps↓ S↓ Alt↓ K↓ K↑ Alt↑ S↑ Caps↑ = ⇧↓ Ctrl↓ Home↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ Alt↓ K↓ K↑ Alt↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ Home↕ Ctrl↑ ⇧↑
Caps↓ S↓ Alt↓ J↓ J↑ J↓ J↑ Alt↑ S↑ Caps↑ = ⇧↓ Ctrl↓ End↕ End↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ Alt↓ J↓ J↑ J↓ J↑ Alt↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ End↕ End↕ Ctrl↑ ⇧↑
Caps↓ S↓ Alt↓ K↓ K↑ K↓ K↑ Alt↑ S↑ Caps↑ = ⇧↓ Ctrl↓ Home↕ Home↕ Ctrl↑ ⇧↑
Caps↓ ⇧↓ Alt↓ K↓ K↑ K↓ K↑ Alt↑ ⇧↑ Caps↑ = ⇧↓ Ctrl↓ Home↕ Home↕ Ctrl↑ ⇧↑
```

### LAlt
```
Alt↓ `↓ `↑ ⭾↓ ⭾↑ Alt↑ = Ctrl↓ Ctrl↑ Alt↓ `↕ Alt↑ Ctrl↓ Ctrl↑ Alt↓ ⭾↕ Alt↑
Alt↓ ⭾↓ ⭾↑ Q↓ Q↑ Alt↑ = Ctrl↓ Ctrl↑ Alt↓ ⭾↕ Supr↕ Alt↑
Alt↓ ⭾↓ ⭾↑ Esc↓ Esc↑ Alt↑ = Ctrl↓ Ctrl↑ Alt↓ ⭾↕ Esc↕ Alt↑ Ctrl↓ Ctrl↑
Alt↓ ⇧↓ ⭾↓ ⭾↑ ⭾↓ ⭾↑ ⇧↑ Alt↑ = Ctrl↓ ⇧↓ Ctrl↑ Alt↓ ⭾↕ Alt↓ ⭾↕ ⇧↑ Alt↑
Alt↓ Esc↓ Esc↑ Alt↑ = Ctrl↓ Ctrl↑
Alt↓ Q↓ Q↑ Alt↑ = Ctrl↓ Ctrl↑ Alt↓ F4↕ Alt↑ Ctrl↓ Ctrl↑
Alt↓ ⌫↓ ⌫↑ Alt↑ = Ctrl↓ Ctrl↑ ⇧↓ Home↕ ⇧↑ ⌫↕ Ctrl↓ Ctrl↑
Alt↓ J↓ J↑ Alt↑ = Ctrl↓ Ctrl↑ Next↕ Ctrl↓ Ctrl↑
Alt↓ Space↓ Space↑ Alt↑ = Ctrl↓ F12↕ Ctrl↑
Alt↓ F3↓ F3↑ Alt↑ = Ctrl↓ Ctrl↑ F3↕ Ctrl↓ Ctrl↑
Alt↓ ⭾↓ ⭾↑ C↓ C↑ = Ctrl↓ Ctrl↑ Alt↓ ⭾↕
Alt↓ ⭾↓ ⭾↑ = Ctrl↓ Ctrl↑ Alt↓ ⭾↕
Alt↓ C↓ = Ctrl↓ C↓
Alt↓ C↓ C↑ Alt↑ = Ctrl↓ C↓ C↑ Ctrl↑
Alt↓ = Ctrl↓
Alt↓ Alt↑ = Ctrl↓ Ctrl↑
```

### LCtrl
```
Ctrl↓ ⭾↓ ⭾↑ Ctrl↑ = Alt↓ Alt↑ Ctrl↓ ⭾↕ Ctrl↑
```

### LCtrl or LAlt
```
Ctrl↓ C↓ C↑ Ctrl↑ = Alt↓ C↓ C↑ Alt↑
Ctrl↓ C↓ = Alt↓ C↓
Alt↓ C↓ C↑ Alt↑ = Ctrl↓ C↓ C↑ Ctrl↑
Alt↓ C↓ = Ctrl↓ C↓
Ctrl↓ ⇧↓ ⇧↑ Ctrl↑ = Alt↓ ⇧↓ ⇧↑ Alt↑
Alt↓ ⇧↓ ⇧↑ Alt↑ = Ctrl↓ ⇧↓ ⇧↑ Ctrl↑
Ctrl↓ Ctrl↑ = Alt↓ Alt↑
Ctrl↓ = Alt↓
Alt↓ Alt↑ = Ctrl↓ Ctrl↑
Alt↓ = Ctrl↓
```

### LWin
```
❖↓ 1↓ 1↑ ❖↑ = Alt↓ 1↕ Alt↑ // SC2_x64.exe
❖↓ ⌫↓ ⌫↑ ❖↑ = Ctrl↓ W↕ Ctrl↑ // mintty.exe
❖↓ H↓ H↑ ❖↑ = Alt↓ ⇦↕ Alt↑
❖↓ L↓ L↑ ❖↑ = Alt↓ ⇨↕ Alt↑
❖↓ ⌫↓ ⌫↑ ❖↑ = Ctrl↓ ⌫↕ Ctrl↑
❖↓ D↓ D↑ ❖↑ = ❖↓ D↕ ❖↑
❖↓ ❖↑ = 
```

### Shift
```
⇧↓ C↓ C↑ ⇧↑ = ⇧↓ C↓ C↑ ⇧↑
⇧↓ ⇧↑ = ⇧↓ ⇧↑
```
