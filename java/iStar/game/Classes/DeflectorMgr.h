//
//  DeflectorMgr.h
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

@interface DeflectorMgr : NSObject {
	@public
	Vector *location;
	GLfloat mass;
	GLfloat size;

	@private
	GLfloat vertices[12];
	int type;	
}

- (id)initWithX:(GLfloat)x andY:(GLfloat)y andType:(int)t;
- (void)release;
- (void)draw;

@end