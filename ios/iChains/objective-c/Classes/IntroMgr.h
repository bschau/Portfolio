//
//  IntroMgr.h
//  iChains
//
//  Created by Brian Schau on 21/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import "FontMgr.h"
#import "GLView.h"
#import "GfxMgr.h"
#import "HiScoreMgr.h"
#import "NumberFormatter.h"
#import "SoundMgr.h"
#import "StateMgr.h"
#import "Utility.h"

@interface IntroMgr : NSObject {}

+ (void) coldBoot;
+ (void) initIntro;
+ (int) run:(GLView *) glView;
+ (void) drawHeader;

@end