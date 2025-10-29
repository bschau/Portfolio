//
//  AnglesAppDelegate.m
//  Angles
//
//  Created by Brian Schau on 9/30/09.
//  Copyright iPhoneChi 2009. All rights reserved.
//
#import "AnglesAppDelegate.h"
#import "AnglesViewController.h"

@implementation AnglesAppDelegate

@synthesize window;
@synthesize viewController;

/**
 * Application main.
 */
- (void) applicationDidFinishLaunching:(UIApplication *) application {
	srandom(time(NULL));	
	[PrefsMgr loadPrefs];

    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
		
	splashView = [[UIImageView alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 480.0)];
	splashView.image = [UIImage imageNamed:@"Default.png"];
	[window addSubview:splashView];
	[window bringSubviewToFront:splashView];
	[UIView beginAnimations:nil context:nil];
	[UIView setAnimationDuration:0.5];
	[UIView setAnimationTransition:UIViewAnimationTransitionNone forView:window cache:YES];
	[UIView setAnimationDelegate:self];
	[UIView setAnimationDidStopSelector:@selector(startupAnimationDone:finished:context:)];
	splashView.alpha = 0.0;
	splashView.frame = CGRectMake(-60, -60, 440, 600);
	[UIView commitAnimations];
}

/**
 * Handle animation done - free image.
 */
- (void) startupAnimationDone:(NSString *) animationID finished:(NSNumber *) finished context:(void *) context {
	[splashView removeFromSuperview];
	[splashView release];	
}

/**
 * Dealloc resources.
 */
- (void) dealloc {
    [viewController release];
    [window release];
    [super dealloc];
}

/**
 * Terminate callback.
 */
- (void) applicationWillTerminate:(UIApplication *) app {
	[PrefsMgr savePrefs];
}

@end
