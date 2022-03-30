## Installation

- Mac
  - Download the latest version from `Releases`
  - On first time, it will ask you to install the Helper (Service that will listen to Keyboard input via IOKit)
  - Click the app icon in the menu bar and click `Open Config folder`. You'll find the config file `~/KeyRemapperMac/config.json`
  - Config usage in [Usage section](#user-content-usage)
  - Reselect the profile from the app menu to reload it
- Windows
  - Install Interception driver following https://github.com/oblitum/Interception (it requires restart)
  - Download the latest version from `Releases`
  - Copy the `keyRemapperWin` folder to your home directory (eg. `C:\Users\me`)
  - Run the .exe with Administrator Rights (It is not necessary but to be able to run inside eg. Task Admin or some other privileged app, admin rights are needed)
  - Config usage in [Usage section](#user-content-usage)

## Usage

Your initial `config.json` will be similar to the one below

```jsonc
{
  "profiles": [
    {
      "remaps": [],
      "keyPresses": [],
      "keybindings": []
    }
  ]
}
```

### Remaps

One to one remaps of keys, as soon a remap meets criteria, others are skipped

```jsonc
[
  // examples
  { "from": "A", "to": "B" },
  {
    "from": "C",
    "to": "D"

    // optionals:

    // "if"
  }
]
```

### KeyPresses

```jsonc
[
  {
    "key": "Shift",
    "ifPressedNTimes": 2,

    // "send": keyEvents
    // more examples of keyEvents down below
    "send": "Cmd:down C Cmd:up"

    // optionals:

    // "set"
    // "if"
    // "afterKeyUp"
  }
]
```

### Keybindings

Used for complex flows, once a keybinding meets criteria, others are skipped

```jsonc
[
  {
    /*
      Listen for any of these keys
      - you can find a list of these in the (win|mac symbols.json file
      - eg. listen for A or B
    */
    "keys": ["A", "B"],

    // Tuple, first item will fire on key down, second item on key up
    // - "send": [keyEvents, keyEvents]
    // Examples:

    // these are the same but :down/:up gives more granular control
    "send": ["C", null],
    "send": ["C:down C:up", null],

    "send": ["CmdL:down C CmdL:up", null],

    // currentKey is the key that activated this
    "send": ["currentKey:down", "currentKey:up"],

    "send": ["LeftClick", null],
    "send": ["LeftClick:down", "LeftClick:up"],

    // Special key Delay SK:Delay:{ms}
    "send": ["CmdL:down Tab SK:Delay:250 Tab CmdL:up", null]

    // optionals:

    // "if"
    // "set"
    // "setOnKeyUp"
    // "afterKeyUp"
  }
]
```

### Shared

`keyEvents` referenced in `send` properties can have keys found in `symbols.json` (see [reference](#user-content-reference)) and other reserved keys.

Common key properties used above

```jsonc
{
  // conditions that should be met
  "if": {
    "CmdL": true,
    "AltL": true,
    "MY_CUSTOM_VAR": false,

    // Reserved keys
    "isKeyDown": false, // useful to check if current key was just released
    "appName": "com.google.Chrome",
    "keyboard": "1234"
  },

  // fired on the next key event after this keyup
  // "afterKeyUp": keyEvents
  "afterKeyUp": "C:down C:up O O L",

  // Set custom variables
  // - don't use reserved keys
  // - set works on keydown, setOnKeyUp on keyup
  "set": { "MY_CUSTOM_VAR": true },
  "setOnKeyUp": { "MY_CUSTOM_VAR": false }
}
```

Also, sharing arrays and objects inside the `config.json` file can be achieved using the special helpers `%array`, `%dotdotdotArray`, `%object` and `%dotdotdotObject`. Check my personal config or the tests for this functionality (see [reference](#user-content-reference))

```jsonc
{
  "profiles": [
    {
      "name": "Profile 1",
      "remaps": "%array(_remaps.json)",
      "keyBindings": [
        "%dotdotdotArray(_vimMode.json)",
        "%object(_sharedKeybinding)",
        {
          "%dotdotdotObject": "(_sharedKeybinding2.json)"
        }
      ]
    },

    {
      "name": "Profile 2",
      "remaps": "%array(_remaps.json)",
      "keyBindings": [
        "%dotdotdotArray(_vimMode.json)",
        "%object(_sharedKeybinding)",
        {
          "%dotdotdotObject": "(_sharedKeybinding2.json)"
        }
      ]
    }
  ]
}
```

## Build

- Mac
  - Create a `mac/.env` file using `mac/.env.example` and fill up the values
  - Open project `~mac/KeyRemapper.xcodeproj` with XCode
  - Sign the 2 targets (KeyRemapper, Daemon)
  - You should be able to run it on Xcode
- Windows
  - In Visual Studio 2019+, run a batch build and mark all projects, they all should REBUILD successfully
  - Navigate to project's root folder and run `$ bash win/release.sh` in WSL Ubuntu's terminal to generate the KeyRemapper.zip file with .exes for x86 and x64. (You need zip installed in terminal, also not sure if this works with Powershell)
  - File `win/output/KeyRemapper.zip` is generated

## Tests

- Mac
  ```sh
    $ g++ -o Tests/output -std=c++17 Tests/index.cpp && ./Tests/output
  ```
- Windows
  ```sh
    # open `Developer PowerShell for VS 2022` or similar
    $ cl .\Tests\index.cpp /std:c++17 /Fe"Tests/output.exe" /Fo"Tests/output.obj" | .\Tests\output.exe
  ```

## Reference

- Current keyboard and app names can be found using the `Logger`
- Mac's [symbols.json](mac/KeyRemapper/Resources/symbols.json)
- Windows' [symbols.json](win/src/files/symbols.json)
- My personal Mac's [config.json](https://github.com/goerwin/dotfiles/blob/master/src/keyRemapperMac/config.json)
- My personal Window's [config.json](https://github.com/goerwin/dotfiles/blob/master/src/keyRemapperWin/config.json)
- [Json Helpers test example](Tests/imports.json)
- [Notes](notes.md)
