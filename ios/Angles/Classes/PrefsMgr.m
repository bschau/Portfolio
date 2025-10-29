//
//  PrefsMgr.m
//  Angles
//
//  Created by Brian Schau on 10/5/09.
//  Copyright 2009 iPhoneChi. All rights reserved.
//
#import "PrefsMgr.h"

@implementation PrefsMgr

static int hiScores[5] = { 0, 0, 0, 0, 0 };
static NSString *scoreNames[5] = { SCORE_NAME1, SCORE_NAME2, SCORE_NAME3, SCORE_NAME4, SCORE_NAME5 };
static NSString *scoreValues[5] = { SCORE_VALUE1, SCORE_VALUE2, SCORE_VALUE3, SCORE_VALUE4, SCORE_VALUE5 };
static NSString *prefsFilePath;
static NSMutableDictionary *prefs;

/**
 * Load preferences and leaderboard.
 */
+ (void) loadPrefs {
	if (prefsFilePath == nil) {
		NSString *docDir = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
		prefsFilePath = [docDir stringByAppendingPathComponent:@"angles.plist"];
		
		[prefsFilePath retain];
	}
	
	if ([[NSFileManager defaultManager] fileExistsAtPath:prefsFilePath]) {
		prefs = [[NSMutableDictionary alloc] initWithContentsOfFile:prefsFilePath];
	} else {
		prefs = [[NSMutableDictionary alloc] initWithCapacity:11];
		
		[prefs setObject:@"1" forKey:REFLEX_ANGLES];
		[prefs setObject:@"bsc" forKey:SCORE_NAME1];
		[prefs setObject:@"5" forKey:SCORE_VALUE1];
		[prefs setObject:@"bsc" forKey:SCORE_NAME2];
		[prefs setObject:@"4" forKey:SCORE_VALUE2];
		[prefs setObject:@"bsc" forKey:SCORE_NAME3];
		[prefs setObject:@"3" forKey:SCORE_VALUE3];
		[prefs setObject:@"bsc" forKey:SCORE_NAME4];
		[prefs setObject:@"2" forKey:SCORE_VALUE4];
		[prefs setObject:@"bsc" forKey:SCORE_NAME5];
		[prefs setObject:@"1" forKey:SCORE_VALUE5];
	}
	
	NSString *s = [PrefsMgr get:SCORE_VALUE1];
	hiScores[0] = [s intValue];
	s = [PrefsMgr get:SCORE_VALUE2];
	hiScores[1] = [s intValue];
	s = [PrefsMgr get:SCORE_VALUE3];
	hiScores[2] = [s intValue];
	s = [PrefsMgr get:SCORE_VALUE4];
	hiScores[3] = [s intValue];
	s = [PrefsMgr get:SCORE_VALUE5];
	hiScores[4] = [s intValue];
}

/**
 * Save preferences and leaderboard.
 */
+ (void) savePrefs {
	[prefs writeToFile:prefsFilePath atomically:YES];
}

/**
 * Get the value of the key.
 *
 * @param key Key to get.
 * @return Value (or nil).
 */
+ (NSString *) get:(NSString *) key {
	return [prefs objectForKey:key];
}

/**
 * Set value of key.
 *
 * @param key Key.
 * @param value Value.
 */
+ (void) set:(NSString *) key andValue:(NSString *) value {
	[prefs setObject:value forKey:key];
}

/**
 * Did this score qualify to the leaderboard table?
 *
 * @param pts Points.
 * @return 0 - 4 if qualified (position) or -1 if outside leaderboard.
 */
+ (int) didQualify:(int) pts {
	NSString *s1, *s2;
	int pos;
	
	for (pos = 0; pos < 5; pos++) {
		if (pts > hiScores[pos]) {
			for (int i = 4; i > pos; i--) {
				hiScores[i] = hiScores[i - 1];
				s1 = scoreNames[i];
				s2 = scoreNames[i - 1];				
				[PrefsMgr set:s1 andValue:[PrefsMgr get:s2]];
				s1 = scoreValues[i];
				s2 = scoreValues[i - 1];				
				[PrefsMgr set:s1 andValue:[PrefsMgr get:s2]];
			}
			
			hiScores[pos] = pts;
			[PrefsMgr set:scoreNames[pos] andValue:@""];
			
			s1 = [[NSString alloc] initWithFormat:@"%i", pts];
			[PrefsMgr set:scoreValues[pos] andValue:s1];
			[s1 release];
			return pos;
		}
	}

	return -1;
}

/**
 * Update the name at position.
 *
 * @param name New name.
 * @param pos Position in leaderboard.
 */
+ (void) updateNameAtPos:(NSString *) name withPosition:(int) pos {
	[PrefsMgr set:scoreNames[pos] andValue:name];
	[PrefsMgr savePrefs];
}

@end
