/**
 * OpenGLContextManager.cpp
 *
 * Created by David Cox on Thu Dec 05 2002.
 * Copyright (c) 2002 MIT. All rights reserved.
 */

#include "OpenGLContextManager.h"
#include "StandardVariables.h"
#include "Experiment.h"
#include "Utilities.h"
#include "Event.h"
#include "ComponentRegistry.h"


//******************************************************************
//******************************************************************
//                      Using Cocoa OpenGL
//******************************************************************
// *****************************************************************


//#include "GLWindow.h"
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <OpenGL/glext.h>
#import <IOKit/graphics/IOGraphicsTypes.h>
#import "StateSystem.h"


@interface MWKOpenGLView : NSOpenGLView
@end


@implementation MWKOpenGLView


- (void)update
{
    // This method is called by the windowing system on the main thread.  Since drawing normally occurs
    // on a non-main thread, we need to acquire the context lock before updating the context.
    mw::OpenGLContextLock ctxLock(static_cast<CGLContextObj>([[self openGLContext] CGLContextObj]));
    [super update];
}


@end


BEGIN_NAMESPACE_MW


//#define kDefaultDisplay 1


OpenGLContextLock::OpenGLContextLock(CGLContextObj contextObj) :
    contextObj(contextObj)
{
    if (contextObj) {
        CGLError error = CGLLockContext(contextObj);
        if (kCGLNoError != error) {
            merror(M_DISPLAY_MESSAGE_DOMAIN, "Unable to lock OpenGL context (error = %d)", error);
        }
    }
}


void OpenGLContextLock::unlock(bool clearCurrent) {
    if (contextObj) {
        CGLError error = CGLUnlockContext(contextObj);
        if (kCGLNoError != error) {
            merror(M_DISPLAY_MESSAGE_DOMAIN, "Unable to unlock OpenGL context (error = %d)", error);
        }
        if (clearCurrent) {
            [NSOpenGLContext clearCurrentContext];
        }
    }
}


OpenGLContextManager::OpenGLContextManager() {
	
	mirror_window_active = NO;
    fullscreen_window_active = NO;
    
    mirror_window = nil;
    mirror_view = nil;
    fullscreen_window = nil;
    fullscreen_view = nil;
    
    display_sleep_block = kIOPMNullAssertionID;
    
    contexts = [[NSMutableArray alloc] init];
    
    glew_initialized = false;
    
    main_display_index = -1;
}


int OpenGLContextManager::getNMonitors() {
	NSArray *screens = [NSScreen screens];
    if(screens != nil){
        return [screens count];
    } else {
        return 0;
    }
}

NSScreen *OpenGLContextManager::_getScreen(const int index){
    NSArray *screens = [NSScreen screens];
    
    if(index < 0 || index > [screens count]){
        // TODO: better exception
        throw SimpleException("Attempt to query an invalid screen");
    }
    
    return [screens objectAtIndex: index];
}

NSRect OpenGLContextManager::getDisplayFrame(const int index){
    NSScreen *screen = _getScreen(index);
    NSRect frame = [screen frame];
    
    return frame;
}

int OpenGLContextManager::getDisplayWidth(const int index) {

    NSRect frame = getDisplayFrame(index);
    return frame.size.width;

}

int OpenGLContextManager::getDisplayHeight(const int index) {
    NSRect frame = getDisplayFrame(index);
    return frame.size.height;
}

double OpenGLContextManager::getDisplayRefreshRate(const int index){
    std::map<int, double>::iterator rate = display_refresh_rates.find(index);
    
    if (rate == display_refresh_rates.end()) {
        return 0.0;
    }
    
    double refresh_rate = (*rate).second;
        
    return refresh_rate;
}

CGDirectDisplayID OpenGLContextManager::_getDisplayID(int screen_number) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	NSDictionary *device_description = [_getScreen(screen_number) deviceDescription];
    CGDirectDisplayID display_id = [[device_description objectForKey:@"NSScreenNumber"] intValue];

    [pool drain];
    
    return display_id;
}

void OpenGLContextManager::_measureDisplayRefreshRate(const int index)
{
    CGDirectDisplayID display_id = _getDisplayID(index);
    CGDisplayModeRef mode = CGDisplayCopyDisplayMode(display_id);
    display_refresh_rates[index] = CGDisplayModeGetRefreshRate(mode);
}

CGDirectDisplayID OpenGLContextManager::getMainDisplayID() {
    return _getDisplayID(main_display_index);
}

CVReturn OpenGLContextManager::prepareDisplayLinkForContext(CVDisplayLinkRef displayLink, int context_id) {
    NSOpenGLContext *ctx = [contexts objectAtIndex:context_id];
    CGLContextObj cglContext = (CGLContextObj)[ctx CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[(NSOpenGLView *)[ctx view] pixelFormat] CGLPixelFormatObj];
    return CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
}


int OpenGLContextManager::newMirrorContext(){
    
    // Determine the width and height of the mirror window

    double width = 100.0;    // conspicuously wrong defaults
	double height = 100.0;
	
    shared_ptr<ComponentRegistry> reg = ComponentRegistry::getSharedRegistry();
    shared_ptr<Variable> main_screen_info = reg->getVariable(MAIN_SCREEN_INFO_TAGNAME);
    
	if(main_screen_info != NULL){
        Datum info = *main_screen_info;
		
		if(info.hasKey(M_DISPLAY_WIDTH_KEY) 
		   && info.hasKey(M_DISPLAY_HEIGHT_KEY)
		   && info.hasKey(M_MIRROR_WINDOW_BASE_HEIGHT_KEY)){
			
			double display_width, display_height, display_aspect;
			display_width = info.getElement(M_DISPLAY_WIDTH_KEY);
			display_height = info.getElement(M_DISPLAY_HEIGHT_KEY);
			display_aspect = display_width / display_height;
			height = (double)info.getElement(M_MIRROR_WINDOW_BASE_HEIGHT_KEY);
			width = height * display_aspect;
		}
	}
    
    
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        0
    };
    
    NSOpenGLPixelFormat* pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

    NSOpenGLContext *opengl_context = [[NSOpenGLContext alloc] initWithFormat:pixel_format
                                                                 shareContext:[fullscreen_view openGLContext]];
    if (!opengl_context) {
        throw SimpleException(M_SERVER_MESSAGE_DOMAIN, "Failed to create OpenGL context for mirror window");
    }
    
    GLint swap_int = 1;
    [opengl_context setValues: &swap_int forParameter: NSOpenGLCPSwapInterval];
    
    // NOTE: As of OS X 10.11, performing window and view operations from a non-main thread causes issues
    dispatch_sync(dispatch_get_main_queue(), ^{
        NSRect mirror_rect = NSMakeRect(50.0, 50.0, width, height);
        mirror_window = [[NSWindow alloc] initWithContentRect:mirror_rect
                                                    styleMask:(NSTitledWindowMask | NSMiniaturizableWindowMask)
                                                      backing:NSBackingStoreBuffered
                                                        defer:NO];
        
        NSRect view_rect = NSMakeRect(0.0, 0.0, mirror_rect.size.width, mirror_rect.size.height);
        mirror_view = [[MWKOpenGLView alloc] initWithFrame:view_rect pixelFormat:pixel_format];
        [mirror_window setContentView:mirror_view];
        [mirror_view setOpenGLContext:opengl_context];
        [opengl_context setView:mirror_view];
        
        [mirror_window makeKeyAndOrderFront:nil];
    });
    
    [contexts addObject:opengl_context];
    int context_id = [contexts count] - 1;
    
    [opengl_context release];
    [pixel_format release];
    
    _measureDisplayRefreshRate(0);
    
    
    OpenGLContextLock ctxLock = setCurrent(context_id);
    _initGlew();
        
    return context_id;
    
}


int OpenGLContextManager::newFullscreenContext(int screen_number){
    NSScreen *screen = _getScreen(screen_number);
    
        
    NSRect screen_rect = [screen frame];
//    NSLog(@"screen_rect: %g w, %g h, %g x, %g y", screen_rect.size.width,
//          screen_rect.size.height,
//          screen_rect.origin.x,
//          screen_rect.origin.y);
    
    // for some reason, some displays have random values here...
    screen_rect.origin.x = 0.0;
    screen_rect.origin.y = 0.0;
    
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        0
    };
    
    NSOpenGLPixelFormat* pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    
    NSOpenGLContext *opengl_context = [[NSOpenGLContext alloc] initWithFormat:pixel_format shareContext:nil];
    
    GLint swap_int = 1;
    [opengl_context setValues: &swap_int forParameter: NSOpenGLCPSwapInterval];
    
    
    // NOTE: As of OS X 10.11, performing window and view operations from a non-main thread causes issues
    dispatch_sync(dispatch_get_main_queue(), ^{
        fullscreen_window = [[NSWindow alloc] initWithContentRect:screen_rect
                                                        styleMask:NSBorderlessWindowMask
                                                          backing:NSBackingStoreBuffered
                                                            defer:NO
                                                           screen:screen];
        
        [fullscreen_window setLevel:NSMainMenuWindowLevel+1];
        
        [fullscreen_window setOpaque:YES];
        [fullscreen_window setHidesOnDeactivate:NO];
        
        NSRect view_rect = NSMakeRect(0.0, 0.0, screen_rect.size.width, screen_rect.size.height);
        
        fullscreen_view = [[MWKOpenGLView alloc] initWithFrame:view_rect pixelFormat:pixel_format];
        [fullscreen_window setContentView:fullscreen_view];
        [fullscreen_view setOpenGLContext:opengl_context];
        [opengl_context setView:fullscreen_view];
        
        [fullscreen_window makeKeyAndOrderFront:nil];
    });
    
    [contexts addObject:opengl_context];
    int context_id = [contexts count] - 1;
    
    [opengl_context release];
    [pixel_format release];
    
    _measureDisplayRefreshRate(screen_number);
    
    OpenGLContextLock ctxLock = setCurrent(context_id);
    _initGlew();
    
    if (kIOPMNullAssertionID == display_sleep_block) {
        if (kIOReturnSuccess != IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                                            kIOPMAssertionLevelOn,
                                                            (CFStringRef)@"MWorks Prevent Display Sleep",
                                                            &display_sleep_block)) {
            mwarning(M_SERVER_MESSAGE_DOMAIN, "Cannot disable display sleep");
        }
    }
    
    return context_id;
}


OpenGLContextLock OpenGLContextManager::setCurrent(int context_id) {
    if(context_id < 0 || context_id >= [contexts count]) {
		mwarning(M_SERVER_MESSAGE_DOMAIN, "OpenGL Context Manager: no context to set current.");
		//NSLog(@"OpenGL Context Manager: no context to set current.");
        return OpenGLContextLock();
    }
    
    NSOpenGLContext *ctx = [contexts objectAtIndex:context_id];
    [ctx makeCurrentContext];
    return OpenGLContextLock((CGLContextObj)[ctx CGLContextObj]);
}


void OpenGLContextManager::releaseDisplays() {
  
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    [contexts makeObjectsPerformSelector:@selector(clearDrawable)];
    
    // NOTE: As of OS X 10.11, performing window and view operations from a non-main thread causes issues
    dispatch_sync(dispatch_get_main_queue(), ^{
        mirror_window_active = NO;
        if(mirror_window != nil){
            [mirror_window orderOut:nil];
            [mirror_window release];
            [mirror_view clearGLContext];
            [mirror_view release];
            mirror_window = nil;
            mirror_view = nil;
        }
        
        fullscreen_window_active = NO;
        if(fullscreen_window != nil){
            [fullscreen_window orderOut:nil];
            [fullscreen_window release];
            [fullscreen_view clearGLContext];
            [fullscreen_view release];
            fullscreen_window = nil;
            fullscreen_view = nil;
        }
    });

    if (kIOPMNullAssertionID != display_sleep_block) {
        (void)IOPMAssertionRelease(display_sleep_block);  // Ignore the return code
        display_sleep_block = kIOPMNullAssertionID;
    }
	
    [contexts removeAllObjects];

    [pool drain];

    main_display_index = -1;

}


void OpenGLContextManager::flush(int context_id) {
    if(context_id < 0 || context_id >= [contexts count]){
		mwarning(M_SERVER_MESSAGE_DOMAIN, "OpenGL Context Manager: no context to flush");
        return;
    }
    
    [[contexts objectAtIndex:context_id] flushBuffer];
    
}


SINGLETON_INSTANCE_STATIC_DECLARATION(OpenGLContextManager)


END_NAMESPACE_MW
