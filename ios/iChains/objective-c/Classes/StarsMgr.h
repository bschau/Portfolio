//
//  StarsMgr.h
//  iChains
//
//  Created by Brian Schau on 07/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>

@interface StarsMgr : NSObject {}

+ (void) setup;
+ (void) animate;
+ (void) draw;

@end