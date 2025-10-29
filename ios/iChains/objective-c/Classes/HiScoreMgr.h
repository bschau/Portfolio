//
//  HiScoreMgr.h
//  iChains
//
//  Created by Brian Schau on 7/15/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <CoreFoundation/CoreFoundation.h>
#import <CommonCrypto/CommonDigest.h>
#import <Foundation/Foundation.h>
#import "FontMgr.h"
#import "NumberFormatter.h"
#import "PrefsMgr.h"
#import "StateMgr.h"
#import "Utility.h"
#include "Constants.h"

@interface HiScoreMgr : NSObject {}

+ (void) loadScore;
+ (void) saveScore;
+ (int) getPosition;
+ (unsigned long) getScore;
+ (BOOL) addScore:(unsigned long) sc;
+ (BOOL) canSubmit;
+ (void) clearSubmittable;
+ (BOOL) setupForSubmit;
+ (BOOL) getHiScoreErr;
+ (int) submit:(int) state;

@end
