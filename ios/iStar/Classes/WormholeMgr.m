//
//	WormholeMgr.c
//	iStar
//
//	Created by Brian Schau on 23/05/09.
//	Copyright 2009 Folera.Com. All rights reserved.
//
#import "WormholeMgr.h"

@implementation WormholeMgr
#define ANIMDELAY 3
#define ANIMFRAMES 5

/**
 * Create a new wormhole at x, y. There can only be two pr. level.
 *
 * @param x X Coordinate.
 * @param y Y Coordinate.
 * @return wormhole or nil.
 */
- (id)initWithX:(GLfloat)x andY:(GLfloat)y {
	self = [super init];
	
	if (self) {
		location = [[Vector alloc] initWithX:x andY:y];
		if (location == nil) {
			return nil;
		}
				
		vertices[0] = location->x - (WORMHOLESIZE / 2.0);
		vertices[1] = location->y - (WORMHOLESIZE / 2.0);
		vertices[2] = 0.0;
		vertices[3] = location->x + (WORMHOLESIZE / 2.0);
		vertices[4] = vertices[1];
		vertices[5] = 0.0;
		vertices[6] = vertices[3];
		vertices[7] = location->y + (WORMHOLESIZE / 2.0);
		vertices[8] = 0.0;
		vertices[9] = vertices[0];
		vertices[10] = vertices[7];
		vertices[11] = 0.0;
		
		[self clearAnimation];
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
 * Draw this wormhole.
 */
- (void)draw {
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	
	glTexCoordPointer(2, GL_FLOAT, 0, &spriteDefs[(animIdx + SPRWORMHOLEIDX) * 8]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);	
}

/**
 * Clear animation constants.
 */
- (void)clearAnimation {
	animIdx = 0;
	animDelay = 0;
}

/**
 * Animate this wormhole.
 */
- (void)animate {
	animDelay++;
	if (animDelay >= ANIMDELAY) {
		animDelay = 0;
		animIdx++;
		NSLog(@"Bump idx to %i", animIdx);
		if (animIdx >= ANIMFRAMES) {
			animIdx = 0;
		}
	}
}
@end
