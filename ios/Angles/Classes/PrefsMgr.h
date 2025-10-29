//
//  PrefsMgr.h
//  Angles
//
//  Created by Brian Schau on 10/5/09.
//  Copyright 2009 iPhoneChi. All rights reserved.
//
#import <Foundation/Foundation.h>
#include "Constants.h"

@interface PrefsMgr : NSObject {}

+ (void) loadPrefs;
+ (void) savePrefs;
+ (NSString *) get:(NSString *) key;
+ (void) set:(NSString *) key andValue:(NSString *)value;
+ (int) didQualify:(int) pts;
+ (void) updateNameAtPos:(NSString *) name withPosition:(int) pos;

@end
