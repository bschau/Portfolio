//
//  Vector.h
//  iStar
//
//  Created by Brian Schau on 12/05/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>

@interface Vector : NSObject {
	@public
	GLfloat x;
	GLfloat y;
}

- (Vector *)initWithZero;
- (Vector *)initWithX:(GLfloat)x andY:(GLfloat)y;
- (void)zero;
- (void)setX:(GLfloat)x andY:(GLfloat)y;
- (void)clone:(Vector *)v;
- (void)add:(Vector *)v;
- (void)subtract:(Vector *)v;
- (void)multiply:(GLfloat)v;
- (void)divide:(GLfloat)v;

@end
