//
//  GameMgr.h
//  iChains
//
//  Created by Brian Schau on 07/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import "FontMgr.h"
#import "GLView.h"
#import "HiScoreMgr.h"
#import "NumberFormatter.h"
#import "SoundMgr.h"
#import "StateMgr.h"
#import "Utility.h"

@interface GameMgr : NSObject <UIAlertViewDelegate> {}
+ (void) open;
+ (void) close;
+ (void) fromIntro;
+ (void) setup;
+ (int) run:(GLView *) glView andState:(int) state;
+ (void) saveState:(int) state;
+ (int) loadState;
//#ifdef DEBUG
//+ (void) specialSetup;
//#endif

@end