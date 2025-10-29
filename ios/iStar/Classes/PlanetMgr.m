//
//  PlanetMgr.m
//  iStar
//
//  Created by Brian Schau on 18/05/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import "PlanetMgr.h"

@interface PlanetMgr (private)
- (void)setPlanetAttributes;
@end

@implementation PlanetMgr
#define PLANETSIZE 24.0

/**
 * Create a new planet at x, y of type PlanetSmall|PlanetMedium|PlanetLarge.
 *
 * @param x X Coordinate.
 * @param y Y Coordinate.
 * @param t Type of planet.
 * @return planet or nil.
 */
- (id)initWithX:(GLfloat)x andY:(GLfloat)y andType:(int)t {
	self = [super init];

	if (self) {
		type = t;
		location = [[Vector alloc] initWithX:x andY:y];
		if (location == nil) {
			return nil;
		}

		[self setPlanetAttributes];
	
		vertices[0] = location->x - (PLANETSIZE / 2.0);
		vertices[1] = location->y - (PLANETSIZE / 2.0);
		vertices[2] = 0.0;
		vertices[3] = location->x + (PLANETSIZE / 2.0);
		vertices[4] = vertices[1];
		vertices[5] = 0.0;
		vertices[6] = vertices[3];
		vertices[7] = location->y + (PLANETSIZE / 2.0);
		vertices[8] = 0.0;
		vertices[9] = vertices[0];
		vertices[10] = vertices[7];
		vertices[11] = 0.0;
	}
	
	return self;
}

/**
 * Set the attributes of this planet.
 */
- (void)setPlanetAttributes {
	switch (type) {
		case PLANETS:
			mass = 1500.0;
			size = 7.0;
			break;
		case PLANETM:
			mass = 2500.0;
			size = 11.0;
			break;
		default:	// PLANETL
			mass = 5000.0;
			size = 15.0;
			break;
	}
}

/**
 * Release all objects.
 */
- (void)release {
	[location release];
	[super release];
}

/**
 * Draw this planet.
 */
- (void)draw {
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	
	glTexCoordPointer(2, GL_FLOAT, 0, &spriteDefs[(type + SPRPLANETIDX) * 8]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);		
}

/**
 * Grow this planet.
 */
- (void)grow {
	if (type == PLANETM) {
		type = PLANETL;
	} else if (type == PLANETS) {
		type = PLANETM;
	}
	
	[self setPlanetAttributes];
}
@end
