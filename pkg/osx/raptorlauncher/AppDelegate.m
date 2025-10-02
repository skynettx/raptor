//
//  AppDelegate.m
//  raptorlauncher
//
//  Created by skynettx on 17.04.23.
//

#import "AppDelegate.h"

@interface AppDelegate ()


@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

- (BOOL) applicationShouldHandleReopen:(NSApplication *)app hasVisibleWindows:(BOOL)flag {
  if (!flag) {
    NSStoryboard *storyboard = [NSStoryboard storyboardWithName:@"Main" bundle:nil];
    NSWindowController *controllerWindow = [[NSWindowController alloc] init];
    controllerWindow = [storyboard instantiateInitialController];
    [controllerWindow showWindow:self];
  }
  return flag;
}

@end
