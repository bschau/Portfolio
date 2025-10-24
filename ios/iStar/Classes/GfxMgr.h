//
//  GfxMgr.h
//  iStar
//
//  Created by Brian Schau on 10/05/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>

@interface GfxMgr : NSObject {}

typedef struct {
	int cnt;
	int size;
} SprVInfo;

+ (void)releaseAll;
+ (void)loadSprites;

#ifdef GFXMGR
GLushort verticeIdx[] = { 1, 2, 0, 3};
GLuint spriteId;
GLfloat *spriteDefs;
#else
extern GLushort verticeIdx[];
extern GLuint spriteId;
extern GLfloat *spriteDefs;
#endif

@end
