#pragma once

#include "../runner/GameRunner.h"

#import <Cocoa/Cocoa.h>
#include <CoreServices/CoreServices.h>

@interface GameView : NSOpenGLView

@property (nonatomic, assign) CVDisplayLinkRef displayLink;
@property (nonatomic, assign) uint64_t lastFrameTime;
@property (nonatomic, assign) Keg::GameRunner* gameRunner;
@property (nonatomic, assign) FSEventStreamRef eventStream;
@property (nonatomic, assign) BOOL cursorIsCaptured;

@end
