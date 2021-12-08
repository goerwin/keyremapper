//
//  main.h
//  KeyRemappertest
//
//  Created by Erwin Gaitan Ospino on 7/12/21.
//  Copyright © 2021 Erwin GO. All rights reserved.
//

#ifndef main_h
#define main_h

void openIOHIDManager();
void closeIOHIDManager();
bool getCapslockState();
void toggleCapslockState();
void initializeIOHIDManager();

#endif /* main_h */
