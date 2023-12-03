//
//  iChainsAppDelegate.h
//  iChains
//
//  Created by Brian Schau on 07/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "FontMgr.h"
#import "GameMgr.h"
#import "GfxMgr.h"
#import "GLView.h"
#import "HiScoreMgr.h"
#import "IntroMgr.h"
#import "Modal.h"
#import "NumberFormatter.h"
#import "PrefsMgr.h"
#import "StarsMgr.h"
#import "SoundMgr.h"
#include "Constants.h"

@interface iChainsAppDelegate : NSObject <UIApplicationDelegate, UIAlertViewDelegate> {
    UIWindow *window;
	UIImageView *splashView;
	BOOL gotMemoryWarning;
	BOOL splashContinue;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end
