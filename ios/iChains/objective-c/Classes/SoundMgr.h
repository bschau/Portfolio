//
//  SoundMgr.h
//  iChains
//
//  Created by Brian Schau on 7/9/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <AVFoundation/AVAudioPlayer.h>
#import <AudioToolbox/AudioToolbox.h>
#import <Foundation/Foundation.h>
#include "Constants.h"

@interface SoundMgr : NSObject <AVAudioPlayerDelegate> {}

+ (void) open;
+ (void) close;
+ (void) startBackgroundMusic:(BOOL) startMuted;
+ (void) unmute;
+ (void) mute;
+ (void) playEffect:(int) name;

@end
