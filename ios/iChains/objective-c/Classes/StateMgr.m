//
//  StateMgr.m
//  iChains
//
//  Created by Brian Schau on 7/21/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "StateMgr.h"

@interface StateMgr (private)
- (void) openStateFile:(NSString *) file andMode:(char *) mode;
@end

@implementation StateMgr

/**
 * Open state file (Documents/${file}).
 *
 * @param file File name.
 * @param mode File open mode.
 */
- (void) openStateFile:(NSString *) file andMode:(char *) mode {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *nsAppFile = [documentsDirectory stringByAppendingPathComponent:file];

	stateErr = NO;
	stateFile = [nsAppFile cStringUsingEncoding:[NSString defaultCStringEncoding]];	
	fp = fopen(stateFile, mode);
}

/**
 * Open a state file for reading.
 *
 * @param file File.
 */
- (void) openForReading:(NSString *) file {
	[self openStateFile:file andMode:"r"];
}
			   
/**
 * Open a state file for writing.
 *
 * @param file File.
 */
- (void) openForWriting:(NSString *) file {
	[self openStateFile:file andMode:"w"];
}

/**
 * Close a state file.
 *
 * @param delete Delete state file?
 */
- (void) close:(BOOL) delete {
	if (fp) {
		fclose(fp);
		fp = NULL;
	}
	
	if (delete == YES) {
		unlink(stateFile);
	}
}

/**
 * Write len bytes to stateFile from src.
 *
 * @param src Source data.
 * @param len Length of source data.
 */
- (void) write:(void *) src andLength:(int) len {
	if ((fp == NULL) || (stateErr == YES)) {
		return;
	}
	
	if (fwrite(src, len, 1, fp) != 1) {
		stateErr = YES;
	}	
}

/**
 * Read len bytes from stateFile to dst.
 *
 * @param dst Destination area.
 * @param len No# of bytes to read.
 */
- (void) read:(void *) dst andLength:(int) len {
	if ((fp == NULL) || (stateErr == YES)) {
		return;
	}
	
	if (fread(dst, len, 1, fp) != 1) {
		stateErr = YES;
	}
}

/**
 * Return error flag.
 *
 * @return error.
 */
- (BOOL) getError {
	return stateErr;
}

@end