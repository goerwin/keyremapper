# KeyRemapper

<p align="center">
  <img src="images/readme-icon.png" width="128" alt="KeyRemapper icon">
</p>

Keyboard remapper for macOS, configured with a JSON file. It seizes the keyboards via IOKit and posts the remapped events, so no driver is needed.

## Installation

1. Download `mac.zip` from [Releases](https://github.com/goerwin/keyremapper/releases), unzip it and move `KeyRemapper.app` to Applications
2. Open it. The first time macOS blocks it (it isn't notarized), so click _Open Anyway_ in _System Settings > Privacy & Security_
3. Turn it on in _System Settings > General > Login Items & Extensions_ (it runs a background service that listens to the keyboards) and grant _Accessibility_ and _Input Monitoring_
4. Edit `~/KeyRemapperMac/config.json` (menu bar > _Open Config folder_). Saving any JSON file in that folder reloads the active profile

_Uninstall KeyRemapper…_ in the menu removes the background service, login item and permissions (the config folder is kept).

## Config

```jsonc
{
  "profiles": [
    {
      "name": "My profile",
      // optionals, in ms
      "tapDelay": 200,
      "holdDelay": 400,
      "delayUntilRepeat": 300,
      "keyRepeatInterval": 25,
      "doubleClickSpeed": 500,

      "remaps": [],
      "rules": [],
      "tests": []
    }
  ]
}
```

Key names come from [symbols.json](KeyRemapper/Resources/symbols.json). The app's _Logger_ shows the current app and keyboard names. Mistakes (unknown fields or keys) are reported when the profile loads.

### Remaps

One to one, before the rules. The first one that matches wins.

```jsonc
[
  { "from": "CmdL", "to": "AltL", "keyboard": "50475:1133" },
  { "from": "Caps", "to": "F18" }
]
```

### Rules

The first rule that matches the pressed key replaces it, until it's released. Keys without a rule pass through.

```jsonc
[
  {
    "from": ["H"], // any of these keys
    "modifiers": ["CmdL"], // held, the others can't be (optional)
    "optional": ["ShiftL"], // can also be held, ["any"] for all (optional)
    "app": "com.google.Chrome", // or ["com.google.Chrome", "com.apple.finder"] (optional)
    "keyboard": "50475:1133", // productId:vendorId, a string or array too (optional)

    "to": "LeftArrow", // on key down, without it the key is silenced
    "tap": "Esc", // released within tapDelay, without pressing other keys
    "doubleTap": "CmdL+F", // tapped twice
    "hold": "CmdL+W" // held for holdDelay, without other key events
  }
]
```

Only `from` is required. Any key can be a modifier (eg. `["F18", "F"]`), but only the real modifiers (Cmd, Alt, Ctrl, Shift and Fn) have to be listed in `modifiers` or `optional` to be held. The keys sent by the `modifiers` are released while the rule's key is pressed, so `Cmd + H` sending `LeftArrow` doesn't send `Cmd + LeftArrow`.

Each action is space separated. Like Karabiner, the last key of `to` and `hold` is held until the key is released, the others are tapped. `tap` and `doubleTap` come on release, so all their keys are tapped. The keys of `hold` don't repeat.

```jsonc
"to": "C"                      // C, held
"to": "CmdL+ShiftL+C"          // with modifiers
"to": "Tab delay:250 Tab"      // waits 250ms
"to": "currentKey"             // the pressed key
```

With several keys, an action can also have one item per key:

```jsonc
{
  "from": ["H", "J", "K", "L"],
  "modifiers": ["F18"],
  "optional": ["any"],
  "to": ["LeftArrow", "DownArrow", "UpArrow", "RightArrow"]
}
```

### Sharing parts of the config

`"%import(file.json)"` is replaced by that file, relative to the one importing it. In an array, an imported array is spread into it (see [Tests/imports.json](Tests/imports.json) or my [config](https://github.com/goerwin/dotfiles/blob/master/src/keyRemapperMac/config.json)).

```jsonc
{
  "remaps": "%import(_remaps.json)",
  "rules": ["%import(_vimMode.json)", { "from": ["A"], "to": "B" }]
}
```

### Tests

Each profile can have `tests`: pairs of input key events and the expected events sent to the OS. Inputs can also have `delay:{ms}`, `app:{name}` and `keyboard:{id}`.

```jsonc
"tests": [
  ["Caps", "Esc"],
  ["Caps:down delay:250 Caps:up", ""],
  ["app:com.google.Chrome CmdL:down H CmdL:up", "CmdL:down CmdL:up LeftArrow CmdL:down CmdL:up"]
]
```

## Development

```sh
make test                            # engine tests (Tests/)
make test-runtime                    # Mac runtime tests: key events in, posted events out (Tests/runtime.mm)
make test-app                        # builds, installs and drives the signed app's menu (Tests/app.sh)
make test-config [CONFIG=...]        # tests of each profile (default ~/KeyRemapperMac/config.json)
make build                           # unsigned Debug build of the Mac app
make dev [CONFIG=... PROFILE=1 LOG=1] # run the remapper from the terminal
make icon                            # regenerates the app and menu bar icons from images/*.svg
```

- Use `$HOME` instead of `~` in `CONFIG`, zsh doesn't expand it there
- `make test-app` replaces the installed app and needs _Accessibility_ for the terminal app. It briefly saves an invalid config to check reloading, and restores yours after. Physical keystrokes can't be automated (it would need a virtual HID driver), so try a few keys after it
- `make dev` needs no app, helper or signing. Quit KeyRemapper first and grant _Input Monitoring_ and _Accessibility_ to the terminal app. Stop it with Ctrl+C
- Xcode: open `KeyRemapper.xcodeproj` and sign both targets (KeyRemapper, Daemon) with your Apple Development certificate. The daemon only accepts apps signed by the same team
- The app only restarts the daemon when its version changes, so after changing the daemon bump the version
- Icons: `images/icon.svg` and `images/menubar-icon.svg` are the sources, edit them and run `make icon` (needs `brew install librsvg`). Don't edit the files in `Assets.xcassets` directly

More dev notes in [NOTES.md](NOTES.md).

## Release

Push to `main`, then run `make release-patch`, `make release-minor` or `make release-major`. It previews the new version and its commits and, after confirmation, pushes an annotated tag. The [release workflow](.github/workflows/release.yml) builds the app and publishes it, using the commits as release notes.

The workflow can also be run manually (_Actions > Run workflow_) to build without releasing.

The Mac build is signed with these repository secrets:

- `MAC_BUILD_CERTIFICATE_BASE64`: the Apple Development certificate (`.p12`) in base64 (`base64 -i certificate.p12`)
- `MAC_BUILD_CERTIFICATE_BASE64_PASSWORD`: its password
- `MAC_APP_CERTIFICATE`: its name, eg. `Apple Development: me@email.com (XXXXXXXXXX)`
