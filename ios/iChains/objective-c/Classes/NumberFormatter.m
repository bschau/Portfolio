//
//  NumberFormatter.m
//  iChains
//
//  Created by Brian Schau on 7/23/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "NumberFormatter.h"

@implementation NumberFormatter
#define BUFLEN 50
static char buffer[BUFLEN];
static char separator;

/**
 * Open the number formatter.
 */
+ (void) open {
	NSNumberFormatter *fmt = [[NSNumberFormatter alloc] init];
	[fmt setLocale:[NSLocale currentLocale]];
	NSString *sep = [fmt groupingSeparator];
	const char *c = [sep cStringUsingEncoding:NSASCIIStringEncoding];
	
	separator = *c;
	
	[fmt release];
}

/**
 * Format a number into the default buffer.
 *
 * @param n Number to format.
 * @return buffer with formatted number.
 */
+ (char *) format:(unsigned long) n {
	char *p = &buffer[BUFLEN - 1];
	int s = 0;

	*p = 0;
	do {
		*--p = (n % 10) + '0';
		n /= 10;

		s++;
		if ((s == 3) && (n)) {
			*--p = separator;
			s = 0;
		}
	} while (n);
	
	return p;
}

/**
 * Format a number into the user supplied buffer.
 *
 * @param n Number to format.
 * @return buffer with formatted number.
 */
+ (char *) format:(unsigned long) n andDst:(char *) dst {
	char *d = [NumberFormatter format:n];
	
	memmove(dst, d, strlen(d) + 1);
	return dst;
}

@end