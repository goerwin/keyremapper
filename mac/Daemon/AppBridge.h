#ifndef AppBridge_h
#define AppBridge_h

#import <Foundation/Foundation.h>

@interface AppBridge : NSObject
- (int) start:(NSString*)configPath withSymbolsPath:(NSString*)symbolsPath withAppName:(NSString*)appName;
- (void) stop;
- (void) setAppName:(NSString*)appName;
@end

#endif