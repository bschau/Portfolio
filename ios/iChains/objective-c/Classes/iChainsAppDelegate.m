//
//  iChainsAppDelegate.m
//  iChains
//
//  Created by Brian Schau on 07/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
//	The "rate" mechanism: http://www.mobileorchard.com/fighting-back-against-the-app-stores-negative-rating-bias/
//
#import "iChainsAppDelegate.h"
#import "UIKit/UIDevice.h"

@interface iChainsAppDelegate (private)
- (void) drawView;
- (void) startupAnimationDone:(NSString *) animationID finished:(NSNumber *) finished context:(void *) context;
@end

@implementation iChainsAppDelegate
#define RENDERFREQ 30.0
@synthesize window;
static NSUserDefaults *defaults;
static GLView *glView;
static NSTimer *animationTimer;
static int state;
static int titleFontW[64] = {
6, 5, 10, 20, 15, 22, 0, 5,
9, 8, 14, 15, 6, 10, 6, 16,
16, 7, 14, 14, 16, 14, 14, 16,
15, 14, 6, 6, 13, 13, 13, 14,
22, 12, 12, 11, 12,	13, 10, 12,
12, 5, 9, 12, 5, 17, 12, 12,
12, 12, 12, 12, 11, 12, 13, 17,
13, 12, 12, 9, 16, 10, 13, 15 };

/** Public (global) variables */
FontInfo titleFont = { @"titlefont.png", 32, 8, 8, (int)' ', &titleFontW[0], nil, 0, nil };
GLfloat opaqueness[11] = { 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };
CGPoint touchPoint;

/**
 * Application entry point.
 *
 * @param application Application info.
 */
- (void) applicationDidFinishLaunching:(UIApplication*) application {
	state = STATEINTROINIT;
	srandom(time(NULL));
	gotMemoryWarning = NO;
	[NumberFormatter open];

	defaults = [NSUserDefaults standardUserDefaults];
	if ([defaults boolForKey:@"rated"] == NO) {
		NSInteger inv = [defaults integerForKey:@"invocations"];
		inv++;
		[defaults setInteger:inv forKey:@"invocations"];
		[defaults synchronize];
		
		if (inv > 10) {
			[[[UIAlertView alloc] initWithTitle:@"Do you like this game?" message:@"Please rate this game in the App Store!" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Rate It!", @"No Thanks", nil] show]; 
		}
	}

	@try {
		[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];

		CGRect rect = [[UIScreen mainScreen] bounds];
	
		window = [[UIWindow alloc] initWithFrame:rect];
	
		glView = [[GLView alloc] initWithFrame:rect];

		[PrefsMgr loadPrefs];
		[HiScoreMgr loadScore];
		[SoundMgr open];
		[SoundMgr startBackgroundMusic:[PrefsMgr isSoundMuted]];
		
		[StarsMgr setup];
		[GameMgr open];
		[glView setupView:&rect];
		[IntroMgr coldBoot];
		
		state = [GameMgr loadState];
		
//#ifdef DEBUG
//		state = STATEGAMESWITCHLEVEL;
//		[GameMgr specialSetup];
//#endif
		splashContinue = NO;
		[window makeKeyAndVisible];
		
		splashView = [[UIImageView alloc] initWithFrame:CGRectMake(0,0, 320, 480)];
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

		[GfxMgr loadIntroSprites];
		[GfxMgr loadPrefsSprites];
		
		[FontMgr loadFont:&titleFont];
		splashContinue = YES;
	}
	@catch (id ex) {
		NSLog(@"%@", ex);
	}
}

/**
 * Handle animation done - free image.
 */
- (void) startupAnimationDone:(NSString *) animationID finished:(NSNumber *) finished context:(void *) context {
	[splashView removeFromSuperview];
	[splashView release];
	
	while (splashContinue == NO) {
		usleep(200000);
	}
	
	[window addSubview:glView];
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0 / RENDERFREQ) target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}

/**
 * Deallocate application.
 */
- (void) dealloc {
	[animationTimer invalidate];
	animationTimer = nil;

	[GameMgr close];
	[GfxMgr releaseAll];
	[FontMgr releaseFont:&titleFont];
	[SoundMgr close];
	[window release];
	[super dealloc];
}

/**
 * Termination callback.
 */
- (void) applicationWillTerminate:(UIApplication *) app {
	[HiScoreMgr saveScore];
	
	switch (state) {
		case STATEGAMERUNNING:
		case STATEGAMELEVELCOMPLETED:
		case STATEGAMELEVELFAILED:
		case STATEGAMELEVELDONE:
		case STATEGAMEMISSIONACCOMPLISHED:
		case STATEGAMESWITCHLEVEL:
			[GameMgr saveState:state];
			break;
	}
}

/**
 * "Rate" mechanism.
 */
- (void) alertView:(UIAlertView *) alertView clickedButtonAtIndex:(NSInteger) buttonIndex {
	if (buttonIndex == 1) {		// Rate It!
		[defaults setBool:YES forKey:@"rated"];
		[defaults synchronize];
		NSURL *url = [NSURL URLWithString:@"http://phobos.apple.com/WebObjects/MZStore.woa/wa/viewSoftware?id=328303743"];
		[[UIApplication sharedApplication] openURL:url];
	} else if (buttonIndex == 2) {	// No, thanks
		[defaults setBool:YES forKey:@"rated"];
		[defaults synchronize];
	}
}

/**
 * Draw the current view.
 */
- (void) drawView {
	[glView prologue];
	
	[StarsMgr animate];
	[StarsMgr draw];

	glDisableClientState(GL_COLOR_ARRAY);

	if (gotMemoryWarning == YES) {
		[HiScoreMgr saveScore];
		
		switch (state) {
			case STATEGAMERUNNING:
			case STATEGAMELEVELCOMPLETED:
			case STATEGAMELEVELFAILED:
			case STATEGAMELEVELDONE:
			case STATEGAMEMISSIONACCOMPLISHED:
			case STATEGAMESWITCHLEVEL:
				[GameMgr saveState:state];
				break;
		}
		
		[Modal initModal:MODALTYPEMEMORYWARNING andNextState:STATEEXIT];
		state = STATEMODALRUN;
		[glView epilogue];
		gotMemoryWarning = NO;
		return;
	}

	switch (state) {
		case STATEINTROCOLDBOOT:
			[IntroMgr coldBoot];
			[IntroMgr initIntro];
			state = STATEINTRORUN;
			break;
			
		case STATEINTROINIT:
			[IntroMgr initIntro];
			[IntroMgr drawHeader];
			state = STATEINTRORUN;
			break;
			
		case STATEINTRORUN:
			state = [IntroMgr run:glView];
			[IntroMgr drawHeader];
			if (state != STATEINTRORUN) {
				[GameMgr fromIntro];
			}
			break;
		
		case STATEGAMERUNNING:
		case STATEGAMENEWLEVEL:
		case STATEGAMELEVELCOMPLETED:
		case STATEGAMELEVELFAILED:
		case STATEGAMELEVELDONE:
		case STATEGAMEMISSIONACCOMPLISHED:
		case STATEGAMESWITCHLEVEL:
			state = [GameMgr run:glView andState:state];
			break;
			
		case STATESUBMITHISCOREINIT:
			if ([HiScoreMgr setupForSubmit] == NO) {
				[Modal initModal:MODALTYPELOWSCORE andNextState:STATEINTROINIT];
				state = STATEMODALRUN;
			} else {
				state = STATESUBMITHISCORESEND;
			}

			[IntroMgr drawHeader];
			break;
			
		case STATESUBMITHISCORESEND:
		case STATESUBMITHISCORERESULT:
			[IntroMgr drawHeader];
			state = [HiScoreMgr submit:state];
			break;
			
		case STATEPREFSINIT:
			[PrefsMgr initPrefs];
			[IntroMgr drawHeader];
			state = STATEPREFSRUN;
			break;
			
		case STATEPREFSRUN:
			state = [PrefsMgr run:glView];
			[IntroMgr drawHeader];
			break;

		case STATEMODALRUN:
			state = [Modal run:glView];
			[IntroMgr drawHeader];
			break;
			
		default:		// eg, STATEEXIT
			[glView epilogue];
			exit(0);
	}

	[glView epilogue];
}

/**
 * What to do when memory is low ...
 */
- (void) applicationDidReceiveMemoryWarning:(UIApplication *) application {
	gotMemoryWarning = YES;
}

@end