//
//  Application.h
//  KeyRemapperObjectiveC
//
//  Created by Erwin Gaitan Ospino on 9/12/21.
//

#ifndef Application_h
#define Application_h

#include <iostream>

@interface Application:NSObject
  @property std::string activeApplication;
  @property std::function<void(std::string)> activeApplicationChangeCb;

  - (void)handleApplicationChange:(NSNotification*)notification;
@end

#endif /* Application_h */
