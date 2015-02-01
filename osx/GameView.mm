#include "GameView.h"

#include <CoreFoundation/CFBundle.h>
#include <mach/mach_time.h>

@implementation GameView

@synthesize displayLink;
@synthesize lastFrameTime;
@synthesize gameRunner;
@synthesize eventStream;
@synthesize cursorIsCaptured;

static
CVReturn drawFrameCallback(
        CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime,
        CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext) {
#pragma unused(displayLink)
#pragma unused(flagsIn)
#pragma unused(flagsOut)

    GameView* gameView = static_cast<GameView*>(displayLinkContext);
    Keg::GameRunner* gameRunner = gameView.gameRunner;

    if (gameView.lastFrameTime == 0)
        gameView.lastFrameTime = now->hostTime;
    float elapsedTime = static_cast<float>(outputTime->hostTime - gameView.lastFrameTime)/NSEC_PER_SEC;

    CGLContextObj context = static_cast<CGLContextObj>([[gameView openGLContext] CGLContextObj]);
    CGLSetCurrentContext(context);
    CGLLockContext(context);

    gameRunner->drawFrame(
            static_cast<float>(gameView.frame.size.width),
            static_cast<float>(gameView.frame.size.height),
            elapsedTime);

    gameView.lastFrameTime = outputTime->hostTime;

    // This should probably only be checked in dev mode
    GLint gpuVertex, gpuFragment;
    CGLGetParameter(context, kCGLCPGPUVertexProcessing, &gpuVertex);
    CGLGetParameter(context, kCGLCPGPUFragmentProcessing, &gpuFragment);
    if (gpuVertex == 0 || gpuFragment == 0)
        NSLog(@"Rendering not entirely on GPU: vertex: %i, fragment: %i", gpuVertex, gpuFragment);

    CGLFlushDrawable(context);
    CGLUnlockContext(context);

    [gameView setCursorCapture:gameRunner->inputConfig.captureCursor];
    gameRunner->signalRedraw();

    return kCVReturnSuccess;
}

/*
TODO: properly integrate this with a menu-gui for vsync-off implementation
static
void* renderThreaded(void* data) {
    GameView* gameView = static_cast<GameView*>(data);
    CGLContextObj context = static_cast<CGLContextObj>([[gameView openGLContext] CGLContextObj]);
    uint64_t lastFrameTime = mach_absolute_time();
    CGLSetCurrentContext(context);
    GLint swapInterval = 0;
    CGLSetParameter(context, kCGLCPSwapInterval, &swapInterval);
    while (lastFrameTime != 0) {
        CGLLockContext(context);
        uint64_t frameTime = mach_absolute_time();
        float elapsedTime = static_cast<float>(frameTime - lastFrameTime)/NSEC_PER_SEC;
        gameView.gameRunner->drawFrame(
                    static_cast<float>(gameView.frame.size.width),
                    static_cast<float>(gameView.frame.size.height),
                    elapsedTime);
        CGLFlushDrawable(context);
        CGLUnlockContext(context);
        lastFrameTime = frameTime;
    }
    return nullptr;
}
*/

static
void fsEventCallback(
    ConstFSEventStreamRef streamRef,
    void *clientCallBackInfo,
    size_t numEvents,
    void *rawEventPaths,
    const FSEventStreamEventFlags eventFlags[],
    const FSEventStreamEventId eventIds[]) {
#pragma unused(streamRef)
#pragma unused(eventIds)
    GameView* gameView = static_cast<GameView*>(clientCallBackInfo);
    char** eventPaths = static_cast<char**>(rawEventPaths);
    for (size_t i=0; i<numEvents; ++i) {
        if (eventFlags[i] & kFSEventStreamEventFlagItemIsFile) {
            unsigned int mask = kFSEventStreamEventFlagItemCreated | kFSEventStreamEventFlagItemRenamed | kFSEventStreamEventFlagItemModified | kFSEventStreamEventFlagItemRemoved;
            if (eventFlags[i] & mask)
                gameView.gameRunner->handleFileModification(eventPaths[i]);
        }
    }
}


static
NSString* getAbsolutePath(NSString* relativePath) {
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef executableURL = CFBundleCopyExecutableURL(bundle);
    NSString const* executablePath = reinterpret_cast<NSString const *>(CFURLCopyFileSystemPath(executableURL, kCFURLPOSIXPathStyle));
    NSString* executableDir = [executablePath stringByDeletingLastPathComponent];
    NSString* absolutePath = [executableDir stringByAppendingPathComponent: relativePath];
    NSString* cleanAbsolutePath = [absolutePath stringByStandardizingPath];
    [absolutePath release];
    [executableDir release];
    [executablePath release];
    CFRelease(executableURL);
    return cleanAbsolutePath;
}


- (id) init {
    NSOpenGLPixelFormatAttribute attributes[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFANoRecovery,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAStencilSize, 8,
        NSOpenGLPFAMultisample,
        NSOpenGLPFASamples, 8,
        NSOpenGLPFASampleBuffers, 1,
        0
    };
    NSString* libraryPath = getAbsolutePath(@"libgame.dylib");
    NSString* assetsPath = getAbsolutePath(@"../../../../data");
    self.gameRunner = new Keg::GameRunner(libraryPath.UTF8String, assetsPath.UTF8String);
    self.gameRunner->allocateMemory();
    [libraryPath release];
    [assetsPath release];
    return [super
        initWithFrame:NSMakeRect(0, 0, 0, 0)
        pixelFormat:
            [[[NSOpenGLPixelFormat alloc]
            initWithAttributes: attributes] autorelease]];
}

- (void) prepareOpenGL {

    GLint swapInterval = 2;
    [[self openGLContext] setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
    CVDisplayLinkRef newDisplayLink;
    CVDisplayLinkCreateWithActiveCGDisplays(&newDisplayLink);
    CVDisplayLinkSetOutputCallback(newDisplayLink, &drawFrameCallback, self);
    CGLContextObj cglContext = static_cast<CGLContextObj>([[self openGLContext] CGLContextObj]);
    CGLPixelFormatObj cglPixelFormat = static_cast<CGLPixelFormatObj>([[self pixelFormat] CGLPixelFormatObj]);
    CGLSetParameter(cglContext, kCGLCPSwapInterval, &swapInterval);
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(newDisplayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(newDisplayLink);
    self.displayLink = newDisplayLink;

    //pthread_t renderThread;
    //pthread_create(&renderThread, nullptr, renderThreaded, self);

    self.gameRunner->startAssetLoader();
    self.gameRunner->reloadLibrary();

    NSString const* pathsToWatch[2];
    pathsToWatch[0] = getAbsolutePath(@"");
    pathsToWatch[1] = getAbsolutePath(@"../../../../data");
    CFArrayRef pathsToWatchArray = CFArrayCreate(NULL, reinterpret_cast<void const**>(&pathsToWatch[0]), 2, NULL);
    FSEventStreamContext streamContext = {0, self, NULL, NULL, NULL};

    self.eventStream = FSEventStreamCreate(NULL,
        &fsEventCallback,
        &streamContext,
        pathsToWatchArray,
        kFSEventStreamEventIdSinceNow,
        0.0f,
        kFSEventStreamCreateFlagFileEvents);
    CFRelease(pathsToWatchArray);
    [pathsToWatch[0] autorelease];
    [pathsToWatch[1] autorelease];
    FSEventStreamScheduleWithRunLoop(self.eventStream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    FSEventStreamStart(self.eventStream);
}

- (BOOL) acceptsFirstResponder {
    return YES;
}

- (void) mouseMoved:(NSEvent *) event {
    NSPoint locationInWindow = [event locationInWindow];
    NSPoint locationInView = [self convertPoint:locationInWindow fromView:nil];

    self.gameRunner->lockInputState();
    auto& inputState = self.gameRunner->writableInputState;
    inputState.mouse.position = {float(locationInView.x), float(locationInView.y)};
    inputState.mouse.positionDelta = {float(event.deltaX), float(event.deltaY)};
    self.gameRunner->unlockInputState();
}

- (void) mouseDown:(NSEvent *) event {
#pragma unused(event)
    self.gameRunner->lockInputState();
    auto& inputState = self.gameRunner->writableInputState;
    inputState.mouse.leftButton.isDown = true;
    inputState.mouse.leftButton.wasPressed = true;
    self.gameRunner->unlockInputState();
}

- (void) mouseUp:(NSEvent *) event {
#pragma unused(event)
    self.gameRunner->lockInputState();
    auto& inputState = self.gameRunner->writableInputState;
    inputState.mouse.leftButton.isDown = false;
    inputState.mouse.leftButton.wasReleased = true;
    self.gameRunner->unlockInputState();
}

- (void) rightMouseDown:(NSEvent *) event {
#pragma unused(event)
    self.gameRunner->lockInputState();
    auto& inputState = self.gameRunner->writableInputState;
    inputState.mouse.rightButton.isDown = true;
    inputState.mouse.rightButton.wasPressed = true;
    self.gameRunner->unlockInputState();
}

- (void) rightMouseUp:(NSEvent *) event {
#pragma unused(event)
    self.gameRunner->lockInputState();
    auto& inputState = self.gameRunner->writableInputState;
    inputState.mouse.rightButton.isDown = false;
    inputState.mouse.rightButton.wasReleased = true;
    self.gameRunner->unlockInputState();
}

- (void) flagsChanged: (NSEvent *) event {
    if (event.keyCode == 56) {
        // TODO: if the *right* shift key is pressed while the left is down, the up won't be correctly detected
        self.gameRunner->lockInputState();
        auto& inputState = self.gameRunner->writableInputState;
        if (event.modifierFlags & NSShiftKeyMask) {
            inputState.keyboard.walkKey.isDown = true;
            inputState.keyboard.walkKey.wasPressed = true;
        } else {
            inputState.keyboard.walkKey.isDown = false;
            inputState.keyboard.walkKey.wasReleased = true;
        }
        self.gameRunner->unlockInputState();
    }
}

- (void) keyDown: (NSEvent *) event {
    self.gameRunner->lockInputState();
    auto& inputState = self.gameRunner->writableInputState;
    switch (event.keyCode) {
        case 13: // Z
        case 126: // up arrow
            inputState.keyboard.forwardKey.isDown = true;
            inputState.keyboard.forwardKey.wasPressed = true;
        break;
        case 0: // Q
        case 123: // left arrow
            inputState.keyboard.leftKey.isDown = true;
            inputState.keyboard.leftKey.wasPressed = true;
        break;
        case 1: // S
        case 125: // down arrow
            inputState.keyboard.backKey.isDown = true;
            inputState.keyboard.backKey.wasPressed = true;
        break;
        case 2: // D
        case 124: // right arrow
            inputState.keyboard.rightKey.isDown = true;
            inputState.keyboard.rightKey.wasPressed = true;
        break;
        default:
                NSLog(@"down, keycode: %i", event.keyCode);
        break;
    }
    self.gameRunner->unlockInputState();
}

- (void) keyUp: (NSEvent *) event {
    self.gameRunner->lockInputState();
    auto& inputState = self.gameRunner->writableInputState;
    switch (event.keyCode) {
        case 13: // Z
        case 126: // up arrow
            inputState.keyboard.forwardKey.isDown = false;
            inputState.keyboard.forwardKey.wasReleased = true;
        break;
        case 0: // Q
        case 123: // left arrow
            inputState.keyboard.leftKey.isDown = false;
            inputState.keyboard.leftKey.wasReleased = true;
        break;
        case 1: // S
        case 125: // down arrow
            inputState.keyboard.backKey.isDown = false;
            inputState.keyboard.backKey.wasReleased = true;
        break;
        case 2: // D
        case 124: // right arrow
            inputState.keyboard.rightKey.isDown = false;
            inputState.keyboard.rightKey.wasReleased = true;
        break;
        default:
            NSLog(@"up, keycode: %i", event.keyCode);
        break;
    }
    self.gameRunner->unlockInputState();
}

- (void) scrollWheel:(NSEvent *) event {
    self.gameRunner->lockInputState();
    auto& inputState = self.gameRunner->writableInputState;
    inputState.mouse.scrollDelta = {float(event.scrollingDeltaX), float(event.scrollingDeltaY)};
    self.gameRunner->unlockInputState();
}

- (void) magnifyWithEvent:(NSEvent *) event {
    NSLog(@"%@", event);
}

- (void) setCursorCapture: (BOOL) enableCapture {
    if (self.cursorIsCaptured != enableCapture) {
        //CGEventSourceRef evsrc = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
        //CGEventSourceSetLocalEventsSuppressionInterval(evsrc, 0.0);
        if (enableCapture) {
            NSLog(@"Capturing cursor");
            CGDisplayHideCursor(kCGDirectMainDisplay);
            CGAssociateMouseAndMouseCursorPosition(false);
        } else {
            NSLog(@"Releasing cursor");
            CGDisplayShowCursor(kCGDirectMainDisplay);
            CGAssociateMouseAndMouseCursorPosition(true);
        }
        self.cursorIsCaptured = enableCapture;
    }
}

- (void) reshape {
    self.gameRunner->waitRedraw();
}

- (void) dealloc {
    CVDisplayLinkRelease(self.displayLink);
    self.gameRunner->stopAssetLoader();
    delete self.gameRunner;
    FSEventStreamStop(self.eventStream);
    FSEventStreamRelease(self.eventStream);
    [super dealloc];
}

@end
