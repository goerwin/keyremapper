// All about XPC
// https://rderik.com/blog/xpc-services-on-macos-apps-using-swift/#the-idea-behind-xpc-and-its-uses

#import "AppKit/AppKit.h"
#import "Foundation/Foundation.h"

#import "./AppBridge.h"
#import "./Runtime.hpp"

#import "KeyRemapperDaemon-Swift.h"

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

- (StartResult)start:(NSString*)config
         withSymbols:(NSString*)symbols
      withProfileIdx:(int)profileIdx {
  return runtime.start([config UTF8String], [symbols UTF8String], profileIdx);
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
@end
