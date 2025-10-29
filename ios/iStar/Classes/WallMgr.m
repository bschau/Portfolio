//
//	WallMgr.c
//	iStar
//
//	Created by Brian Schau on 27/05/09.
//	Copyright 2009 Folera.Com. All rights reserved.
//
#import "WallMgr.h"

@implementation WallMgr
#define WALLSIZE 60.0

/**
 * Create a new wall at x, y of type WALLHS|WALLHM|WALLHL|WALLVS|WALLVM|WALLVL.
 *
 * @param x X Coordinate.
 * @param y Y Coordinate.
 * @param t Type of wall.
 * @return wall or nil.
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
			case WALLHS:
				size = [[Vector alloc] initWithX:10.0 andY:30.0];
				break;
			case WALLHM:
				size = [[Vector alloc] initWithX:10.0 andY:45.0];
				break;
			case WALLHL:
				size = [[Vector alloc] initWithX:10.0 andY:60.0];
				break;
			case WALLVS:
				size = [[Vector alloc] initWithX:30.0 andY:10.0];
				break;
			case WALLVM:
				size = [[Vector alloc] initWithX:45.0 andY:10.0];
				break;
			default:	// WALLVL
				size = [[Vector alloc] initWithX:60.0 andY:10.0];
				break;
		}
		
		if (size == nil) {
			return nil;
		}
		
		vertices[0] = location->x - (WALLSIZE / 2.0);
		vertices[1] = location->y - (WALLSIZE / 2.0);
		vertices[2] = 0.0;
		vertices[3] = location->x + (WALLSIZE / 2.0);
		vertices[4] = vertices[1];
		vertices[5] = 0.0;
		vertices[6] = vertices[3];
		vertices[7] = location->y + (WALLSIZE / 2.0);
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
	[size release];
	[super release];
}

/**
 * Draw this deflector.
 */
- (void)draw {
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	
	glTexCoordPointer(2, GL_FLOAT, 0, &spriteDefs[(type + SPRWALLIDX) * 8]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);		
}
@end
