//
//  GameMgr.m
//  iStar
//
//  Created by Brian Schau on 11/05/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import "GameMgr.h"

@implementation GameMgr
static CGPoint touchPoint;
static BOOL origoSet;

/**
 * Setup a new level.
 *
 * @param level Level to setup.
 */
+ (void)setup:(int)level {
	level--;
	
	origoSet = NO;
	
	[PlayerMgr setup];
	
	[ThingMgr reset];

	PlanetMgr *p = (PlanetMgr *)[[PlanetMgr alloc] initWithX:160.0 andY:240.0 andType:PLANETL];
	if (p == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate planet" userInfo:nil];
	}
	[ThingMgr add:p];
	
	p = (PlanetMgr *)[[PlanetMgr alloc] initWithX:100.0 andY:100.0 andType:PLANETS];
	if (p == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate planet" userInfo:nil];
	}
	[ThingMgr add:p];

	p = (PlanetMgr *)[[PlanetMgr alloc] initWithX:200.0 andY:300.0 andType:PLANETM];
	if (p == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate planet" userInfo:nil];
	}
	[ThingMgr add:p];
	
	DeflectorMgr *d = (DeflectorMgr *)[[DeflectorMgr alloc] initWithX:40.0 andY:100.0 andType:DEFLECTORS];
	if (d == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate deflector" userInfo:nil];
	}
	[ThingMgr add:d];
	
	WormholeMgr *w = (WormholeMgr *)[[WormholeMgr alloc] initWithX:280.0 andY:280.0];
	if (w == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate wormhole" userInfo:nil];
	}
	[ThingMgr add:w];

	w = (WormholeMgr *)[[WormholeMgr alloc] initWithX:20.0 andY:20.0];
	if (w == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate wormhole" userInfo:nil];
	}
	[ThingMgr add:w];
	
	WallMgr *a = (WallMgr *)[[WallMgr alloc] initWithX:70.0 andY:450.0 andType:WALLVS];
	if (a == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate wall" userInfo:nil];
	}
	[ThingMgr add:a];
	
	a = (WallMgr *)[[WallMgr alloc] initWithX:70.0 andY:370.0 andType:WALLVL];
	if (a == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate wall" userInfo:nil];
	}
	[ThingMgr add:a];
	
	PaddleMgr *b = (PaddleMgr *)[[PaddleMgr alloc] initWithX:50.0 andY:290.0 andType:PADDLEVLF];
	if (b == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate paddle" userInfo:nil];
	}
	[ThingMgr add:b];

	b = (PaddleMgr *)[[PaddleMgr alloc] initWithX:40.0 andY:220.0 andType:PADDLEVSS];
	if (b == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate paddle" userInfo:nil];
	}
	[ThingMgr add:b];

	b = (PaddleMgr *)[[PaddleMgr alloc] initWithX:220.0 andY:40.0 andType:PADDLEHMS];
	if (b == nil) {
		@throw [NSException exceptionWithName:@"Game setup" reason:@"Cannot allocate paddle" userInfo:nil];
	}
	[ThingMgr add:b];	
}

/**
 * Run the level.
 */
+ (void)run:(GLView *)glView {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, spriteId);
	glColor4f(1.0, 1.0, 1.0, 1.0);

	[PlayerMgr animThings];
	[ThingMgr draw];

	if (canFire) {
		if ([glView getTouch:&touchPoint]) {
			if (origoSet == NO) {
				[PlayerMgr setOrigo:touchPoint.x andY:touchPoint.y];
				origoSet = YES;
			} else {
				if ([PlayerMgr setInMotion:touchPoint.x andY:touchPoint.y] == NO) {
					origoSet = NO;
				}
			}
		}
	} else {
		[PlayerMgr move];
	}
	
	[PlayerMgr draw];
}
@end
