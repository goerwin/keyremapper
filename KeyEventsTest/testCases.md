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

### 1 Modifier - Caps
```
Caps↓↑ = 
```

### 1 Modifier - Caps + _
```
Caps↓ _↓↑ Caps↑ = 
```

### 1 Modifier - Ctrl
```
Ctrl↓↑ = Alt↓↑
```

### 1 Modifier - Ctrl + _
```
Ctrl↓ _↓↑ Ctrl↑ = Alt↓ _↓↑ Alt↑
```

### 1 Modifier - Win
```
Win↓↑ = 
```

### 1 Modifier - Win + _
```
Win↓ _↓↑ Win↑ = 
```

### 1 Modifier - Alt
```
Alt↓↑ = Ctrl↓↑
```

### 1 Modifier - Alt + _
```
Alt↓ _↓↑ Alt↑ = Ctrl↓ _↓↑ Ctrl↑
```

### 1 Modifier - Shift
```
Shift↓↑ = Shift↓↑
```

### 1 Modifier - Shift + _
```
Shift↓ _↓↑ Shift↑ = Shift↓ _↓↑ Shift↑
```

### 2 Modifiers - Caps + Shift
```
Caps↓ Shift↓↑ Caps↑ = Shift↓↑
```

### 2 Modifiers - Caps + Shift + _
```
Caps↓ Shift↓ _↓↑ Shift↑ Caps↑ = Shift↓↑
```

### 2 Modifiers - Caps + Alt
```
Caps↓ Alt↓↑ Caps↑ = Ctrl↓↑
```

### 2 Modifiers - Caps + Alt + _
```
Caps↓ Alt↓ _↓↑ Alt↑ Caps↑ = Ctrl↓↑
```

### 2 Modifiers - Ctrl + Shift
```
Ctrl↓ Shift↓↑ Ctrl↑ = Alt↓ Shift↓↑ Alt↑
```

### 2 Modifiers - Ctrl + Shift + _
```
Ctrl↓ Shift↓ _↓↑ Shift↑ Ctrl↑ = Alt↓ Shift↓ _↓↑ Shift↑ Alt↑
```

### 2 Modifiers - Win + Shift
```
Win↓ Shift↓↑ Win↑ = Shift↓↑
```

### 2 Modifiers - Win + Shift + _
```
Win↓ Shift↓ _↓↑ Shift↑ Win↑ = Shift↓↑
```

### 2 Modifiers - Win + Alt
```
Win↓ Alt↓↑ Win↑ = Ctrl↓↑
```

### 2 Modifiers - Win + Alt + _
```
Win↓ Alt↓ _↓↑ Alt↑ Win↑ = Ctrl↓↑
```

### Key - DownUp
```
H↓↑ = H↓↑
```

### Key - Down
```
H↓↓↓ = H↓↓↓
```

### Key - Down then Up
```
H↓↓↑ = H↓↓↑
```

### Key - 2 Keys In Order
```
H↓ J↓↑ H↑ = H↓ J↓↑ H↑
```

### Key - 2 Keys No Order
```
H↓ J↓ H↑ J↑ = H↓ J↓ H↑ J↑
```

### Key - 3 Keys In Order
```
H↓ J↓ K↓↑ J↑ H↑ = H↓ J↓ K↓↑ J↑ H↑
```

### Key - 3 Keys No Order
```
H↓ J↓ K↓↑ H↑ J↑ = H↓ J↓ K↓↑ H↑ J↑
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

### Win + Alt
```
Win↓ Alt↓ H↓↑ Alt↑ Win↑ = Ctrl↓↑ Win↓ Left↕ Win↑ Ctrl↑
Win↓ Alt↓ J↓↑ Alt↑ Win↑ = Ctrl↓↑ Win↓ Down↕ Win↑ Ctrl↑
Win↓ Alt↓ K↓↑ Alt↑ Win↑ = Ctrl↓↑ Win↓ Up↕ Win↑ Ctrl↑
Win↓ Alt↓ L↓↑ Alt↑ Win↑ = Ctrl↓↑ Win↓ Right↕ Win↑ Ctrl↑
```

### Ctrl
```
Ctrl↓ Tab↓↑ Ctrl↑ = Alt↓↑ Ctrl↓ Tab↕ Ctrl↑
Ctrl↓ Tab↓↑ Ctrl↑ = Alt↓↑ Ctrl↓ Tab↕ Ctrl↑
```

### Alt
```
Alt↓ Esc↓↑ Alt↑ = Ctrl↓↑
Alt↓ Q↓↑ Alt↑ = Ctrl↓↑ Alt↓ F4↕ Alt↑ Ctrl↓↑
Alt↓ J↓↑ Alt↑ = Ctrl↓↑ Next↕ Ctrl↓↑
Alt↓ K↓↑ Alt↑ = Ctrl↓↑ Prior↕ Ctrl↓↑
Alt↓ Shift↓ J↓↑ Shift↑ Alt↑ = Ctrl↓ Shift↓ Ctrl↑ Next↕ Ctrl↓ Shift↑ Ctrl↑
Alt↓ Shift↓ K↓↑ Shift↑ Alt↑ = Ctrl↓ Shift↓ Ctrl↑ Prior↕ Ctrl↓ Shift↑ Ctrl↑
Alt↓ Tab↓↑ = Ctrl↓↑ Alt↓ Tab↕
Alt↓ Shift↓ Tab↓↑ Shift↑ Alt↑ = Ctrl↓ Shift↓ Ctrl↑ Alt↓ Tab↕ Shift↑ Alt↑
Alt↓ Tab↓↑ C↓↑ = Ctrl↓↑ Alt↓ Tab↕
Alt↓ Tab↓↑ Esc↓↑ Alt↑ = Ctrl↓↑ Alt↓ Tab↕ Esc↕ Alt↑ Ctrl↓↑
Alt↓ Tab↓↑ Q↓↑ Alt↑ = Ctrl↓↑ Alt↓ Tab↕ Supr↕ Alt↑
```
