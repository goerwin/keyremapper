#import "./ConfigFile.h"

#import "../Common/Config.hpp"

@implementation ConfigFile
+ (NSString*)resolve:(NSString*)path error:(NSError**)error {
  try {
    return [NSString stringWithUTF8String:Config::resolve(path.UTF8String).c_str()];
  } catch (const std::exception& err) {
    if (error)
      *error = [NSError
          errorWithDomain:@"ConfigFile"
                     code:0
                 userInfo:@{
                   NSLocalizedDescriptionKey : [NSString stringWithUTF8String:err.what()]
                 }];
    return nil;
  }
}
@end
