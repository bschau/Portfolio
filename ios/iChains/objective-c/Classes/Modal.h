//
//  Modal.h
//  iChains
//
//  Created by Brian Schau on 8/11/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FontMgr.h"
#import "GLView.h"
#import "GfxMgr.h"
#import "HiScoreMgr.h"
#import "SoundMgr.h"
#import "Utility.h"
#include "Constants.h"

@interface Modal : NSObject {}

+ (void) initModal:(int) type andNextState:(int) state;
+ (int) run:(GLView *) glView;

@end
