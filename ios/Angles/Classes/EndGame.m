//
//  EndGame.m
//  Angles
//
//  Created by Brian Schau on 10/10/09.
//  Copyright 2009 iPhoneGame. All rights reserved.
//

#import "EndGame.h"

@interface EndGame (private)
- (void) didQualifyForLeaderboard;
@end

@implementation EndGame

@synthesize tfYouScored;
@synthesize superViewController;

/**
 * Handle memory warnings.
 */
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

/**
 * If view did unload ...
 */
- (void)viewDidUnload { }

/**
 * Dealloc view.
 */
- (void)dealloc {
    [super dealloc];
}

/**
 * View appeared.
 *
 * @param animated Did it animate?
 */
- (void) viewDidAppear:(BOOL) animated {
	[super viewDidAppear:animated];

	NSString *t = [[NSString alloc] initWithFormat:@"%i %@", score, NSLocalizedString(@"pts", @"")];
	[tfYouScored setText:t];
	[t release];
	
	lbPosition = [PrefsMgr didQualify:score];
	
	if (lbPosition > -1) {
		[self didQualifyForLeaderboard];
	}
}

/**
 * Set points.
 *
 * @param pts Points.
 */
- (void) setPoints:(int) pts {
	score = pts;
}

/**
 * OK/Done pressed.
 */
- (IBAction) done {
	[self dismissModalViewControllerAnimated:YES];
	[superViewController dismissModalViewControllerAnimated:YES];
}

/**
 * If score qualified for the leaderboard get player name.
 */
- (void) didQualifyForLeaderboard {
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"You're in Top 5!", "") message:NSLocalizedString(@"Congratulations!\nYou're in Top 5\n- please enter your name:\n\n\n", "") delegate:self cancelButtonTitle:NSLocalizedString(@"OK", "") otherButtonTitles:nil];
	CGRect frame = CGRectMake(12.0, 125.0, 260.0, 25.0);
	
	if (!tfName) {
		tfName = [[UITextField alloc] initWithFrame:frame];
		tfName.borderStyle = UITextBorderStyleBezel;
		tfName.textColor = [UIColor blackColor];
		tfName.textAlignment = UITextAlignmentCenter;
		tfName.font = [UIFont systemFontOfSize:14.0];
		tfName.backgroundColor = [UIColor whiteColor];
		tfName.autocorrectionType = UITextAutocorrectionTypeNo;
		tfName.keyboardType = UIKeyboardTypeAlphabet;
		tfName.returnKeyType = UIReturnKeyDone;
		tfName.delegate = self;
		tfName.clearButtonMode = UITextFieldViewModeWhileEditing;
	}
	
	tfName.text = nil;
	
	[tfName becomeFirstResponder];
	[alert addSubview:tfName];
	
	CGAffineTransform myTransform = CGAffineTransformMakeTranslation(0.0, 110.0);
	[alert setTransform:myTransform];
	
	[alert show];
	[alert release];
}

/**
 * Handler for alert.
 */
- (void) alertView:(UIAlertView *) av clickedButtonAtIndex:(NSInteger) buttonIndex {
	if (buttonIndex == [av cancelButtonIndex]) {
		if (([tfName text] == nil) || ([[tfName text] length] == 0)) {
			[PrefsMgr updateNameAtPos:@"" withPosition:lbPosition];
		} else {
			[PrefsMgr updateNameAtPos:[tfName text] withPosition:lbPosition];
		}
	}
}

/**
 * Dismiss keyboard.
 */
- (BOOL) textFieldShouldReturn:(UITextField *) theTextField {
	[tfName resignFirstResponder];
	return YES;
}
	
@end
