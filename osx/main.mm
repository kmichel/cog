#include "GameView.h"
#include "GameWindowDelegate.h"

#import <Cocoa/Cocoa.h>

static
NSMenu *makeAppMenu(NSString *appName) {
    NSMenu *menubar = [[NSMenu new] autorelease];
    NSMenuItem *appMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:appMenuItem];
    NSMenu *appMenu = [[NSMenu new] autorelease];
    [appMenu
        addItemWithTitle:[@"Hide " stringByAppendingString:appName]
        action:@selector(hide:)
        keyEquivalent:@"h"];
    NSMenuItem* hideOthers = [appMenu
        addItemWithTitle:@"Hide Others"
        action:@selector(hideOtherApplications:)
        keyEquivalent:@"h"];
    hideOthers.keyEquivalentModifierMask = NSAlternateKeyMask | NSCommandKeyMask;
    [appMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
    [appMenu
        addItem: [NSMenuItem separatorItem]];
    [appMenu
        addItemWithTitle:[@"Quit " stringByAppendingString:appName]
        action:@selector(terminate:)
        keyEquivalent:@"q"];
  [appMenuItem setSubmenu:appMenu];

    NSMenuItem *viewMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:viewMenuItem];
    NSMenu *viewMenu = [[[NSMenu alloc] initWithTitle:@"View"] autorelease];
    NSMenuItem* enterFullScreen = [viewMenu addItemWithTitle:@"Enter Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"];
    enterFullScreen.keyEquivalentModifierMask = NSControlKeyMask | NSCommandKeyMask;
    [viewMenuItem setSubmenu:viewMenu];


  NSMenuItem *windowMenuItem = [[NSMenuItem new] autorelease];
  [menubar addItem:windowMenuItem];
  NSMenu *windowMenu = [[[NSMenu alloc] initWithTitle:@"Window"] autorelease];
  [windowMenu addItemWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
  [windowMenuItem setSubmenu:windowMenu];

  return menubar;
}


int main() {
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        NSString *appName = [[NSProcessInfo processInfo] processName];
        [NSApp setMainMenu:makeAppMenu(appName)];
        NSWindow *window = [[[NSWindow alloc]
            initWithContentRect:NSMakeRect(0, 0, 640, 480)
            styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
            backing:NSBackingStoreBuffered
            defer:NO] autorelease];
        [window setColorSpace: [NSColorSpace sRGBColorSpace]];
        [window setContentView:[[[GameView alloc] init] autorelease]];
        GameWindowDelegate* gameWindowDelegate = [[[GameWindowDelegate alloc] init] autorelease];
        [window setDelegate:gameWindowDelegate];
        gameWindowDelegate.window = window;
        [window center];
        [window setFrameAutosaveName:@"gameWindow"];
        [window setCollectionBehavior:NSWindowCollectionBehaviorManaged | NSWindowCollectionBehaviorFullScreenPrimary];
        [window setTitle:appName];
        [window setAcceptsMouseMovedEvents:YES];
        [window makeKeyAndOrderFront:nil];
        [[NSNotificationCenter defaultCenter]
            addObserver:NSApp
            selector:@selector(terminate:)
            name:NSWindowWillCloseNotification
            object:nil];
        NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
        if ([userDefaults boolForKey:@"forceActivate"])
            [NSApp activateIgnoringOtherApps:YES];
        if ([userDefaults boolForKey:@"alwaysOnTop"])
            [window setLevel: NSTornOffMenuWindowLevel];
        [NSApp run];
    }
    return 0;
}
