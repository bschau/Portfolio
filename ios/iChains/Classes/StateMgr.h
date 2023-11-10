//
//  StateMgr.h
//  iChains
//
//  Created by Brian Schau on 7/21/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <Foundation/Foundation.h>

@interface StateMgr : NSObject {
	BOOL stateErr;
	const char *stateFile;
	FILE *fp;
}

- (void) openForReading:(NSString *) file;
- (void) openForWriting:(NSString *) file;
- (void) close:(BOOL) delete;
- (void) write:(void *) src andLength:(int) len;
- (void) read:(void *) dst andLength:(int) len;
- (BOOL) getError;

@end