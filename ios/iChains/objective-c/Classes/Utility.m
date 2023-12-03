//
//  Utility.m
//  iChains
//
//  Created by Brian Schau on 8/10/09.
//  Copyright 2009 iPhoneChi.com. All rights reserved.
//
#import "Utility.h"

@implementation Utility
static char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
extern FontInfo titleFont;

/**
 * Draw a line centered.
 *
 * @param str String to render.
 * @param y Y position.
 */
+ (void) centerLine:(char *) str atY:(GLfloat) y {
	[FontMgr render:&titleFont andText:str atX:((320.0 - [FontMgr getStringWidth:&titleFont andString:str]) / 2.0) andY:y];	
}

/**
 * BASE64 encode a buffer.
 *
 * @param src Source data to encode.
 * @return string with encoded data - caller must release this string.
 */
+ (NSString *) base64encode:(NSString *) src {
	NSData *encodeData = [src dataUsingEncoding:NSUTF8StringEncoding];
	char *srcData = (char *) [encodeData bytes];
	int sLen = [encodeData length], len;
	unsigned int t, b;
	unsigned long int s;
	char *dst, *d;

	len = ((sLen + 3 - (sLen % 3)) /3 ) * 4;

	if ((dst = (char *) malloc(len + 1)) == NULL) {
		return nil;
	}
	
	memset(dst, 0, len + 1);
	
	d = dst;
    for (t = 0; t < sLen; t += 3) {
		s = 0;
		for (b = 0; (b < 3) && ((t + b) < sLen); ++b) {
			s <<= 8;
			s |= *(srcData + t + b) & 0xff;
		}
		
		s <<= (6 - ((8 * b) % 6)) % 6;

		*d = '=';
		*(d + 1) = '=';
		*(d + 2) = '=';
		*(d + 3) = '=';

		switch (b) {
			case 3:
				*(d + 3) = base64[s & 0x3f];
				s >>= 6;

			case 2:
				*(d + 2) = base64[s & 0x3f];
				s >>= 6;

			case 1:
				*(d + 1) = base64[s & 0x3f];
				s >>= 6;
				*d = base64[s & 0x3f];
		}
		
		d += 4;
    }
	
	*d = 0;
	
	return [NSString stringWithCString:dst length:strlen(dst)];	
}

@end