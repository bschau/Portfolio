//
//  AnglesViewController.h
//  Angles
//
//  Created by Brian Schau on 9/30/09.
//  Copyright iPhoneChi 2009. All rights reserved.
//
#import <UIKit/UIKit.h>
#import "GameController.h"
#include "Constants.h"

@class GameController;

@interface AnglesViewController : UIViewController {
	GameController *game;
	UITextField *scVal1;
	UITextField *scVal2;
	UITextField *scVal3;
	UITextField *scVal4;
	UITextField *scVal5;
	UITextField *scName1;
	UITextField *scName2;
	UITextField *scName3;
	UITextField *scName4;
	UITextField *scName5;
}

@property (nonatomic, retain) IBOutlet GameController *game;
@property (nonatomic, retain) IBOutlet UITextField *scVal1;
@property (nonatomic, retain) IBOutlet UITextField *scVal2;
@property (nonatomic, retain) IBOutlet UITextField *scVal3;
@property (nonatomic, retain) IBOutlet UITextField *scVal4;
@property (nonatomic, retain) IBOutlet UITextField *scVal5;
@property (nonatomic, retain) IBOutlet UITextField *scName1;
@property (nonatomic, retain) IBOutlet UITextField *scName2;
@property (nonatomic, retain) IBOutlet UITextField *scName3;
@property (nonatomic, retain) IBOutlet UITextField *scName4;
@property (nonatomic, retain) IBOutlet UITextField *scName5;

- (IBAction) play;

@end

