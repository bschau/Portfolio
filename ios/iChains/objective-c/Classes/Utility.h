//
//  Utility.h
//  iChains
//
//  Created by Brian Schau on 8/10/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>
#import "FontMgr.h"

@interface Utility : NSObject {}

+ (void) centerLine:(char *) str atY:(GLfloat) y;
+ (NSString *) base64encode:(NSString *) src;

@end
