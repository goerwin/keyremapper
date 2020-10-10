/*//
//  main.cpp
//  KeyRemappertest
//
//  Created by Erwin GO on 10/4/20.
//  Copyright © 2020 Erwin GO. All rights reserved.
//
#include "KeyDispatcher.hpp"
#include "Helpers.hpp"
#include <fstream>
#include <sstream>

#include <sstream>
#include <iostream>

int main(int argc, const char * argv[]) {
    
    
    // insert code here...
    std::string path = std::filesystem::current_path().concat("/mode1.json");
    
    std::ifstream file(path);
    std::string fileStr((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
    std::cout << "Hello, World!\n";
    
    std::cout << fileStr;
    return 0;
}
*/
#include "KeyDispatcher.hpp"

#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

KeyDispatcher *keyDispatcher;

void initializeKeyDispatcher() {
  auto rules = Helpers::getJsonFile("mode1.json");
  auto symbols = Helpers::getJsonFile("symbols.json");
  keyDispatcher = new KeyDispatcher(rules, symbols);
    //auto testResults = keyDispatcher->runTests();
    //Helpers::print(!testResults.is_null() ? testResults["message"]
                                                // : "NO TESTS RUN");
}

bool eventHandled = false;
// https://stackoverflow.com/questions/51662245/mac-creating-keyboard-events-causes-memory-leaks
CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStatePrivate);

CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    //if (type != kCGEventKeyDown && type != kCGEventFlagsChanged && type != kCGEventKeyUp) { return event; }
        
    // Paranoid sanity check.
    if ((type != kCGEventKeyDown) && (type != kCGEventKeyUp) && (type != kCGEventFlagsChanged) && (type !=
    NX_SYSDEFINED))
      return event;
        
    if (CGEventGetIntegerValueField(event, kCGEventSourceStateID) == CGEventSourceGetSourceStateID(eventSource)) {
        return event;
    }
    
    // Print the human readable key to the logfile.
    unsigned short code = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    unsigned short state = type == kCGEventKeyDown ? 0 : 1;
    
    std::cout << std::to_string(code) + ":::";

    if (code == 54 || code == 55) // Left/Right Commands
        state = CGEventGetFlags(event) & kCGEventFlagMaskCommand ? 0 : 1;
    else if (code == 57) // Caps
        state = CGEventGetFlags(event) & kCGEventFlagMaskAlphaShift ? 0 : 1;
    
    CGEventTapLocation loc = kCGHIDEventTap;
    
    auto newKeys = keyDispatcher->applyKeys({{code, state}});
    auto newKeysSize = newKeys.size();
    
     for (size_t i = 0; i < newKeysSize; i++) {
       auto [code, state] = newKeys[i];
         
         auto newEvent = CGEventCreateKeyboardEvent (eventSource, (CGKeyCode)code, state == 0);

         
            // TODO: this guy is firing a new system event and it is captured here again,
         // causing an infinite loop. should look for a way to filter out my events
         CGEventPost(loc, newEvent);
         CFRelease(newEvent);
         //CFRelease(eventSource);
       /*auto newKeyStroke = InterceptionKeyStroke({code, state});

       
         interception_send(context, device, (InterceptionStroke *)&newKeyStroke,
                           1);*/
     }


    return nullptr;
}

int main(int argc, const char * argv[]) {
    initializeKeyDispatcher();
    
    // Create an event tap to retrieve keypresses.
    CGEventMask eventMask = (CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventFlagsChanged) | CGEventMaskBit(kCGEventKeyUp));
    CFMachPortRef eventTap = CGEventTapCreate(
        kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, eventMask, CGEventCallback, NULL
    );
    
    // Create a run loop source and add enable the event tap.
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    
    // Add to the current run loop.
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    
    // Enable the event tap.
    CGEventTapEnable(eventTap, true);
    
    CFRunLoopRun();
}
