#ifndef AppBridge_h
#define AppBridge_h

#import <Foundation/Foundation.h>

#import "../Common/StartResult.h"

@interface AppBridge : NSObject
- (StartResult)start:(NSString*)config
         withSymbols:(NSString*)symbols
      withProfileIdx:(int)profileIdx;
- (void)stop;
- (void)startLogging;
- (void)stopLogging;
@end

#endif
