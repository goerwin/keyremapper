#ifndef AppBridge_h
#define AppBridge_h

#import <Foundation/Foundation.h>

@interface AppBridge : NSObject
- (int) start:(NSString*)configPath withSymbolsPath:(NSString*)symbolsPath withProfileIdx:(int)profileIdx withAppName:(NSString*)appName;
- (void) stop;
- (void) startLogging;
- (void) stopLogging;
- (void) setAppName:(NSString*)appName;
@end

#endif
