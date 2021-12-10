//
//  Application.mm
//  KeyRemapperObjectiveC
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

    if (self.activeApplicationChangeCb)
      self.activeApplicationChangeCb(std::string([bundleIdentifier UTF8String]));
  }
@end
