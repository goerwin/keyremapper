#ifndef KeyRemapperWrapper_h
#define KeyRemapperWrapper_h

#import <Foundation/Foundation.h>

@interface KeyRemapperWrapper : NSObject
- (KeyRemapperWrapper*)init:(NSString*)dirPath withSymbolsPath:(NSString*)symbolsPath;

- (void)setAppName:(NSString*)appName;
- (void)applyKeyEvent:(int)scancode state:(int)state keyboard:(NSString*)kb keyboardDescription:(NSString*)kbDesc;
@end

#endif
