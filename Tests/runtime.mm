// Runtime tests: feeds key events to the Runtime and checks the events it
// posts, without seizing the keyboards or posting anything to the OS.
// Usage: runtime-tests <runtime.json> <symbols.json>

#import "AppKit/AppKit.h"

#include <filesystem>

#import "../Common/Config.hpp"
#import "../Daemon/Runtime.hpp"

static nlohmann::json symbols;
static std::vector<std::string> posted;
static std::vector<std::chrono::steady_clock::time_point> repeatTimes;
static int failures = 0;

long msSince(std::chrono::steady_clock::time_point from,
             std::chrono::steady_clock::time_point to) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(to - from)
      .count();
}

ushort scancode(std::string key) { return symbols[key][0]; }
ushort vkCode(std::string key) { return symbols[key][3]; }

// eg. "key:11:down", "key:9:up+cmd", "key:11:down:repeat", "mod:55+cmd",
// "media:0:down", "click:left:down:1"
std::string describe(CGEventRef event) {
  auto type = CGEventGetType(event);
  auto flags = CGEventGetFlags(event);
  std::string mods;
  if (flags & kCGEventFlagMaskCommand) mods += "+cmd";
  if (flags & kCGEventFlagMaskShift) mods += "+shift";
  if (flags & kCGEventFlagMaskAlphaShift) mods += "+caps";

  if (type == kCGEventKeyDown || type == kCGEventKeyUp) {
    auto code = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    auto isRepeat =
        CGEventGetIntegerValueField(event, kCGKeyboardEventAutorepeat);
    return "key:" + std::to_string(code) +
           (type == kCGEventKeyDown ? ":down" : ":up") + mods +
           (isRepeat ? ":repeat" : "");
  }

  // Modifier keys, whether they are held shows in the flags
  if (type == kCGEventFlagsChanged) {
    auto code = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    return "mod:" + std::to_string(code) + mods;
  }

  if (type == kCGEventLeftMouseDown || type == kCGEventLeftMouseUp) {
    auto clicks = CGEventGetIntegerValueField(event, kCGMouseEventClickState);
    return std::string("click:left:") +
           (type == kCGEventLeftMouseDown ? "down" : "up") + mods + ":" +
           std::to_string(clicks);
  }

  auto data1 = [NSEvent eventWithCGEvent:event].data1;
  return "media:" + std::to_string((data1 & 0xffff0000) >> 16) +
         (((data1 & 0xff00) >> 8) == 0xa ? ":down" : ":up");
}

void press(Runtime& runtime, std::string key, bool isKeyDown) {
  runtime.handleInput(scancode(key), isKeyDown, 1, 2, "Test", "Keyboard");
}

void tap(Runtime& runtime, std::string key) {
  press(runtime, key, true);
  press(runtime, key, false);
}

void wait(double ms) {
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, ms / 1000, false);
}

void expect(std::string name, std::vector<std::string> expected) {
  if (posted == expected) {
    Helpers::print("ok: " + name);
  } else {
    failures++;
    std::string actual, wanted;
    for (auto& e : posted) actual += e + " ";
    for (auto& e : expected) wanted += e + " ";
    Helpers::print("FAIL: " + name + "\n  expected: " + wanted +
                   "\n  actual:   " + actual);
  }
  posted.clear();
}

void check(std::string name, bool value) {
  if (value) return Helpers::print("ok: " + name);
  failures++;
  Helpers::print("FAIL: " + name);
}

CGEventRef systemDefinedEvent(int nxKeyType, short subtype = 8) {
  NSEvent* nsEvent = [NSEvent otherEventWithType:NSEventTypeSystemDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0xa00
                                       timestamp:0
                                    windowNumber:0
                                         context:0
                                         subtype:subtype
                                           data1:(nxKeyType << 16) | (0xa << 8)
                                           data2:-1];
  return [nsEvent CGEvent];
}

CGEventRef keyboardEvent(ushort vkCode) {
  return (CGEventRef)CFAutorelease(
      CGEventCreateKeyboardEvent(NULL, vkCode, true));
}

bool isConsumerKeyEvent(CGEventRef event) {
  return Keyboards::isConsumerKeyEvent(CGEventGetType(event), event);
}

std::string key(std::string name, std::string suffix) {
  return "key:" + std::to_string(vkCode(name)) + ":" + suffix;
}

int main(int argc, const char* argv[]) {
  if (argc < 3) {
    Helpers::print("Usage: runtime-tests <runtime.json> <symbols.json>");
    return 2;
  }

  symbols = Helpers::getJsonFile(argv[2]);
  // As the app sends them
  auto config = Config::resolve(argv[1]);
  std::ifstream symbolsFile(argv[2]);
  std::string symbolsJson((std::istreambuf_iterator<char>(symbolsFile)),
                          std::istreambuf_iterator<char>());
  std::vector<std::string> errors;
  std::vector<bool> capslockStates;

  Runtime runtime;
  runtime.postEvent = [](CGEventRef event) {
    posted.push_back(describe(event));
    if (CGEventGetIntegerValueField(event, kCGKeyboardEventAutorepeat))
      repeatTimes.push_back(std::chrono::steady_clock::now());
  };
  runtime.setCapslock = [&](bool state) { capslockStates.push_back(state); };
  runtime.onError = [&](std::string err) { errors.push_back(err); };

  runtime.load(config, symbolsJson, 0, "");

  tap(runtime, "A");
  expect("remaps", {key("B", "down"), key("B", "up")});

  tap(runtime, "Z");
  expect("passes unmapped keys through", {key("Z", "down"), key("Z", "up")});

  // Keypad 1, which has no symbol
  runtime.handleInput(89, true, 1, 2, "Test", "Keyboard");
  runtime.handleInput(89, false, 1, 2, "Test", "Keyboard");
  expect("drops keys without a symbol", {});

  tap(runtime, "Application");
  expect("passes the Application key through",
         {key("Application", "down"), key("Application", "up")});

  check("maps the consumer keys to their symbols",
        Keyboards::getScancode(kHIDPage_Consumer, 0xcd) == scancode("PlayPause") &&
            Keyboards::getScancode(kHIDPage_Consumer, 0xcf) == scancode("Dictation") &&
            Keyboards::getScancode(kHIDPage_Consumer, 0xe2) == scancode("Mute") &&
            Keyboards::getScancode(kHIDPage_Consumer, 0xea) == scancode("VolumeDown") &&
            Keyboards::getScancode(kHIDPage_Consumer, 0xe9) == scancode("VolumeUp") &&
            Keyboards::getScancode(kHIDPage_Consumer, 0x224) == scancode("Back"));
  check("ignores the other consumer usages",
        Keyboards::getScancode(kHIDPage_Consumer, 0) == -1 &&
            Keyboards::getScancode(kHIDPage_Consumer, 0xffffffff) == -1);
  check("doesn't take consumer usages as keyboard ones",
        Keyboards::getScancode(kHIDPage_KeyboardOrKeypad, 0xe2) == scancode("AltL") &&
            Keyboards::getScancode(0xff, 3) == scancode("Fn"));

  check("tells the events macOS makes from consumer keys apart",
        isConsumerKeyEvent(systemDefinedEvent(NX_KEYTYPE_PLAY)) &&
            isConsumerKeyEvent(systemDefinedEvent(NX_KEYTYPE_SOUND_UP)) &&
            isConsumerKeyEvent(keyboardEvent(176)) &&
            !isConsumerKeyEvent(systemDefinedEvent(NX_KEYTYPE_BRIGHTNESS_UP)) &&
            !isConsumerKeyEvent(systemDefinedEvent(NX_KEYTYPE_PLAY, 7)) &&
            !isConsumerKeyEvent(keyboardEvent(vkCode("A"))));

  tap(runtime, "Mute");
  expect("passes consumer keys through",
         {"media:" + std::to_string(NX_KEYTYPE_MUTE) + ":down",
          "media:" + std::to_string(NX_KEYTYPE_MUTE) + ":up"});

  tap(runtime, "Dictation");
  expect("passes the Dictation key through",
         {key("Dictation", "down"), key("Dictation", "up")});

  tap(runtime, "Back");
  expect("remaps the keys without a Mac key",
         {key("Enter", "down"), key("Enter", "up")});

  tap(runtime, "M");
  expect("doesn't post the keys without a Mac key", {});

  tap(runtime, "C");
  expect("adds the held modifiers to the keys",
         {"mod:55+cmd", key("V", "down+cmd"), key("V", "up+cmd"), "mod:55"});

  // Timers fire late on slow machines (CI) but never before their deadline,
  // so only the minimum times are checked. A late repeat can be followed
  // closely by the next one, as the deadlines don't move. The default delay
  // (250) wouldn't repeat here
  repeatTimes.clear();
  auto pressTime = std::chrono::steady_clock::now();
  press(runtime, "A", true);
  wait(100 + 20 * 5);
  std::string times;
  bool isOnTime = !repeatTimes.empty();
  for (size_t i = 0; i < repeatTimes.size(); i++) {
    times += std::to_string(msSince(pressTime, repeatTimes[i])) + "ms ";
    if (msSince(pressTime, repeatTimes[i]) < 100 + 20 * (long)i)
      isOnTime = false;
  }
  check("repeats after delayUntilRepeat, every keyRepeatInterval (" + times +
            ")",
        isOnTime);
  posted.clear();
  press(runtime, "A", false);
  wait(100);
  expect("stops repeating on key up", {key("B", "up")});

  press(runtime, "A", true);
  runtime.stop();
  wait(150);
  expect("stops repeating on stop", {key("B", "down")});
  runtime.load(config, symbolsJson, 0, "");

  // The main run loop also serves the keyboards and the mouse, so it can't
  // sleep through the delay. Blocking would post everything during the taps
  auto start = std::chrono::steady_clock::now();
  tap(runtime, "D");
  tap(runtime, "Z");
  expect("doesn't block during a delay, sends the keys before it",
         {key("Tab", "down"), key("Tab", "up")});
  while (posted.empty() && msSince(start, std::chrono::steady_clock::now()) < 1000)
    wait(5);
  auto delayTime = msSince(start, std::chrono::steady_clock::now());
  check("waits for the delay (" + std::to_string(delayTime) + "ms)",
        delayTime >= 60);
  expect("sends the keys after the delay, then the ones pressed meanwhile",
         {key("Tab", "down"), key("Tab", "up"), key("Z", "down"),
          key("Z", "up")});

  tap(runtime, "D");
  runtime.stop();
  wait(100);
  expect("drops the keys after a delay on stop",
         {key("Tab", "down"), key("Tab", "up")});
  runtime.load(config, symbolsJson, 0, "");

  tap(runtime, "E");
  expect("posts media keys down and up",
         {"media:" + std::to_string(NX_KEYTYPE_SOUND_UP) + ":down",
          "media:" + std::to_string(NX_KEYTYPE_SOUND_UP) + ":up"});

  // Wide margins, since timers fire late on slow machines (CI)
  press(runtime, "I", true);
  wait(50);
  expect("waits for the hold", {});
  press(runtime, "I", false);
  expect("taps when released before the hold", {key("J", "down"), key("J", "up")});
  wait(300);
  expect("cancels the hold on release", {});

  press(runtime, "I", true);
  wait(400);
  expect("sends the hold while the key is held", {key("K", "down"), key("K", "up")});
  press(runtime, "I", false);
  expect("doesn't tap after a hold", {});

  press(runtime, "I", true);
  tap(runtime, "Z");
  wait(400);
  press(runtime, "I", false);
  expect("cancels the hold when another key is pressed",
         {key("Z", "down"), key("Z", "up")});

  press(runtime, "I", true);
  wait(50);
  press(runtime, "I", false);
  wait(200);
  press(runtime, "I", true);
  wait(150);
  expect("restarts the hold time on each press", {key("J", "down"), key("J", "up")});
  press(runtime, "I", false);
  posted.clear();

  tap(runtime, "F");
  tap(runtime, "F");
  expect("posts clicks, counting double clicks",
         {"click:left:down:1", "click:left:up:1", "click:left:down:2",
          "click:left:up:2"});

  tap(runtime, "Caps");
  tap(runtime, "A");
  tap(runtime, "Caps");
  check("toggles Caps Lock", capslockStates == std::vector<bool>{true, false});
  expect("adds Caps Lock to the keys", {key("B", "down+caps"), key("B", "up+caps")});

  tap(runtime, "G");
  expect("checks the app name", {key("G", "down"), key("G", "up")});
  runtime.setAppName("com.test.app");
  tap(runtime, "G");
  expect("uses the app name set later", {key("H", "down"), key("H", "up")});

  std::string log;
  runtime.startLogging([&](std::string line) { log += line; });
  tap(runtime, "A");
  runtime.stopLogging();
  check("logs the key events",
        log.find("Keyboard (productId:vendorId): 2:1") != std::string::npos &&
            log.find("Keys: ") != std::string::npos);
  posted.clear();

  runtime.stop();
  tap(runtime, "A");
  runtime.setAppName("com.other.app");
  runtime.startLogging([](std::string) {});
  expect("ignores everything while stopped", {});

  for (auto& err : errors) Helpers::print("  error: " + err);
  check("reports no errors", errors.empty());

  runtime.start(config, symbolsJson, 0, "");
  auto startResult = runtime.start(config, symbolsJson, 99, "");
  check("reports an invalid profile index",
        startResult == StartResultReportedError && errors.size() == 1 &&
            errors.back() == "StartError: Profile 100 not found");
  tap(runtime, "A");
  expect("stays stopped after a failed start", {});

  startResult = runtime.start("{\"profiles\": {}}", symbolsJson, 0, "");
  check("reports a config without profiles",
        startResult == StartResultReportedError &&
            errors.back() == "StartError: The config has no \"profiles\" array");

  startResult = runtime.start(
      R"({"profiles": [{"rules": [{"keys": ["A"], "send": "NoExist"}]}]})",
      symbolsJson, 0, "");
  check("reports an invalid rule when it starts",
        startResult == StartResultReportedError &&
            errors.back().find("unknown key \"NoExist\"") != std::string::npos);

  // Comments and imports are resolved before the runtime gets the config
  auto dir = std::filesystem::temp_directory_path() / "keyremapper-tests";
  std::filesystem::create_directories(dir);
  std::ofstream(dir / "config.json")
      << "{\n  // comment\n  \"profiles\": \"%import(profiles.json)\"\n}";
  std::ofstream(dir / "profiles.json") << "[{ \"name\": \"Imported\" }]";
  check("resolves the comments and imports of the config",
        nlohmann::json::parse(Config::resolve(dir / "config.json")) ==
            nlohmann::json::parse("{\"profiles\": [{\"name\": \"Imported\"}]}"));

  std::ofstream(dir / "profiles.json") << "[{";
  std::string resolveError;
  try {
    Config::resolve(dir / "config.json");
  } catch (const std::exception& err) {
    resolveError = err.what();
  }
  check("reports invalid JSON in the imported files",
        resolveError.find("isn't valid JSON") != std::string::npos);

  Helpers::print(failures ? std::to_string(failures) + " FAILED" : "SUCCESS!");
  return failures ? 1 : 0;
}
