//
//  FontMgr.m
//  iChains
//
//  Created by Brian Schau on 10/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "FontMgr.h"

@interface FontMgr (private)
+ (void) releaseFont:(FontInfo *) fnt;
+ (size_t) getLargestPowerOf2:(size_t) width andHeight:(size_t) height;
+ (void) loadAndBindTexture:(FontInfo *) fnt;
+ (void) loadAndInitializeFont:(FontInfo *) fnt;
@end

@implementation FontMgr
static size_t quad;
static GLfloat vertices[12] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
static GLushort verticeIdx[] = { 1, 2, 0, 3};

/**
 * Load all fonts.
 */
+ (void) loadFont:(FontInfo *) fnt {
	[self loadAndInitializeFont:fnt];
}

/**
 * Release a font.
 *
 * @param fnt Font.
 */
+ (void) releaseFont:(FontInfo *) fnt {
	if (fnt->img != nil) {
		CGImageRelease(fnt->img);
		fnt->img = nil;
		glDeleteTextures(1, &fnt->fid);
	}
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
 * @param fnt Font.
 */ 
+ (void) loadAndBindTexture:(FontInfo *) fnt {
	size_t width = CGImageGetWidth(fnt->img), height = CGImageGetHeight(fnt->img);
	quad = [self getLargestPowerOf2:width andHeight:height];
	GLubyte *data = (GLubyte *) calloc(quad * quad * 4, 1);
	CGContextRef ctx;
	GLint saveId;
	
	if (!data) {
		@throw [NSException exceptionWithName:@"loadAndBindTexture" reason:@"Cannot allocate quad" userInfo:nil];
	}
	
	CGRect bounds = CGRectMake(0.0f, 0.0f, (CGFloat)width, (CGFloat)height);
	
	ctx = CGBitmapContextCreate(data, quad, quad, 8, quad * 4, CGImageGetColorSpace(fnt->img), kCGImageAlphaPremultipliedLast);
	CGContextScaleCTM(ctx, 1.0f, -1.0f);
	bounds.size.height = bounds.size.height * -1;
	
	CGContextDrawImage(ctx, bounds, fnt->img);
	CGContextRelease(ctx);
	
	glGenTextures(1, &fnt->fid);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveId);
	glBindTexture(GL_TEXTURE_2D, fnt->fid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, quad, quad, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, saveId);
	free(data);
}

/**
 * Load and bind font.
 *
 * @param fnt FontInfo.
 */
+ (void) loadAndInitializeFont:(FontInfo *) fnt {
	int glyphCnt = fnt->lines * fnt->columns;
	
	if ((fnt->defs = malloc(glyphCnt * 8 * sizeof(GLfloat))) == nil) {
		@throw [NSException exceptionWithName:@"loadAndInitializeFont" reason:@"Cannot alloc vertices" userInfo:nil];
	}
	
	if ((fnt->img = [UIImage imageNamed:fnt->name].CGImage) == nil) {
		@throw [NSException exceptionWithName:@"loadAndInitializeFont" reason:@"Cannot load image" userInfo:nil];
	}
	
	[self loadAndBindTexture:fnt];
	GLfloat y = 1.0f, q = (GLfloat) quad, *d = fnt->defs, x;
	GLfloat s = 1.0 / (q / (GLfloat) fnt->glyphSize);
	int idx = 0, j;
	
	for (idx = 0;  idx < fnt->lines; idx++) {
		x = 0.0f;
		for (j = 0; j < fnt->columns; j++) {
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
 * Begin rendering of text. Setup states.
 */
+ (void) beginRender {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
}

/**
 * End rendering of text. Tear down states.
 */
+ (void) endRender {
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

/**
 * Render text at (x, y).
 *
 * @param fnt Font.
 * @param txt String to render.
 * @param x X coordinate.
 * @param y Y coordinate.
 */
+ (void) render:(FontInfo *) fnt andText:(const char *) txt atX:(GLfloat) x andY:(GLfloat) y {
	[FontMgr render:fnt andText:txt andCount:strlen(txt) atX:x andY:y];
}

/**
 * Render #cnt chars of text at (x, y).
 *
 * @param fnt Font.
 * @param txt String to render.
 * @param cnt Number of chars to render.
 * @param x X coordinate.
 * @param y Y coordinate.
 */
+ (void) render:(FontInfo *) fnt andText:(const char *) txt andCount:(int) cnt atX:(GLfloat) x andY:(GLfloat) y {
	int c;

	glBindTexture(GL_TEXTURE_2D, fnt->fid);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	GLfloat size = (GLfloat)fnt->glyphSize;

	y = 480.0 - y;
	vertices[1] = y;
	vertices[4] = y;
	vertices[7] = y + size;
	vertices[10] = vertices[7];

	while (cnt > 0) {
		c = toupper((int) *txt) - fnt->subtract;
		if (fnt->glyphWidths[c] > 0) {
			vertices[0] = x;
			vertices[3] = x + size;
			vertices[6] = vertices[3];		
			vertices[9] = x;
		
			glTexCoordPointer(2, GL_FLOAT, 0, &fnt->defs[c * 8]);
			glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);
		}
		
		x += fnt->glyphWidths[c];
		txt++;
		cnt--;
	}
}

/**
 * Get pixel width of string.
 *
 * @param fnt Font.
 * @param str String to measure.
 * @return width.
 */
+ (int) getStringWidth:(FontInfo *) fnt andString:(const char *) str {
	return [FontMgr getStringWidth:fnt andString:str andCount:strlen(str)];
}

/**
 * Get pixel width of #cnt chars of string.
 *
 * @param fnt Font.
 * @param str String to measure.
 * @param cnt Count.
 * @return width.
 */
+ (int) getStringWidth:(FontInfo *) fnt andString:(const char *) str andCount:(int) cnt {
	int len = 0;
	
	while ((*str) && (cnt > 0)) {
		len += fnt->glyphWidths[toupper((int) *str) - fnt->subtract];
		str++;
		cnt--;
	}
	
	return len;
}

/**
 * Fit a string in a box.
 *
 * @param fnt Font.
 * @param str String to fit.
 * @param width Width of box.
 * @return number of characters in box.
 */
+ (int) fitInBox:(FontInfo *) fnt andString:(const char *) str andWidth:(int) width {
	int len = 0;
	
	while ((*str) && (width > 0)) {
		width -= fnt->glyphWidths[toupper((int) *str) - fnt->subtract];
		len++;
		str++;
	}
	
	return len;
}

/**
 * Find out which of the strings is the longest.
 *
 * @param fnt Font.
 * @param strings - must end in NULL.
 * @return width.
 */
+ (int) getLargestWidthOfStrings:(FontInfo *) fnt, ... {
	int w = 0, x;
	va_list args;
	char *p;
	
	va_start(args, fnt);
	while ((p = (char *) va_arg(args, char *)) != NULL) {
		x = [FontMgr getStringWidth:fnt andString:p];
		if (x > w) {
			w = x;
		}
	}
	
	va_end(args);
	return w;
}

@end