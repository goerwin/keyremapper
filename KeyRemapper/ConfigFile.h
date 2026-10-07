#pragma once

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface ConfigFile : NSObject
// The config at path with its imports resolved, as JSON
+ (nullable NSString*)resolve:(NSString*)path error:(NSError**)error;
@end

NS_ASSUME_NONNULL_END
