//
//  PaddleMgr.h
//	iStar
//
//	Created by Brian Schau on 28/05/09.
//	Copyright 2009 Folera.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>
#import "GfxMgr.h"
#import "Constants.h"
#import "Vector.h"

@interface PaddleMgr : NSObject {
@public
	Vector *sledge;
	Vector *sledgeSize;
	
@private
	GLfloat vertices[12];
	int type;
	int sprIdx;
	Vector *location;
	Vector *size;
	Vector *speed;
}

- (id)initWithX:(GLfloat)x andY:(GLfloat)y andType:(int)t;
- (void)release;
- (void)draw;
- (void)move;

@end