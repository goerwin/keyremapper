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

@implementation AppBridge
- (int)start:(NSString*)configPath
    withSymbolsPath:(NSString*)symbolsPath
     withProfileIdx:(int)profileIdx
        withAppName:(NSString*)appName {
  Runtime::onError = [](std::string err) {
    [GlobalSwift notifyError:toNSString(err)];
  };

  return Runtime::start([configPath UTF8String], [symbolsPath UTF8String],
                        profileIdx, [appName UTF8String]);
}

- (void)stop {
  Runtime::stop();
}

- (void)startLogging {
  Runtime::startLogging([](std::string log) {
    [GlobalSwift sendKeyEventLogsToClient:toNSString(log)];
  });
}

- (void)stopLogging {
  Runtime::stopLogging();
}

- (void)setAppName:(NSString*)appName {
  Global::keyRemapper->setAppName([appName UTF8String]);
}
@end
