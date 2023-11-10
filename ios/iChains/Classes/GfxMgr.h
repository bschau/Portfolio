//
//  GfxMgr.h
//  iChains
//
//  Created by Brian Schau on 07/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>

@interface GfxMgr : NSObject {}

typedef struct {
	int cnt;
	int size;
} SprVInfo;

+ (void) releaseAll;
+ (void) loadIntroSprites;
+ (void) releaseIntroSprites;
+ (void) loadPrefsSprites;
+ (void) releasePrefsSprites;

#ifdef GFXMGR
GLushort verticeIdx[] = { 1, 2, 0, 3};
GLuint introId, prefsId;
GLfloat *introDefs, *prefsDefs;
#else
extern GLushort verticeIdx[];
extern GLuint introId, prefsId;
extern GLfloat *introDefs, *prefsDefs;
#endif

@end
