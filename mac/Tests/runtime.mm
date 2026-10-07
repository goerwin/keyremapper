// Runtime tests: feeds key events to the Runtime and checks the events it
// posts, without seizing the keyboards or posting anything to the OS.
// Usage: runtime-tests <runtime.json> <symbols.json>

#import "AppKit/AppKit.h"

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

std::string key(std::string name, std::string suffix) {
  return "key:" + std::to_string(vkCode(name)) + ":" + suffix;
}

int main(int argc, const char* argv[]) {
  if (argc < 3) {
    Helpers::print("Usage: runtime-tests <runtime.json> <symbols.json>");
    return 2;
  }

  symbols = Helpers::getJsonFile(argv[2]);
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

  check("loads the profile", runtime.load(argv[1], argv[2], 0, "") == 0);

  tap(runtime, "A");
  expect("remaps", {key("B", "down"), key("B", "up")});

  tap(runtime, "Z");
  expect("passes unmapped keys through", {key("Z", "down"), key("Z", "up")});

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
  runtime.load(argv[1], argv[2], 0, "");

  auto start = std::chrono::steady_clock::now();
  tap(runtime, "D");
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::steady_clock::now() - start)
                     .count();
  expect("sends keys around SK:Delay",
         {key("Tab", "down"), key("Tab", "up"), key("Tab", "down"),
          key("Tab", "up")});
  check("waits for SK:Delay", elapsed >= 60);

  tap(runtime, "E");
  expect("posts media keys down and up",
         {"media:" + std::to_string(NX_KEYTYPE_SOUND_UP) + ":down",
          "media:" + std::to_string(NX_KEYTYPE_SOUND_UP) + ":up"});

  // Wide margins, since timers fire late on slow machines (CI)
  press(runtime, "I", true);
  wait(50);
  expect("waits for ifHeldFor", {});
  press(runtime, "I", false);
  expect("taps when released before ifHeldFor", {key("J", "down"), key("J", "up")});
  wait(300);
  expect("cancels the hold on release", {});

  // Resets the tap count, so the release after the hold would be a single tap
  tap(runtime, "Z");
  posted.clear();
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
  runtime.start(argv[1], argv[2], 0, "");
  auto startResult = runtime.start(argv[1], argv[2], 99, "");
  check("reports an invalid profile index",
        startResult == 3 && errors.size() == 1);
  tap(runtime, "A");
  expect("stays stopped after a failed start", {});

  Helpers::print(failures ? std::to_string(failures) + " FAILED" : "SUCCESS!");
  return failures ? 1 : 0;
}
