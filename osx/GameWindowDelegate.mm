#include "GameWindowDelegate.h"

@implementation GameWindowDelegate

@synthesize window;

- (void) windowDidMove:(NSNotification *)notification {
#pragma unused(notification)
    [self.window saveFrameUsingName:@"gameWindow"];
}

- (void)windowWillEnterFullScreen:(NSNotification *)notification {
#pragma unused(notification)
    NSLog(@"TODO: pause game time");
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification {
#pragma unused(notification)
    NSLog(@"TODO: resume game time");
}

- (void)windowWillExitFullScreen:(NSNotification *)notification {
#pragma unused(notification)
    NSLog(@"TODO: pause game time");
}

- (void)windowDidExitFullScreen:(NSNotification *)notification {
#pragma unused(notification)
    NSLog(@"TODO: resume game time");
}
@end
