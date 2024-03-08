//
//	DeflectorMgr.c
//	iStar
//
//	Created by Brian Schau on 23/05/09.
//	Copyright 2009 Folera.Com. All rights reserved.
//
#import "DeflectorMgr.h"

@implementation DeflectorMgr
#define DEFLECTORSIZE 24.0

/**
 * Create a new deflector at x, y of type DeflectorSmall|DeflectorMedium|DeflectorLarge.
 *
 * @param x X Coordinate.
 * @param y Y Coordinate.
 * @param t Type of deflector.
 * @return deflector or nil.
 */
- (id)initWithX:(GLfloat)x andY:(GLfloat)y andType:(int)t {
	self = [super init];
	
	if (self) {
		type = t;
		location = [[Vector alloc] initWithX:x andY:y];
		if (location == nil) {
			return nil;
		}
		
		switch (type) {
			case DEFLECTORS:
				mass = 2000.0;
				size = 16.0;
				break;
			case DEFLECTORM:
				mass = 3000.0;
				size = 20.0;
				break;
			default:	// DEFLECTORL
				mass = 4000.0;
				size = 24.0;
				break;
		}
		
		vertices[0] = location->x - (DEFLECTORSIZE / 2.0);
		vertices[1] = location->y - (DEFLECTORSIZE / 2.0);
		vertices[2] = 0.0;
		vertices[3] = location->x + (DEFLECTORSIZE / 2.0);
		vertices[4] = vertices[1];
		vertices[5] = 0.0;
		vertices[6] = vertices[3];
		vertices[7] = location->y + (DEFLECTORSIZE / 2.0);
		vertices[8] = 0.0;
		vertices[9] = vertices[0];
		vertices[10] = vertices[7];
		vertices[11] = 0.0;
	}
	
	return self;
}

/**
 * Release all objects.
 */
- (void)release {
	[location release];
	[super release];
}

/**
 * Draw this deflector.
 */
- (void)draw {
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	
	glTexCoordPointer(2, GL_FLOAT, 0, &spriteDefs[(type + SPRDEFLECTORIDX) * 8]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);		
}
@end
