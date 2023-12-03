//
//  FontMgr.h
//  iChains
//
//  Created by Brian Schau on 10/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <OpenGLES/ES1/gl.h>

@interface FontMgr : NSObject {}

typedef struct {
	NSString *name;
	int glyphSize;
	int columns;
	int lines;
	int subtract;
	int *glyphWidths;
	CGImageRef img;
	GLuint fid;
	GLfloat *defs;	
} FontInfo;

+ (void) loadFont:(FontInfo *) fnt;
+ (void) releaseFont:(FontInfo *) fnt;
+ (void) beginRender;
+ (void) endRender;
+ (void) render:(FontInfo *) fnt andText:(const char *) txt atX:(GLfloat) x andY:(GLfloat) y;
+ (void) render:(FontInfo *) fnt andText:(const char *) txt andCount:(int) cnt atX:(GLfloat) x andY:(GLfloat) y;
+ (int) getStringWidth:(FontInfo *) fnt andString:(const char *) str;
+ (int) getStringWidth:(FontInfo *) fnt andString:(const char *) str andCount:(int) cnt;
+ (int) fitInBox:(FontInfo *) fnt andString:(const char *) str andWidth:(int) width;
+ (int) getLargestWidthOfStrings:(FontInfo *) fnt, ...;

@end