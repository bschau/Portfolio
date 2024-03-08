//
//  StarsMgr.h
//  iStar
//
//  Created by Brian Schau on 06/05/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>

@interface StarsMgr : NSObject {
}

+ (void)setup;
+ (void)animate;
+ (void)draw;

@end
