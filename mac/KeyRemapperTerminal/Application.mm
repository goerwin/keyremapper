//
//  Application.m
//
//  Created by Erwin Gaitan Ospino on 9/12/21.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "./Application.h"

@implementation Application
  - (void)handleApplicationChange:(NSNotification*)notif {
    NSRunningApplication *frontmostApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
    NSString *bundleIdentifier = [frontmostApp bundleIdentifier];
    NSString *localizedName = [frontmostApp localizedName];
    std::string appName = std::string(
      bundleIdentifier != nil ?
        [bundleIdentifier UTF8String] :
        localizedName != nil ? [localizedName UTF8String] : "Unknown"
    );
    
    if (self.activeApplicationChangeCb) self.activeApplicationChangeCb(appName);
  }
@end
