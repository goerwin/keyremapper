# KeyRemapper

<p align="center">
  <img src="common/images/readme-icon.png" width="128" alt="KeyRemapper icon">
</p>

Keyboard remapper for macOS and Windows, configured with a JSON file. On macOS it seizes the keyboards via IOKit and posts the remapped events, so no driver is needed. On Windows it uses the Interception driver.

## Installation

- Mac
  1. Download `mac.zip` from [Releases](https://github.com/goerwin/keyremapper/releases), unzip it and move `KeyRemapper.app` to Applications
  2. Open it. The first time macOS blocks it (it isn't notarized), so click _Open Anyway_ in _System Settings > Privacy & Security_
  3. Turn it on in _System Settings > General > Login Items & Extensions_ (it runs a background service that listens to the keyboards) and grant _Accessibility_ and _Input Monitoring_. Updating from 4.x asks for your password once, to remove the old helper
  4. Edit `~/KeyRemapperMac/config.json` (menu bar > _Open Config folder_). Saving any JSON file in that folder reloads the active profile

_Uninstall KeyRemapper…_ in the menu removes the background service, login item and permissions (the config folder is kept).
- Windows
  1. Install the [Interception driver](https://github.com/oblitum/Interception) and restart
  2. Download `win.zip` from [Releases](https://github.com/goerwin/keyremapper/releases) and copy the `keyRemapperWin` folder to your home directory (eg. `C:\Users\me`)
  3. Run the .exe, as Administrator if it has to work in privileged apps (eg. Task Manager)

## Config

```jsonc
{
  "profiles": [
    {
      "name": "My profile",
      // optionals, in ms
      "keyPressesDelay": 200,
      "delayUntilRepeat": 300,
      "keyRepeatInterval": 25,
      "doubleClickSpeed": 500,

      "remaps": [],
      "keyPresses": [],
      "keybindings": [],
      "tests": []
    }
  ]
}
```

Key names come from `symbols.json` ([Mac](mac/KeyRemapper/Resources/symbols.json), [Windows](win/src/files/symbols.json)). The app's _Logger_ shows the current app and keyboard names.

### Remaps

One to one remaps. The first one that matches wins.

```jsonc
[{ "from": "A", "to": "B" /* optional: "if" */ }]
```

### KeyPresses

Fire when a key is tapped N times within `keyPressesDelay`.

```jsonc
[
  {
    "key": "Shift",
    "ifPressedNTimes": 2,
    "send": "Cmd:down C Cmd:up"
    // optionals: "if", "set", "afterKeyUp"
  }
]
```

With `ifHeldFor` (ms) instead, they fire while the key is held, unless another key is pressed or released first. A held key isn't also a tap. Mac only.

```jsonc
[
  { "key": "Backspace", "ifPressedNTimes": 1, "send": "CtrlL:down ShiftL:down Tab ShiftL:up CtrlL:up" },
  { "key": "Backspace", "ifHeldFor": 400, "send": "CmdL:down W CmdL:up" },
  // keeps Cmd down until the key is released
  { "key": "AltR", "ifHeldFor": 300, "send": "CmdL:down Tab", "afterKeyUp": "CmdL:up" }
]
```

To only tap or hold, without the key's own output, add a keybinding with `"send": [null, null]` for it. In `tests`, `test_hold` stands for the held key's `ifHeldFor` time passing (eg. `["Backspace:down test_hold Backspace:up", "CmdL:down W CmdL:up"]`).

### Keybindings

For complex flows. The first one that matches wins.

```jsonc
[
  {
    "keys": ["A", "B"], // any of these keys
    "send": ["C", null], // [on key down, on key up]
    // optionals: "if", "set", "setOnKeyUp", "afterKeyUp"
  }
]
```

`send` examples:

```jsonc
"send": ["C:down C:up", null]                       // same as "C", with granular control
"send": ["CmdL:down C CmdL:up", null]
"send": ["currentKey:down", "currentKey:up"]         // the key that triggered the keybinding
"send": ["LeftClick:down", "LeftClick:up"]
"send": ["CmdL:down Tab SK:Delay:250 Tab CmdL:up", null] // SK:Delay:{ms}
```

### Conditions and variables

```jsonc
{
  "if": {
    "CmdL": true, // held
    "AltL": false, // not held (keys not listed aren't checked)
    "MY_VAR": false,

    // reserved
    "isKeyDown": false, // the current key was just released
    "appName": "com.google.Chrome",
    "keyboard": "4133:6421" // Mac: productId:vendorId, Windows: hardware id
  },
  "set": { "MY_VAR": true }, // on key down
  "setOnKeyUp": { "MY_VAR": false },
  "afterKeyUp": "C:down C:up O O L" // sent on the next key event after this key up
}
```

Conditions are checked again on key up, so a keybinding that holds a key down needs a matching rule that releases it.

### Sharing parts of the config

`%array`, `%dotdotdotArray`, `%object` and `%dotdotdotObject` load other JSON files (see [Tests/imports.json](Tests/imports.json) or my [config](https://github.com/goerwin/dotfiles/blob/master/src/keyRemapperMac/config.json)).

```jsonc
{
  "remaps": "%array(_remaps.json)",
  "keybindings": [
    "%dotdotdotArray(_vimMode.json)",
    "%object(_sharedKeybinding.json)",
    { "%dotdotdotObject": "(_sharedKeybinding2.json)" }
  ]
}
```

### Tests

Each profile can have `tests`: pairs of input key events and the expected events sent to the OS.

```jsonc
"tests": [
  ["Caps:down Caps:up", "Esc"],
  ["Caps:down test_delay:251 Caps:up", ""]
]
```

## Development

```sh
make test                            # engine tests (Tests/)
make test-runtime                    # Mac runtime tests: key events in, posted events out (mac/Tests/runtime.mm)
make test-app                        # builds, installs and drives the signed app's menu (mac/Tests/app.sh)
make test-config [CONFIG=...]        # tests of each profile (default ~/KeyRemapperMac/config.json)
make build                           # unsigned Debug build of the Mac app
make dev [CONFIG=... PROFILE=1 LOG=1] # run the remapper from the terminal
make icon                            # regenerates the app and menu bar icons from common/images/*.svg
```

- Use `$HOME` instead of `~` in `CONFIG`, zsh doesn't expand it there
- `make test-app` replaces the installed app and needs _Accessibility_ for the terminal app. It briefly saves an invalid config to check reloading, and restores yours after. Physical keystrokes can't be automated (it would need a virtual HID driver), so try a few keys after it
- `make dev` needs no app, helper or signing. Quit KeyRemapper first and grant _Input Monitoring_ and _Accessibility_ to the terminal app. Stop it with Ctrl+C
- Xcode: open `mac/KeyRemapper.xcodeproj` and sign both targets (KeyRemapper, Daemon) with your Apple Development certificate. The daemon only accepts apps signed by the same team
- The app only restarts the daemon when its version changes, so after changing the daemon bump the version
- Icons: `common/images/icon.svg` and `common/images/menubar-icon.svg` are the sources, edit them and run `make icon` (needs `brew install librsvg`). Don't edit the files in `Assets.xcassets` directly
- Windows: open the solution with Visual Studio 2019+. Tests: `cl .\Tests\index.cpp /std:c++17 /Fe"Tests/output.exe" /Fo"Tests/output.obj" | .\Tests\output.exe` from a Developer PowerShell

More dev notes in [NOTES.md](NOTES.md).

## Release

Push to `main`, then run `make release-patch`, `make release-minor` or `make release-major`. It previews the new version and its commits and, after confirmation, pushes an annotated tag. The [release workflow](.github/workflows/release.yml) builds both apps and publishes them, using the commits as release notes.

The workflow can also be run manually (_Actions > Run workflow_) to build without releasing.

The Mac build is signed with these repository secrets:

- `MAC_BUILD_CERTIFICATE_BASE64`: the Apple Development certificate (`.p12`) in base64 (`base64 -i certificate.p12`)
- `MAC_BUILD_CERTIFICATE_BASE64_PASSWORD`: its password
- `MAC_APP_CERTIFICATE`: its name, eg. `Apple Development: me@email.com (XXXXXXXXXX)`
