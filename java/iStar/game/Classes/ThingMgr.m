//
//  ThingMgr.m
//  iStar
//
//  Created by Brian Schau on 5/18/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import "ThingMgr.h"

@implementation ThingMgr
#define MAXTHINGS 20
static id *things;
static int thingsCnt;

/**
 * Initialize variables.
 */
+ (void)classInit {
	things = malloc(sizeof(id) * MAXTHINGS);
	if (!things) {
		@throw [NSException exceptionWithName:@"classInit" reason:@"Cannot allocate things array" userInfo:nil];
	}
	
	for (int i = 0; i < MAXTHINGS; i++) {
		things[i] = nil;
	}
}

/**
 * Reset - deallocate all and zero out variables.
 */
+ (void)reset {
	for (int i = 0; i < MAXTHINGS; i++) {
		if (things[i] != nil) {
			[things[i] release];
		}
		
		things[i] = nil;
	}
	
	thingsCnt = 0;
}

/**
 * Add a thing to the things list.  If the list is full, items will be silently discarded.
 *
 * @param t Thing.
 */
+ (void)add:(id)t {
	if (thingsCnt < MAXTHINGS) {
		things[thingsCnt] = t;
		thingsCnt++;
	}
}

/**
 * Draw things.
 */
+ (void)draw {
	for (int i = 0; i < thingsCnt; i++) {
		[things[i] draw];
	}
}

/**
 * Get number of things.
 *
 * @return thingsCnt.
 */
+ (int)getNumThings {
	return thingsCnt;
}

/**
 * Get thing at index idx.
 *
 * @param idx Index.
 * @return thing.
 */
+ (id)get:(int)idx {
	return things[idx];
}
	
@end
