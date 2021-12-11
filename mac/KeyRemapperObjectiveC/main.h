//
//  main.h
//  KeyRemapperObjectiveC
//
//  Created by Erwin Gaitan Ospino on 10/12/21.
//

#ifndef main_h
#define main_h

void initializeIOHIDManager();
void initializeKeyDispatcher(int mode = 0);
void myHIDKeyboardCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value);

#endif /* main_h */
