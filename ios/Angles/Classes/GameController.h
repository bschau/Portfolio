//
//  GameController.h
//  Angles
//
//  Created by Brian Schau on 9/30/09.
//  Copyright 2009 iPhoneChi. All rights reserved.
//
#import <UIKit/UIKit.h>
#import "EndGame.h"
#import "PrefsMgr.h"

@interface GameController : UIViewController <UITextFieldDelegate> {
	UISwitch *reflexAngles;
	UITextField *angle;
	UITextField *txtRound;
	UITextField *txtScore;
	UIImageView *imgCircle;
	UIButton *btnGuess;
	int round;
	int score;
	int angleToGuess;
	int angleStart;
	int allowReflexAngles;
	int whichAlert;
	BOOL animDone;
	EndGame *endGame;
}

#define AlertInvalidGuess 1
#define AlertOkToReset (AlertInvalidGuess + 1)
#define AlertStupidGuess (AlertOkToReset + 1)

@property (nonatomic, retain) IBOutlet UISwitch *reflexAngles;
@property (nonatomic, retain) IBOutlet UITextField *angle;
@property (nonatomic, retain) IBOutlet UITextField *txtRound;
@property (nonatomic, retain) IBOutlet UITextField *txtScore;
@property (nonatomic, retain) IBOutlet UIImageView *imgCircle;
@property (nonatomic, retain) IBOutlet UIButton *btnGuess;
@property (nonatomic, retain) IBOutlet EndGame *endGame;

- (void) initSettings;
- (void) initGame;
- (IBAction) guess;
- (IBAction) toggleReflexAngles;

@end
