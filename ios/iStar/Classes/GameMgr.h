//
//  GameMgr.h
//  iStar
//
//  Created by Brian Schau on 11/05/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>
#import "GLView.h"
#import "GfxMgr.h"
#import "PlayerMgr.h"
#import "ThingMgr.h"
#import "DeflectorMgr.h"
#import "PaddleMgr.h"
#import "PlanetMgr.h"
#import "WallMgr.h"
#import "WormholeMgr.h"
#import "Constants.h"

@interface GameMgr : NSObject {}

+ (void)setup:(int)level;
+ (void)run:(GLView *)glView;

@end
