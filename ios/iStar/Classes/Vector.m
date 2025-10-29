//
//  Vector.m
//  iStar
//
//  Created by Brian Schau on 12/05/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import "Vector.h"

@implementation Vector
/**
 * Initialize a new Vector with zero values.
 *
 * @return vector.
 */
- (Vector *)initWithZero {
	self = [super init];
	
	if (self) {
		[self zero];
	}
	
	return self;
}

/**
 * Initialize a new Vector with the given values.
 *
 * @param vx X value.
 * @param vy Y value.
 * @return vector.
 */
- (Vector *)initWithX:(GLfloat)vx andY:(GLfloat)vy {
	self = [super init];
	
	if (self) {
		x = vx;
		y = vy;
	}
	
	return self;
}

/**
 * Set this vector to 0.0/0.0.
 */
- (void)zero {
	x = 0.0;
	y = 0.0;
}

/**
 * Set this vector to vx, vy.
 *
 * @param vx X value.
 * @param vy Y value.
 * @return vector.
 */
- (void)setX:(GLfloat)vx andY:(GLfloat)vy {
	x = vx;
	y = vy;
}

/**
 * Clone the given vector.
 *
 * @param v Vector to clone.
 */
- (void)clone:(Vector *)v {
	x = v->x;
	y = v->y;
}

/**
 * Add a vector to this vector.
 *
 * @param v Vector to clone.
 */
- (void)add:(Vector *)v {
	x += v->x;
	y += v->y;
}

/**
 * Subtract a vector from this vector.
 *
 * @param v Vector to clone.
 */
- (void)subtract:(Vector *)v {
	x -= v->x;
	y -= v->y;
}

/**
 * Multiply this vector with a constant.
 *
 * @param v Value.
 */
- (void)multiply:(GLfloat)v {
	x *= v;
	y *= v;
}

/**
 * Divide this vector with a constant.
 *
 * @param v Value.
 */
- (void)divide:(GLfloat)v {
	if (v != 0.0) {
		x /= v;
		y /= v;
	}
}

@end
