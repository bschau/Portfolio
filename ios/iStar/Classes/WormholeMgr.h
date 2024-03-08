//
//  WormholeMgr.h
//	iStar
//
//	Created by Brian Schau on 23/05/09.
//	Copyright 2009 Folera.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>
#import "GfxMgr.h"
#import "Constants.h"
#import "Vector.h"

@interface WormholeMgr : NSObject {
	@public
	Vector *location;
	
	@private
	GLfloat vertices[12];
	int animDelay;
	int animIdx;
}

- (id)initWithX:(GLfloat)x andY:(GLfloat)y;
- (void)release;
- (void)draw;
- (void)clearAnimation;
- (void)animate;

@end