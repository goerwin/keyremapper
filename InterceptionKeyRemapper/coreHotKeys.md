### Caps + Alt
```
- CapsAlt Caps => Down(null) Up(null)
- CapsAlt Alt => Down(keyDownLAltAsLCtrl) Up(keyUpLAlt)
- CapsAlt _ => Down(null) Up(null)
```

### Caps
```
- Caps Tab => Down(Ctrl↓ Tab↕ Ctrl↑) Up(null)
- Caps Space => Down(Ctrl↓ Space↕ Ctrl↑) Up(null)
- Caps Caps => Down(null) Up(null)
- Caps _ => Down(null) Up(null)
```

### Ctrl + Shift
```
- CtrlShift Tab => Down(keyDownLCtrlAsLCtrl Tab↕) Up(null)
- CtrlShift Ctrl => Down(keyDownLCtrlAsLAlt) Up(keyUpLCtrl)
- CtrlShift Shift => Down(Shift↓) Up(Shift↑)
- CtrlShift _ => Down(_↓) Up(_↑)
```

### Ctrl
```
- Ctrl Tab => Down(keyDownLCtrlAsLCtrl Tab↕) Up(null)
- Ctrl Ctrl => Down(keyDownLCtrlAsLAlt) Up(keyUpLCtrl)
- Ctrl _ => Down(_↓) Up(_↑)
```

### Win + Alt
```
- WinAlt H => Down(keyUpLAlt Win↓ Left↕ Win↑) Up(null)
- WinAlt J => Down(keyUpLAlt Win↓ Down↕ Win↑) Up(null)
- WinAlt K => Down(keyUpLAlt Win↓ Up↕ Win↑) Up(null)
- WinAlt L => Down(keyUpLAlt Win↓ Right↕ Win↑) Up(null)
- WinAlt Win => Down(null) Up(keyDownLAltAsLCtrl)
- WinAlt Alt => Down(keyDownLAltAsLCtrl) Up(keyUpLAlt)
- WinAlt _ => Down(null) Up(null)
```

### Win + Shift
```
- WinShift Win => Down(null) Up(null)
- WinShift Shift => Down(Shift↓) Up(Shift↑)
- WinShift _ => Down(null) Up(null)
```

### Win
```
- Win D => Down(Win↓ D↕ Win↑) Up(null)
- Win Back => Down(Ctrl↓ Back↕ Ctrl↑) Up(null)
- Win Space => Down(Win↕) Up(null)
- Win Win => Down(null) Up(null)
- Win _ => Down(null) Up(null)
```

### Alt + Shift
```
- AltShift J => Down(keyUpLAlt Next↕ keyDownLAltAsLCtrl) Up(null)
- AltShift K => Down(keyUpLAlt Prior↕ keyDownLAltAsLCtrl) Up(null)
- AltShift Tab => Down(keyDownLAltAsLAlt Tab↕) Up(null)
- AltShift Alt => Down(keyDownLAltAsLCtrl) Up(keyUpLAlt)
- AltShift Shift => Down(Shift↓) Up(Shift↑)
- AltShift _ => Down(_↓) Up(_↑)
```

### Alt
```
- Alt J => Down(keyUpLAlt Next↕ keyDownLAltAsLCtrl) Up(null)
- Alt K => Down(keyUpLAlt Prior↕ keyDownLAltAsLCtrl) Up(null)
- Alt F1 => Down(keyUpLAlt F1↕ keyDownLAltAsLCtrl) Up(null)
- Alt F2 => Down(keyUpLAlt F2↕ keyDownLAltAsLCtrl) Up(null)
- Alt F10 => Down(keyUpLAlt F10↕ keyDownLAltAsLCtrl) Up(null)
- Alt F11 => Down(keyUpLAlt F11↕ keyDownLAltAsLCtrl) Up(null)
- Alt F12 => Down(keyUpLAlt F12↕ keyDownLAltAsLCtrl) Up(null)
- Alt Tab => Down(keyDownLAltAsLAlt Tab↕) Up(null)
- Alt Alt => Down(keyDownLAltAsLCtrl) Up(keyUpLAlt)
- Alt _ => Down(_↓) Up(_↑)
```

### Key
```
- Key F1 => Down(BRIGHTNESSDOWN↕) Up(null)
- Key F2 => Down(BRIGHTNESSUP↕) Up(null)
- Key F10 => Down(MUTE↕) Up(null)
- Key F11 => Down(VOLUMEDOWN↕↕↕↕) Up(null)
- Key F12 => Down(VOLUMEUP↕↕↕↕) Up(null)
- Key _ => Down(_↓) Up(_↑)
```