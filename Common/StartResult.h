#pragma once

#import <Foundation/Foundation.h>

// What the daemon replies when it's asked to start remapping
typedef NS_ENUM(NSInteger, StartResult) {
  StartResultOk = 0,
  StartResultNoAccessibility = 1,
  StartResultEventTapFailed = 2,
  // Sent to onError (eg. invalid config)
  StartResultReportedError = 3,
};
