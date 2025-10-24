//
//  iStarAppDelegate.m
//  iStar
//
//  Created by Brian Schau on 05/05/09.
//  Copyright Folera.Com 2009. All rights reserved.
//
#import "iStarAppDelegate.h"

@implementation iStarAppDelegate
@synthesize window;
#define RENDERFREQ 30.0
static GLView *glView;
static NSTimer *animationTimer;

/**
 * Application entry point.
 * 
 * @param application Application Info.
 */
- (void)applicationDidFinishLaunching:(UIApplication*)application {
	srandom(time(NULL));

	@try {
		[PlayerMgr classInit];
		[ThingMgr classInit];
	
		[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
	
		CGRect rect = [[UIScreen mainScreen] bounds];
	
		window = [[UIWindow alloc] initWithFrame:rect];
	
		glView = [[GLView alloc] initWithFrame:rect];
		[window addSubview:glView];

		[GfxMgr loadSprites];
		[StarsMgr setup];
		[glView setupView:&rect];
		[GameMgr setup:1];
	
		animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0 / RENDERFREQ) target:self selector:@selector(drawView) userInfo:nil repeats:YES];

		[window makeKeyAndVisible];
	}
	@catch (id ex) {
		NSLog(@"%@", ex);
	}
}

/**
 * Deallocation application.
 */
- (void)dealloc {
	[animationTimer invalidate];
	animationTimer = nil;

	[ThingMgr reset];
	[GfxMgr releaseAll];
	[glView release];
	[window release];
	[PlayerMgr releaseAll];
	[super dealloc];
}

/**
 * Draw view.
 */
- (void)drawView {
	[glView prologue];
	
	[StarsMgr animate];
	[StarsMgr draw];

	// if (inGame) {
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	[GameMgr run:glView];
	// }
	[glView epilogue];
}
@end
