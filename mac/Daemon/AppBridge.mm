// All about XPC
// https://rderik.com/blog/xpc-services-on-macos-apps-using-swift/#the-idea-behind-xpc-and-its-uses

#import "AppKit/AppKit.h"
#import "Foundation/Foundation.h"

#import "co_goerwin_KeyRemapperDaemon-Swift.h"

#import "./AppBridge.h"
#import "./Runtime.hpp"

NSString* toNSString(std::string str) {
  return [NSString stringWithUTF8String:str.c_str()];
}

@implementation AppBridge {
  Runtime runtime;
}

- (instancetype)init {
  self = [super init];
  runtime.onError = [](std::string err) {
    [GlobalSwift notifyError:toNSString(err)];
  };
  return self;
}

- (int)start:(NSString*)configPath
    withSymbolsPath:(NSString*)symbolsPath
     withProfileIdx:(int)profileIdx
        withAppName:(NSString*)appName {
  return runtime.start([configPath UTF8String], [symbolsPath UTF8String],
                       profileIdx, [appName UTF8String]);
}

- (void)stop {
  runtime.stop();
}

- (void)startLogging {
  runtime.startLogging([](std::string log) {
    [GlobalSwift sendKeyEventLogsToClient:toNSString(log)];
  });
}

- (void)stopLogging {
  runtime.stopLogging();
}

- (void)setAppName:(NSString*)appName {
  runtime.setAppName([appName UTF8String]);
}
@end
