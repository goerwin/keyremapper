// Dev runner: runs the remapping runtime from the terminal, without the app,
// the privileged helper or code signing.
// Usage: sudo keyremapper-dev <config.json> <symbols.json> [--profile N] [--log]

#import "AppKit/AppKit.h"
#import "Foundation/Foundation.h"

#import "../Daemon/Runtime.hpp"

std::string getFrontmostAppName() {
  NSRunningApplication* app = NSWorkspace.sharedWorkspace.frontmostApplication;
  if (app.bundleIdentifier) return [app.bundleIdentifier UTF8String];
  if (app.localizedName) return [app.localizedName UTF8String];
  return "Unknown";
}

void stopOnSignal(int sig) {
  signal(sig, SIG_IGN);
  dispatch_source_t source =
      dispatch_source_create(DISPATCH_SOURCE_TYPE_SIGNAL, sig, 0,
                             dispatch_get_main_queue());
  dispatch_source_set_event_handler(source, ^{
    Runtime::stop();
    Helpers::print("\nStopped");
    exit(0);
  });
  dispatch_resume(source);
  // keep the source alive for the lifetime of the process
  CFBridgingRetain(source);
}

int main(int argc, const char* argv[]) {
  @autoreleasepool {
    if (argc < 3) {
      Helpers::print(
          "Usage: sudo keyremapper-dev <config.json> <symbols.json> "
          "[--profile N] [--log]");
      return 2;
    }

    std::string configPath = argv[1];
    std::string symbolsPath = argv[2];
    int profileIdx = 0;
    bool shouldLog = false;

    for (int i = 3; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == "--log")
        shouldLog = true;
      else if (arg == "--profile" && i + 1 < argc)
        profileIdx = atoi(argv[++i]);
    }

    if (geteuid() != 0) {
      Helpers::print("Error: run it with sudo, seizing keyboards needs root");
      return 1;
    }

    if (system("pgrep -f co.goerwin.KeyRemapperDaemon > /dev/null") == 0) {
      Helpers::print(
          "Error: the KeyRemapper daemon is running, quit KeyRemapper first");
      return 1;
    }

    static bool hasErrors = false;
    Runtime::onError = [](std::string err) {
      hasErrors = true;
      std::cerr << err << std::endl;
    };

    auto startResult =
        Runtime::start(configPath, symbolsPath, profileIdx,
                       getFrontmostAppName());

    // start() reports exceptions through onError and still returns 0
    if (hasErrors) {
      Runtime::stop();
      return 1;
    }

    if (startResult == 1) {
      Helpers::print(
          "Error: Accessibility permission is missing for this terminal app");
      return 1;
    }

    if (startResult != 0) {
      Helpers::print("Error: start failed with code " +
                     std::to_string(startResult));
      return 1;
    }

    if (shouldLog)
      Runtime::startLogging([](std::string log) { Helpers::print(log); });

    [NSWorkspace.sharedWorkspace.notificationCenter
        addObserverForName:NSWorkspaceDidActivateApplicationNotification
                    object:nil
                     queue:nil
                usingBlock:^(NSNotification* notification) {
                  Global::keyRemapper->setAppName(getFrontmostAppName());
                }];

    stopOnSignal(SIGINT);
    stopOnSignal(SIGTERM);
    stopOnSignal(SIGHUP);

    Helpers::print("Running profile " + std::to_string(profileIdx) + " of " +
                   configPath + " (Ctrl+C or close the terminal to stop)");
    CFRunLoopRun();
  }

  return 0;
}
