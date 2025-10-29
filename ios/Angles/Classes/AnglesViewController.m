//
//  AnglesViewController.m
//  Angles
//
//  Created by Brian Schau on 9/30/09.
//  Copyright iPhoneChi 2009. All rights reserved.
//
#import "AnglesViewController.h"

@implementation AnglesViewController

@synthesize game;
@synthesize scVal1;
@synthesize scVal2;
@synthesize scVal3;
@synthesize scVal4;
@synthesize scVal5;
@synthesize scName1;
@synthesize scName2;
@synthesize scName3;
@synthesize scName4;
@synthesize scName5;

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

	self.scVal1.text = [PrefsMgr get:SCORE_VALUE1];
	self.scVal2.text = [PrefsMgr get:SCORE_VALUE2];
	self.scVal3.text = [PrefsMgr get:SCORE_VALUE3];
	self.scVal4.text = [PrefsMgr get:SCORE_VALUE4];
	self.scVal5.text = [PrefsMgr get:SCORE_VALUE5];
	
	self.scName1.text = [PrefsMgr get:SCORE_NAME1];
	self.scName2.text = [PrefsMgr get:SCORE_NAME2];
	self.scName3.text = [PrefsMgr get:SCORE_NAME3];
	self.scName4.text = [PrefsMgr get:SCORE_NAME4];
	self.scName5.text = [PrefsMgr get:SCORE_NAME5];
}	

/**
 * Start a new game.
 */
- (IBAction) play {
	[self.game initSettings];
	[self.game initGame];
	[self presentModalViewController:self.game animated:YES];
}

@end
