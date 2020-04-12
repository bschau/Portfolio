//
//  PlayerMgr.m
//  iStar
//
//  Created by Brian Schau on 11/05/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#define PLAYERMGR
#import "PlayerMgr.h"

@interface PlayerMgr (private)
+ (void)drawFirepad;
+ (void)drawPlayer:(GLfloat)x andY:(GLfloat)y;
+ (void)resetPlayer;
+ (void)attract:(GLfloat)G andLocation:(Vector *)loc andMass:(GLfloat)mass;
+ (void)deflect:(GLfloat)G andLocation:(Vector *)loc andMass:(GLfloat)mass;
+ (void)adjustVelocity;
+ (BOOL)detectCollision;
+ (BOOL)isInRange:(Vector *)loc andSize:(GLfloat)size;
+ (BOOL)boxCollision:(Vector *)l andSize:(Vector *)s;
@end

@implementation PlayerMgr
#define FIREPADZONE 8.0			// "touch zone"
#define FIREPADFADEDELAY 3		// Delay between frames when animating the firepad
#define FIREPADFRAMES 4			// Number of animations
#define SPEED 6.0				// Max player speed
#define COLLISIONFACTOR 6		// int(SPEED)
#define FIREPADR 160.0			// Size of firepad
#define PLAYERR 20.0			// Size of player
#define PIHALF (M_PI / 180.0)	// pi/180
#define PLAYERMASS 3000.0		// Mass of player
#define PLANETGRAVITY 0.1		// Planet gravity
#define DEFLECTORGRAVITY 0.2	// Deflector gravity
#define TRAIL 9					// Number of ghost players
#define NODETECT 4				// Skip frames when not to detect (wall, paddle)
#define NODETECTWORMHOLE 4		// Skip frames when not to detect (wormhole)
#define WORMHOLERANGE 120		// Range to player when animating wormhole (this will / 2.0 in isInRange)

static GLfloat fade[] = { 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };
static GLfloat trailX[TRAIL];
static GLfloat trailY[TRAIL];
static int fadeIdx, fadeDelay, firepadIdx, firepadDelay, explodingIdx, explodingDelay;
static GLfloat vertices[12];
static BOOL showFirepad, fadeFirepadIn, fadeFirepadOut, showPlayer, exploding;
static Vector *location, *velocity, *accel, *unit, *save, *step;
static int noWormholeTill, noWallTill, noPaddleTill;

/**
 * Initialize class variables.
 */
+ (void)classInit {
	velocity = [[Vector alloc] initWithZero];
	location = [[Vector alloc] initWithZero];
	accel = [[Vector alloc] initWithZero];
	unit = [[Vector alloc] initWithZero];
	save = [[Vector alloc] initWithZero];
	step = [[Vector alloc] initWithZero];
	
	if ((velocity == nil) || (location == nil) || (accel == nil) ||
		(unit == nil) || (save == nil) || (step == nil)) {
		@throw [NSException exceptionWithName:@"classInit" reason:@"Cannot allocate vectors" userInfo:nil];
	}
}

/**
 * Release all.
 */
+ (void)releaseAll {
	[velocity release];
	[location release];
	[accel release];
	[unit release];
	[save release];
}

/**
 * Setup player state. This must be done on each level.
 */
+ (void)setup {
	[PlayerMgr resetPlayer];
	showFirepad = false;
	exploding = false;
	fadeIdx = 0;
	fadeDelay = 0;
	noWormholeTill = 0;
	noWallTill = 0;
	noPaddleTill = 0;
	
	for (int i = 0; i < 12; i++) {
		vertices[i] = 0.0;
	}
}

/**
 * Draw Firepad.
 */
+ (void)drawFirepad {
	GLfloat sX = location->x - (FIREPADR / 2.0), sY = location->y - (FIREPADR / 2.0);
	
	firepadDelay++;
	if (firepadDelay > 1) {
		firepadDelay = 0;
		firepadIdx++;
		if (firepadIdx >= (SPRFIREPADIDX + FIREPADFRAMES)) {
			firepadIdx = SPRFIREPADIDX;
		}
	}
	
	vertices[0] = sX;
	vertices[1] = sY;
	vertices[3] = sX + FIREPADR;
	vertices[4] = sY;
	vertices[6] = vertices[3];
	vertices[7] = sY + FIREPADR;
	vertices[9] = sX;
	vertices[10] = vertices[7];

	glTexCoordPointer(2, GL_FLOAT, 0, &spriteDefs[firepadIdx * 8]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);
}

/**
 * Draw player.
 *
 * @param x X Coordinate.
 * @param y Y Coordinate.
 */
+ (void)drawPlayer:(GLfloat)x andY:(GLfloat)y {
	x -= (PLAYERR / 2.0);
	y -= (PLAYERR / 2.0);
	
	vertices[0] = x;
	vertices[1] = y;
	vertices[3] = x + PLAYERR;
	vertices[4] = y;
	vertices[6] = vertices[3];
	vertices[7] = y + PLAYERR;
	vertices[9] = x;
	vertices[10] = vertices[7];
	
	glTexCoordPointer(2, GL_FLOAT, 0, &spriteDefs[SPRPLAYERIDX * 8]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);
}

/**
 * Draw player state.
 */
+ (void)draw {
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	if (showFirepad) {
		glColor4f(fade[fadeIdx], fade[fadeIdx], fade[fadeIdx], 1.0);

		if (fadeFirepadIn) {
			if (fadeIdx < 10) {
				fadeDelay++;
				if (fadeDelay == FIREPADFADEDELAY) {
					fadeDelay = 0;
					fadeIdx++;
				}
			} else {
				fadeFirepadIn = false;
			}
		} else if (fadeFirepadOut) {
			if (fadeIdx > 0) {
				fadeDelay++;
				if (fadeDelay == FIREPADFADEDELAY) {
					fadeDelay = 0;
					fadeIdx -= 3;
				}
			} else {
				showFirepad = false;
				fadeFirepadOut = false;
			}
		}
		
		[PlayerMgr drawFirepad];
	}

	if (showPlayer) {
		if (fadeFirepadIn) {
			glColor4f(fade[fadeIdx], fade[fadeIdx], fade[fadeIdx], 1.0);
		} else {
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}

		if (!canFire) {
			int idx = TRAIL - 1, i;
			GLfloat alpha = 0.1;
		
			for (i = 0; i < TRAIL; i++) {
				glColor4f(alpha, alpha, alpha, 0.0);
				[PlayerMgr drawPlayer:trailX[idx] andY:trailY[idx]];
			
				idx--;
				alpha += 0.1;
			}
		
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}
		
		[PlayerMgr drawPlayer:location->x andY:location->y];
	}
}

/**
 * Set origo of this shot.
 *
 * @param x X coordinate.
 * @param y Y coordinate.
 */
+ (void)setOrigo:(GLfloat)x andY:(GLfloat)y {
	y = 480.0 - y;
	[location setX:x andY:y];
	
	for (int i = 0; i < TRAIL; i++) {
		trailX[i] = x;
		trailY[i] = y;
	}
	
	fadeIdx = 0;
	fadeDelay = 0;
	firepadDelay = 0;
	firepadIdx = SPRFIREPADIDX;
	showFirepad = true;
	fadeFirepadIn = true;
	fadeFirepadOut = false;
	showPlayer = true;
}

/**
 * Set player in motion.
 *
 * @param x Destination x coordinate.
 * @param y Destination y coordinate.
 * @return YES if player is set in motion, NO otherwise.
 */
+ (BOOL)setInMotion:(GLfloat)x andY:(GLfloat)y {
	y = 480.0 - y;
	GLfloat xf = location->x - x, yf = location->y - y;
	
	if ((abs(xf) <= FIREPADZONE) && (abs(yf) <= FIREPADZONE)) {
		[PlayerMgr resetPlayer];
		return NO;
	}		
	
	yf *= -1.0;
	GLfloat power = sqrt(abs((xf * xf) + (yf * yf)));
	
	if (power > (FIREPADR / 2.0)) {
		[PlayerMgr resetPlayer];
		return NO;
	}
	
	power /= SPEED;
	if (power < 1.0) {
		power = 1.0;
	}
	
	GLfloat angle = atan(yf / xf) / PIHALF;
	
	if (xf < 0.0) {
		angle += 180.0;
	}
	
	if ((xf >= 0.0) && (yf < 0.0)) {
		angle += 360.0;
	}
	
	if (angle >= 360.0) {
		angle -= 360.0;
	}
	
	if (power < 1.0) {
		power = 1.0;
	}
	
	angle *= PIHALF * -1.0;
	[velocity setX:(cos(angle) * power) andY:(sin(angle) * power)];
	
	canFire = false;
	fadeFirepadOut = true;
	fadeFirepadIn = false;
	
	return YES;
}

/**
 * Reset player variables.
 */
+ (void)resetPlayer {
	canFire = true;
	fadeFirepadOut = true;
	fadeFirepadIn = false;
	showPlayer = false;
	location->x = -400.0;
	location->y = -400.0;
}

/**
 * Move player.
 */
+ (void)move {
	id thing;
	
	if (exploding) {
		return;
	}
	
	[accel zero];
	
	for (int idx = 0; idx < [ThingMgr getNumThings]; idx++) {
		thing = [ThingMgr get:idx];
		if ([thing class] == [PlanetMgr class]) {
			[PlayerMgr attract:PLANETGRAVITY andLocation:((PlanetMgr *)thing)->location andMass:((PlanetMgr *)thing)->mass];			
		} else if ([thing class] == [DeflectorMgr class]) {
			[PlayerMgr deflect:DEFLECTORGRAVITY andLocation:((DeflectorMgr *)thing)->location andMass:((DeflectorMgr *)thing)->mass];
		} else if ([thing class] == [WormholeMgr class]) {
			if (noWormholeTill == 0) {
				[PlayerMgr attract:WORMHOLEGRAVITY andLocation:((WormholeMgr *)thing)->location andMass:WORMHOLEMASS];
			}
		}
	
	}

	if (noWormholeTill > 0) {
		noWormholeTill--;
	}
	
	if (noWallTill > 0) {
		noWallTill--;
	}
	
	if (noPaddleTill > 0) {
		noPaddleTill--;
	}
	
	[velocity add:accel];
	[PlayerMgr adjustVelocity];
	
	[save clone:location];
	[save add:velocity];

	[step clone:velocity];
	[step divide:(GLfloat)COLLISIONFACTOR];
	
	BOOL collided = NO;
	for (int i = 0; i < COLLISIONFACTOR; i++) {
		[location add:step];
		if ([PlayerMgr detectCollision] == YES) {
			collided = YES;
			break;
		}
	}
	
	if (collided == NO) {
		[location clone:save];
		[PlayerMgr detectCollision];
	}
	
	[PlayerMgr adjustPlayerPosition];
	
	for (int i = TRAIL - 1; i > 0; i--) {
		trailX[i] = trailX[i - 1];
		trailY[i] = trailY[i - 1];
	}
	
	trailX[0] = location->x;
	trailY[0] = location->y;
}

/**
 * Handle attractors.
 *
 * @param G gravity constant.
 * @param loc Location.
 * @param m mass.
 */
+ (void)attract:(GLfloat)G andLocation:(Vector *)loc andMass:(GLfloat)mass {
	GLfloat magnitude, factor;
	
	[unit clone:loc];
	[unit subtract:location];
	
	magnitude = sqrt(abs((unit->x * unit->x) + (unit->y * unit->y)));
	factor = (G * ((PLAYERMASS * mass) / (magnitude * magnitude * magnitude))) / PLAYERMASS;
	
	[unit multiply:factor];
	[accel add:unit];
}

/**
 * Handle deflectors.
 *
 * @param G gravity constant.
 * @param loc Location.
 * @param m mass.
 */
+ (void)deflect:(GLfloat)G andLocation:(Vector *)loc andMass:(GLfloat)mass {
	GLfloat magnitude, factor;
	
	[unit clone:loc];
	[unit subtract:location];
	
	magnitude = sqrt(abs((unit->x * unit->x) + (unit->y * unit->y)));
	factor = -((G * ((PLAYERMASS * mass) / (magnitude * magnitude * magnitude))) / PLAYERMASS);
	
	[unit multiply:factor];
	[accel add:unit];
}

/**
 * Adjust player velocity.
 */
+ (void)adjustVelocity {
	if (velocity->x > SPEED) {
		velocity->x = SPEED;
	} else if (velocity->x < -SPEED) {
		velocity->x = -SPEED;
	}

	if (velocity->y > SPEED) {
		velocity->y = SPEED;
	} else if (velocity->y < -SPEED) {
		velocity->y = -SPEED;
	}
}

/**
 * Adjust player position - bounce player if at edge of screen.
 */
+ (void)adjustPlayerPosition {
	GLfloat sz = PLAYERR / 2.0;
	
	if (location->x <= sz) {
		velocity->x = -velocity->x;
		location->x = (sz + 0.1);
	} else if (location->x >= (320.0 - sz)) {
		velocity->x = -velocity->x;
		location->x = 319.9 - sz;
	}

	if (location->y <= sz) {
		velocity->y = -velocity->y;
		location->y = (sz + 0.1);
	} else if (location->y >= (480.0 - sz)) {
		velocity->y = -velocity->y;
		location->y = 479.9 - sz;
	}
}

/**
 * Detect collision between player and things.
 *
 * @return YES if collision was detected, NO otherwise.
 */
+ (BOOL)detectCollision {
	int numThings = [ThingMgr getNumThings], idx;
	id thing, t;

	for (idx = 0; idx < numThings; idx++) {
		thing = [ThingMgr get:idx];
		
		if ([thing class] == [PlanetMgr class]) {
			PlanetMgr *p = (PlanetMgr *)thing;
			if ([PlayerMgr isInRange:p->location andSize:p->size] == YES) {
				exploding = YES;
				explodingIdx = 0;
				explodingDelay = 0;
				return YES;
			}
			
		} else if ([thing class] == [WormholeMgr class]) {
			if (noWormholeTill == 0) {
				WormholeMgr *w = (WormholeMgr *)thing;
				
				if ([PlayerMgr isInRange:w->location andSize:1.0] == YES) {
					for (int j = 0; j < numThings; j++) {
						if (j == idx) {
							continue;
						}
						
						t = [ThingMgr get:j];
						if ([t class] == [WormholeMgr class]) {
							w = t;
							break;
						}
					}
					
					[location clone:w->location];
					[velocity multiply:2.0];
					[PlayerMgr adjustVelocity];
					
					noWormholeTill = NODETECTWORMHOLE;
					return YES;
				}
			}
		} else if ([thing class] == [WallMgr class]) {
			if (noWallTill == 0) {
				WallMgr *w = (WallMgr *)thing;
				
				if ([PlayerMgr boxCollision:w->location andSize:w->size]) {
					noWallTill = NODETECT; 
					return YES;
				}
			}
		} else if ([thing class] == [PaddleMgr class]) {
			if (noPaddleTill == 0) {
				PaddleMgr *p = (PaddleMgr *)thing;
				
				if ([PlayerMgr boxCollision:p->sledge andSize:p->sledgeSize]) {
					noPaddleTill = NODETECT;
					return YES;
				}
			}
		}
	}
	
	return NO;
}

/**
 * Detect if the player is in range of a given object.
 *
 * @param loc Location of thing.
 * @param sz Size of thing.
 * @return YES if in range, NO otherwise.
 */
+ (BOOL)isInRange:(Vector *)loc andSize:(GLfloat)sz {
	GLfloat x = location->x - loc->x, y = location->y - loc->y;
	GLfloat l = sqrt(abs((x * x) + (y * y)));
	
	if ((l - (PLAYERR / 2.0) - (sz / 2.0)) <= 1.0) {
		return YES;
	}
	
	return NO;
}

/**
 * Handle collisions against a "box" (wall, paddle).
 *
 * @param l Location of wall.
 * @param s Size of wall.
 * @return YES if collided, NO otherwise.
 */
+ (BOOL)boxCollision:(Vector *)l andSize:(Vector *)s {
	GLfloat sz = PLAYERR / 2.0;
	GLfloat sxHalf = s->x / 2.0, syHalf = s->y / 2.0;
	GLfloat uX = l->x - sxHalf - sz, uY = l->y - syHalf - sz;
	GLfloat lX = l->x + sxHalf + sz, lY = l->y + syHalf + sz;
	
	if ((location->x >= uX) && (location->x <= lX) &&
		(location->y >= uY) && (location->y <= lY)) {
		// upper side
		uX = l->x - sxHalf;
		uY = l->y - syHalf;
		lY = uY;
		uY -= sz;
		lX = l->x + sxHalf;
		if ((location->x >= uX) && (location->x <= lX) &&
			(location->y >= uY) && (location->y <= lY)) {
			velocity->y = -velocity->y;
			return YES;
		}
		
		// lower side
		uY = l->y + syHalf;
		lY = uY + sz;
		if ((location->x >= uX) && (location->x <= lX) &&
			(location->y >= uY) && (location->y <= lY)) {
			velocity->y = -velocity->y;
			return YES;
		}
		
		// left side
		uX = l->x - sxHalf;
		lX = uX;
		uX -= sz;
		uY = l->y - syHalf;
		lY = l->y + syHalf;
		if ((location->x >= uX) && (location->x <= lX) &&
			(location->y >= uY) && (location->y <= lY)) {
			velocity->x = -velocity->x;
			return YES;
		}
		
		// right side
		uX = l->x + sxHalf;
		lX = uX + sz;
		if ((location->x >= uX) && (location->x <= lX) &&
			(location->y >= uY) && (location->y <= lY)) {
			velocity->x = -velocity->x;
			return YES;
		}

		velocity->x = -velocity->x;
		velocity->y = -velocity->y;
		return YES;
	}
	
	return NO;
}

/**
 * Animate things.
 */
+ (void)animThings {
	int numThings = [ThingMgr getNumThings], idx;
	id thing;
	
	for (idx = 0; idx < numThings; idx++) {
		thing = [ThingMgr get:idx];
		
		if ([thing class] == [WormholeMgr class]) {
			WormholeMgr *w = (WormholeMgr *)thing;
			
			if ([PlayerMgr isInRange:w->location andSize:WORMHOLERANGE] == YES) {
				[w animate];
			} else {
				[w clearAnimation];
			}
		} else if ([thing class] == [PaddleMgr class]) {
			[(PaddleMgr *)thing move];
		}
	}
}

@end