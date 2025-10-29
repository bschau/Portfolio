//
//	PaddleMgr.c
//	iStar
//
//	Created by Brian Schau on 28/05/09.
//	Copyright 2009 Folera.Com. All rights reserved.
//
#import "PaddleMgr.h"

@implementation PaddleMgr

/**
 * Create a new paddle at x, y of type PADDLEVSS...HLF.
 *
 * @param x X Coordinate.
 * @param y Y Coordinate.
 * @param t Type of paddle.
 * @return paddle or nil.
 */
- (id)initWithX:(GLfloat)x andY:(GLfloat)y andType:(int)t {
	self = [super init];
	
	if (self) {
		type = t;
		location = [[Vector alloc] initWithX:x andY:y];
		sledge = [[Vector alloc] initWithX:x andY:y];
		if ((location == nil) || (sledge == nil)) {
			return nil;
		}
		
		switch (type) {
			case PADDLEVSS:
				size = [[Vector alloc] initWithX:60.0 andY:0.0];
				speed = [[Vector alloc] initWithX:1.0 andY:0.0];
				sledgeSize = [[Vector alloc] initWithX:20.0 andY:9.0];
				sprIdx = SPRPADDLEIDX + 3;
				break;
			case PADDLEVSF:
				size = [[Vector alloc] initWithX:60.0 andY:0.0];
				speed = [[Vector alloc] initWithX:2.0 andY:0.0];
				sledgeSize = [[Vector alloc] initWithX:20.0 andY:9.0];
				sprIdx = SPRPADDLEIDX + 3;
				break;
			case PADDLEVMS:
				size = [[Vector alloc] initWithX:75.0 andY:0.0];
				speed = [[Vector alloc] initWithX:1.0 andY:0.0];
				sledgeSize = [[Vector alloc] initWithX:30.0 andY:9.0];
				sprIdx = SPRPADDLEIDX + 4;
				break;
			case PADDLEVMF:
				size = [[Vector alloc] initWithX:75.0 andY:0.0];
				speed = [[Vector alloc] initWithX:2.0 andY:0.0];
				sledgeSize = [[Vector alloc] initWithX:30.0 andY:9.0];
				sprIdx = SPRPADDLEIDX + 4;
				break;
			case PADDLEVLS:
				size = [[Vector alloc] initWithX:90.0 andY:0.0];
				speed = [[Vector alloc] initWithX:1.0 andY:0.0];
				sledgeSize = [[Vector alloc] initWithX:40.0 andY:9.0];
				sprIdx = SPRPADDLEIDX + 5;
				break;
			case PADDLEVLF:
				size = [[Vector alloc] initWithX:90.0 andY:0.0];
				speed = [[Vector alloc] initWithX:2.0 andY:0.0];
				sledgeSize = [[Vector alloc] initWithX:40.0 andY:9.0];
				sprIdx = SPRPADDLEIDX + 5;
				break;
			case PADDLEHSS:
				size = [[Vector alloc] initWithX:0.0 andY:60.0];
				speed = [[Vector alloc] initWithX:0.0 andY:1.0];
				sledgeSize = [[Vector alloc] initWithX:9.0 andY:20.0];
				sprIdx = SPRPADDLEIDX;
				break;
			case PADDLEHSF:
				size = [[Vector alloc] initWithX:0.0 andY:60.0];
				speed = [[Vector alloc] initWithX:0.0 andY:2.0];
				sledgeSize = [[Vector alloc] initWithX:9.0 andY:20.0];
				sprIdx = SPRPADDLEIDX;
				break;
			case PADDLEHMS:
				size = [[Vector alloc] initWithX:0.0 andY:75.0];
				speed = [[Vector alloc] initWithX:0.0 andY:1.0];
				sledgeSize = [[Vector alloc] initWithX:9.0 andY:30.0];
				sprIdx = SPRPADDLEIDX + 1;
				break;
			case PADDLEHMF:
				size = [[Vector alloc] initWithX:0.0 andY:75.0];
				speed = [[Vector alloc] initWithX:0.0 andY:2.0];
				sledgeSize = [[Vector alloc] initWithX:9.0 andY:30.0];
				sprIdx = SPRPADDLEIDX + 1;
				break;
			case PADDLEHLS:
				size = [[Vector alloc] initWithX:0.0 andY:90.0];
				speed = [[Vector alloc] initWithX:0.0 andY:1.0];
				sledgeSize = [[Vector alloc] initWithX:9.0 andY:40.0];
				sprIdx = SPRPADDLEIDX + 2;
				break;
			default:		// PADDLEHLF:
				size = [[Vector alloc] initWithX:0.0 andY:90.0];
				speed = [[Vector alloc] initWithX:0.0 andY:2.0];
				sledgeSize = [[Vector alloc] initWithX:9.0 andY:40.0];
				sprIdx = SPRPADDLEIDX + 2;
				break;
		}
		
		if ((size == nil) || (speed == nil) || (sledgeSize == nil)) {
			return nil;
		}
		
		vertices[2] = 0.0;
		vertices[5] = 0.0;
		vertices[8] = 0.0;
		vertices[11] = 0.0;
		
		sprIdx *= 8;
	}
	
	return self;
}

/**
 * Release all objects.
 */
- (void)release {
	[location release];
	[size release];
	[speed release];
	[sledge release];
	[sledgeSize release];
	[super release];
}

/**
 * Draw this deflector.
 */
- (void)draw {
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	
	glTexCoordPointer(2, GL_FLOAT, 0, &spriteDefs[sprIdx]);

	vertices[0] = sledge->x - (PADDLESIZE / 2.0);
	vertices[1] = sledge->y - (PADDLESIZE / 2.0);
	vertices[3] = sledge->x + (PADDLESIZE / 2.0);
	vertices[4] = vertices[1];
	vertices[6] = vertices[3];
	vertices[7] = sledge->y + (PADDLESIZE / 2.0);
	vertices[9] = vertices[0];
	vertices[10] = vertices[7];
	
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);		
}

/**
 * Move this deflector.
 */
- (void)move {
	[sledge add:speed];
	
	if (sledge->x <= location->x) {
		sledge->x = location->x;
		speed->x = -speed->x;
	} else if ((sledge->x + sledgeSize->x) >= (location->x + size->x)) {
		sledge->x = location->x + size->x - sledgeSize->x;
		speed->x = -speed->x;
	}

	if (sledge->y <= location->y) {
		sledge->y = location->y;
		speed->y = -speed->y;
	} else if ((sledge->y + sledgeSize->y) >= (location->y + size->y)) {
		sledge->y = location->y + size->y - sledgeSize->y;
		speed->y = -speed->y;
	}
}
@end
