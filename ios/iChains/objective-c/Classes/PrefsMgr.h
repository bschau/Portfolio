//
//  PrefsMgr.h
//  iChains
//
//  Created by Brian Schau on 7/28/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FontMgr.h"
#import "GLView.h"
#import "GfxMgr.h"
#import "HiScoreMgr.h"
#import "Modal.h"
#import "NumberFormatter.h"
#import "SoundMgr.h"
#import "StateMgr.h"
#import "Utility.h"

@interface PrefsMgr : NSObject {}

+ (void) initPrefs;
+ (int) run:(GLView *) glView;
+ (void) loadPrefs;
+ (void) savePrefs;
+ (BOOL) isSoundMuted;

@end