//
//  GfxMgr.m
//  iChains
//
//  Created by Brian Schau on 07/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#define GFXMGR
#import "GfxMgr.h"

@interface GfxMgr (private)
+ (size_t) getLargestPowerOf2:(size_t) width andHeight:(size_t) height;
+ (void) loadAndBindTexture:(GLuint *) tId andImage:(CGImageRef) img andFlip:(BOOL) flip;
+ (int) getSprDefCount:(SprVInfo *) info;
+ (void) loadAndInitializeSpriteSheet:(SprVInfo *) info andId:(GLuint *) objId andDefs:(GLfloat **) defs andRef:(CGImageRef *) ref andGfx:(NSString *) name;
@end

@implementation GfxMgr
static CGImageRef introRef, prefsRef;
static size_t quad;

/**
 * Release all loaded graphics.
 */
+ (void) releaseAll {
	[GfxMgr releaseIntroSprites];
	[GfxMgr releasePrefsSprites];
}

/**
 * Find the smallest quadrant which will contain both height and width.
 *
 * F.ex. a 320 x 240 can be fitted into a quadrant of 512.
 * A 256 x 256 can be fitted into a quadrant of 256.
 *
 * @param width Width.
 * @param height Height.
 * @return quadrant.
 */
+ (size_t) getLargestPowerOf2:(size_t) width andHeight:(size_t) height {
	int t = (width > height ? width : height), i;
	
	for (i = 1; i < 1024; ) {
		if (i >= t) {
			break;
		}
		
		i <<= 1;
	}
	
	return (size_t) i;
}

/**
 * Load and bind a texture.
 *
 * @param tId Texture ID destination.
 * @param img Image (ref).
 * @param flip Flip texture upside down (= YES).
 */ 
+ (void) loadAndBindTexture:(GLuint *) tId andImage:(CGImageRef) img andFlip:(BOOL) flip {
	size_t width = CGImageGetWidth(img), height = CGImageGetHeight(img);
	quad = [GfxMgr getLargestPowerOf2:width andHeight:height];
	GLubyte *data = (GLubyte *) calloc(quad * quad * 4, 1);
	CGContextRef ctx;
	GLint saveId;
	
	if (!data) {
		@throw [NSException exceptionWithName:@"loadAndBindTexture" reason:@"Cannot allocate quad" userInfo:nil];
	}
	
	CGRect bounds = CGRectMake(0.0f, 0.0f, (CGFloat)width, (CGFloat)height);
	
	ctx = CGBitmapContextCreate(data, quad, quad, 8, quad * 4, CGImageGetColorSpace(img), kCGImageAlphaPremultipliedLast);
	if (flip) {
		CGContextScaleCTM(ctx, 1.0f, -1.0f);
		bounds.size.height = bounds.size.height * -1;
	}
	
	CGContextDrawImage(ctx, bounds, img);
	CGContextRelease(ctx);
	
	glGenTextures(1, tId);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveId);
	glBindTexture(GL_TEXTURE_2D, *tId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, quad, quad, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, saveId);
	free(data);
}

/**
 * Return the number of sprites in this SprVInfo.
 *
 * @param info Sprites info.
 * @return count of sprites.
 */
+ (int) getSprDefCount:(SprVInfo *) info {
	int cnt = 0, idx;
	
	for (idx = 0; info[idx].cnt > 0; idx++) {
		cnt += info[idx].cnt;
	}
	
	return cnt;
}

/**
 * Load and bind spritesheet.
 *
 * @param info Sprites info.
 * @param id Where to store OpenGL id.
 * @param defs Where to store sprite defs.
 * @param ref Where to store image ref.
 * @param name Graphics name.
 */
+ (void) loadAndInitializeSpriteSheet:(SprVInfo *) info andId:(GLuint *) objId andDefs:(GLfloat **) defs andRef:(CGImageRef *) ref andGfx:(NSString *) name {
	int sprCnt = [GfxMgr getSprDefCount:info];
	
	*defs = malloc(sprCnt * 8 * sizeof(GLfloat));
	if (*defs == NULL) {
		@throw [NSException exceptionWithName:@"loadAndInitializeSpriteSheet" reason:@"Cannot alloc vertices" userInfo:nil];
	}
	
	*ref = [UIImage imageNamed:name].CGImage;
	if (!*ref) {
		@throw [NSException exceptionWithName:@"loadAndInitializeSpriteSheet" reason:@"Cannot load image" userInfo:nil];
	}
	
	[GfxMgr loadAndBindTexture:objId andImage:*ref andFlip:YES];

	GLfloat y = 1.0f, q = (GLfloat) quad, *d = *defs, x, s;
	int idx = 0, j;
	
	for (idx = 0; info[idx].cnt > 0; idx++) {
		s = 1.0 / (q / (GLfloat) info[idx].size);
		x = 0.0f;
		for (j = 0; j < info[idx].cnt; j++) {
			*d++ = x;
			*d++ = y - s;
			*d++ = x + s;
			*d++ = y - s;
			*d++ = x + s;
			*d++ = y;
			*d++ = x;
			*d++ = y;
			
			x += s;
		}
		
		y -= s;
	}
}

/**
 * Load the intro sprite sheet.
 */
+ (void) loadIntroSprites {
	SprVInfo info[] = {
		{ 4, 64 },		// i, c, h, a
		{ 4, 64 },		// n, s, f, play
		{ 4, 64 },		// submit, sound, mute, f(blue)
		{ 4, 64 },		// play(blue), submit(blue), sound(blue), mute(blue)
		{ 0, 0 },
	};
			
	[GfxMgr loadAndInitializeSpriteSheet:info andId:&introId andDefs:&introDefs andRef:&introRef andGfx:@"intro.png"];
}

/**
 * Release the intro sprite sheet.
 */
+ (void) releaseIntroSprites {
	if (introRef != NULL) {
		CGImageRelease(introRef);
		introRef = NULL;
		glDeleteTextures(1, &introId);
	}
}

/**
 * Load the prefs sprite sheet.
 */
+ (void) loadPrefsSprites {
	SprVInfo info[] = {
		{ 2, 64 },		// ok (green), cancel (red)
		{ 2, 64 },		// ok (grey), cancel (grey)
		{ 0, 0 },
	};
	
	[GfxMgr loadAndInitializeSpriteSheet:info andId:&prefsId andDefs:&prefsDefs andRef:&prefsRef andGfx:@"prefs.png"];
}

/**
 * Release the prefs sprite sheet.
 */
+ (void) releasePrefsSprites {
	if (prefsRef != NULL) {
		CGImageRelease(prefsRef);
		prefsRef = NULL;
		glDeleteTextures(1, &prefsId);
	}
}

@end