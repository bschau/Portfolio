//
//  PlayerMgr.h
//  iStar
//
//  Created by Brian Schau on 11/05/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>
#import "GfxMgr.h"
#import "Vector.h"
#import "ThingMgr.h"
#import "DeflectorMgr.h"
#import "PaddleMgr.h"
#import "PlanetMgr.h"
#import "WallMgr.h"
#import "WormholeMgr.h"

@interface PlayerMgr : NSObject {}

+ (void)classInit;
+ (void)releaseAll;
+ (void)setup;
+ (void)draw;
+ (void)setOrigo:(GLfloat)x andY:(GLfloat)y;
+ (BOOL)setInMotion:(GLfloat)x andY:(GLfloat)y;
+ (void)move;
+ (void)adjustPlayerPosition;
+ (void)animThings;

#ifdef PLAYERMGR
BOOL canFire;
#else
extern BOOL canFire;
#endif

@end
