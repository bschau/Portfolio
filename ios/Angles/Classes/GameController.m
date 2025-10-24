//
//  GameController.m
//  Angles
//
//  Created by Brian Schau on 9/30/09.
//  Copyright 2009 iPhoneChi. All rights reserved.
//
#import "GameController.h"

@interface GameController (private)
- (void) setRound;
- (void) setupQuiz;
- (void) showFadeOutAnim;
- (void) fadeOutAnimationDone:(NSString *) animationID finished:(NSNumber *) finished context:(void *) context;
@end

@implementation GameController

@synthesize reflexAngles;
@synthesize angle;
@synthesize txtRound;
@synthesize txtScore;
@synthesize imgCircle;
@synthesize btnGuess;
@synthesize endGame;

static const CGFloat rad = 3.14159265 / 180;

/**
 * Release resource if we got a memory warning.
 */
- (void) didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

/**
 * Unload.
 */
- (void) viewDidUnload { }

/**
 * Dealloc.
 */
- (void) dealloc {
    [super dealloc];
}

/**
 * Happens when the view appears.
 *
 * @param animated Did it animate?
 */
- (void) viewDidAppear:(BOOL) animated {
	[super viewDidAppear:animated];

	[self setRound];
	[reflexAngles setOn:(allowReflexAngles ? YES : NO) animated:NO];
}

/**
 * Set round data (score, round, graphics).
 */
- (void) setRound {
	NSString *t = [[NSString alloc] initWithFormat:@"%i / 10", round];
	
	self.txtRound.text = t;
	[t release];
	
	t = [[NSString alloc] initWithFormat:@"%i %@", score, NSLocalizedString(@"pts", @"")];
	self.txtScore.text = t;
	[t release];
	
	CGRect circle = CGRectMake(0.0, 0.0, imgCircle.frame.size.width, imgCircle.frame.size.height);

	UIGraphicsBeginImageContext(imgCircle.frame.size);
    [imgCircle.image drawInRect:circle];

	CGContextRef ctx = UIGraphicsGetCurrentContext();
	CGContextSetShouldAntialias(ctx, true);
	CGContextClearRect(ctx, circle);
	
	CGContextSetRGBFillColor(ctx, 0.7, 0.72, 1.0, 1.0);
	CGContextSetRGBStrokeColor(ctx, 0.7, 0.72, 1.0, 1.0);
	
	CGContextStrokeEllipseInRect(ctx, circle);
	
	CGFloat w = imgCircle.frame.size.width / 2.0;
	CGFloat h = imgCircle.frame.size.height / 2.0;
	CGFloat x, y, v;
	
	CGContextSetRGBFillColor(ctx, 0.4, 0.42, 1.0, 1.0);
	CGContextSetRGBStrokeColor(ctx, 0.4, 0.42, 1.0, 1.0);

	for (int g = 0; g < 360; g += 45) {
		x = (cos(g * rad) * w) + w;
		y = (sin(g * rad) * h) + h;
		CGContextStrokeEllipseInRect(ctx, CGRectMake(x - 1.0, y - 1.0, 1.0, 1.0));
	}

	v = (CGFloat) angleStart;
	for (int g = 0; g < angleToGuess; g++) {
		x = (cos(v * rad) * 50) + w;
		y = (sin(v * rad) * 50) + h;
		CGContextStrokeEllipseInRect(ctx, CGRectMake(x, y, 1.0, 1.0));
		v++;
		if (v > 360.0) {
			v -= 360.0;
		}
	}
	
	CGPoint points[] = { { 0.0, 0.0 }, { 0.0, 0.0 } };
	CGContextBeginPath(ctx);
	points[0].x = w;
	points[0].y = h;
	points[1].x = (cos(angleStart * rad) * w) + w;
	points[1].y = (sin(angleStart * rad) * w) + w;
	CGContextAddLines(ctx, points, 2);

	v = angleStart + angleToGuess;
	if (v > 360.0) {
		v -= 360.0;
	}
	
	points[1].x = (cos(v * rad) * w) + w;
	points[1].y = (sin(v * rad) * w) + w;
	CGContextAddLines(ctx, points, 2);

	CGContextClosePath(ctx);
	CGContextDrawPath(ctx, kCGPathStroke);
    imgCircle.image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
	
}

/**
 * Handle return.
 */
- (BOOL) textFieldShouldReturn:(UITextField *) textField {
	[textField resignFirstResponder];
	return YES;
}

/**
 * What to do when text field editing ends ...
 */
- (void) textFieldDidEndEditing:(UITextField *) textField { }

/**
 * Handle guess.
 */
- (void) guess {
	NSString *g = [angle text];
	
	[self textFieldShouldReturn:angle];
	if (g != nil) {
		g = [g stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];

		if ([g length] > 0) {
			int gV = [g intValue], len = [g length], i;
			
			for (i = 0; i < len; i++) {
				if (([g characterAtIndex:i] < '0') || ([g characterAtIndex:i] > '9')) {
					whichAlert = AlertStupidGuess;
					[[[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Invalid guess!", "") message:NSLocalizedString(@"Please enter a value between 1 and 360.", "") delegate:self cancelButtonTitle:NSLocalizedString(@"OK", "") otherButtonTitles:nil] show];
					return;
				}
			}
			
			if ((gV > 0) && (gV < 361)) {
				if (gV == angleToGuess) {
					score += 500;
				} else {
					score += (360 / abs(angleToGuess - gV));
				}

				round++;
				if (round > 10) {
					[[self endGame] setSuperViewController:[self parentViewController]];
					[[self endGame] setPoints:score];
					[self presentModalViewController:self.endGame animated:YES];
					return;
				}				
				[self showFadeOutAnim];
			}
				
			return;
		}
	}
	
	whichAlert = AlertInvalidGuess;
	[[[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Invalid guess!", "") message:NSLocalizedString(@"Please enter a valid guess.", "") delegate:self cancelButtonTitle:NSLocalizedString(@"OK", "") otherButtonTitles:nil] show];
}

/**
 * Handle alertView.
 *
 * @param alertView Current alert view.
 * @param buttonIndex Index of button clicked.
 */
- (void) alertView:(UIAlertView *) alertView clickedButtonAtIndex:(NSInteger) buttonIndex {
	if (whichAlert == AlertOkToReset) {
		if (buttonIndex == 0) {
			allowReflexAngles = (allowReflexAngles ? 0 : 1);
			[reflexAngles setOn:(allowReflexAngles ? YES : NO) animated:YES];
		} else {
			[PrefsMgr set:REFLEX_ANGLES andValue:(allowReflexAngles ? @"1" : @"0")];
			[self initGame];
			[self setRound];
			[[self view] setNeedsDisplay];
		}
	}
}

/**
 * Setup next question.
 */
- (void) setupQuiz {
	if (allowReflexAngles) {
		angleToGuess = (arc4random() % 350) + 5;
	} else {
		angleToGuess = (arc4random() % 170) + 5;
	}
	
	angleStart = arc4random() % 360;
}

/**
 * Init settings.
 */
- (void) initSettings {
	allowReflexAngles = ([[PrefsMgr get:REFLEX_ANGLES] compare:@"1"] == NSOrderedSame ? 1 : 0);
}

/**
 * Start a new game.
 */
- (void) initGame {
	round = 1;
	score = 0;
	[self setupQuiz];
}	

/**
 * Handle reflex angles change.
 */
- (IBAction) toggleReflexAngles {
	BOOL val = (allowReflexAngles ? YES : NO);
	
	if (reflexAngles.on != val) {
		allowReflexAngles = (allowReflexAngles ? 0 : 1);
		whichAlert = AlertOkToReset;
		[[[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Ok to reset?", "") message:NSLocalizedString(@"Changing the reflex angle setting requires a restart. Is that ok?", "") delegate:self cancelButtonTitle:NSLocalizedString(@"No", "") otherButtonTitles:NSLocalizedString(@"Yes", ""), nil] show];
	}
}

/**
 * Minor fade animation.
 */
- (void) showFadeOutAnim {
	[btnGuess setEnabled:NO];

	[UIView beginAnimations:nil context:nil];
	[UIView setAnimationDuration:0.5];
	[UIView setAnimationTransition:UIViewAnimationTransitionNone forView:imgCircle cache:YES];
	[UIView setAnimationDelegate:self];
	[UIView setAnimationDidStopSelector:@selector(fadeOutAnimationDone:finished:context:)];
	imgCircle.alpha = 0.0;
	[UIView commitAnimations];
}

/**
 * Fade anim handler ...
 */
- (void) fadeOutAnimationDone:(NSString *) animationID finished:(NSNumber *) finished context:(void *) context {
	imgCircle.alpha = 1.0;
	[self setupQuiz];
	[self setRound];
	[btnGuess setEnabled:YES];
}

@end
