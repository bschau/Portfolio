//
//  EndGame.h
//  Angles
//
//  Created by Brian Schau on 10/10/09.
//  Copyright 2009 iPhoneChi. All rights reserved.
//
#import <UIKit/UIKit.h>
#import "PrefsMgr.h"

@interface EndGame : UIViewController <UITextFieldDelegate> {
	UIViewController *superViewController;
	UITextField *tfYouScored;
	UITextField *tfName;
	int score;
	int lbPosition;
}

@property (nonatomic, retain) IBOutlet UITextField *tfYouScored;
@property (nonatomic, retain) IBOutlet UIViewController *superViewController;

- (void)setPoints:(int) pts;
- (IBAction) done;

@end
