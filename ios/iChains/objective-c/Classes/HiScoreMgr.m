//
//  HiScoreMgr.m
//  iChains
//
//  Created by Brian Schau on 7/15/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "HiScoreMgr.h"

@interface HiScoreMgr (private)
+ (void) showStatus:(char *) text step:(int) textStep atY:(GLfloat) y;
+ (void) centerLine:(char *) str atY:(GLfloat) y;
+ (void) submitHiScore;
+ (NSString *) urlEncodeString:(NSString *) str;
@end

@implementation HiScoreMgr
static NSString *response;
static unsigned long score;
static BOOL submittable, running, errHiScore;
static int mFade, mIdx, step, position;
static NSError *reqErr;
extern FontInfo titleFont;
extern GLfloat opaqueness[11];

/**
 * Load score from file.
 */
+ (void) loadScore {
	StateMgr *sm = [[StateMgr alloc] init];
	int version = 0;

	score = 0;
	submittable = NO;
	position = 0;
	[sm openForReading:@"iChainsHiScore.bin"];
	[sm read:&version andLength:sizeof(int)];
	if (version == 1) {
		[sm read:&submittable andLength:sizeof(BOOL)];
		[sm read:&score andLength:sizeof(unsigned long)];
		[sm read:&position andLength:sizeof(int)];
	}
	
	[sm close:NO];
	[sm release];
}

/**
 * Save score to file.
 */
+ (void) saveScore {
	StateMgr *sm = [[StateMgr alloc] init];
	int version = 1;
	
	[sm openForWriting:@"iChainsHiScore.bin"];
	[sm write:&version andLength:sizeof(int)];
	[sm write:&submittable andLength:sizeof(BOOL)];
	[sm write:&score andLength:sizeof(unsigned long)];
	[sm write:&position andLength:sizeof(int)];
	
	[sm close:NO];
	[sm release];
}

/**
 * Get position.
 *
 * @return position.
 */
+ (int) getPosition {
	return position;
}

/**
 * Get score.
 *
 * @return score.
 */
+ (unsigned long) getScore {
	return score;
}

/**
 * Add score.
 */
+ (BOOL) addScore:(unsigned long) sc {
	if (sc > score) {
		score = sc;
		submittable = YES;
		return YES;
	}
	
	return NO;
}

/**
 * Can this score be submitted?
 *
 * @return YES if submittable, NO otherwise.
 */
+ (BOOL) canSubmit {
	return submittable;
}

/**
 * Clear submittable flag.
 */
+ (void) clearSubmittable {
	submittable = NO;
}

/**
 * Setup for submit.
 *
 * @return YES if can submit, no otherwise (low score).
 */
+ (BOOL) setupForSubmit {
	if (score == 0) {
		return NO;
	}
	
	running = NO;
	mIdx = 0;
	mFade = 1;
	step = 0;
	
	[NSThread detachNewThreadSelector:@selector(engine) toTarget:self withObject:nil];
	return YES;
}

/**
 * Get error flags.
 *
 * @return error flag.
 */
+ (BOOL) getHiScoreErr {
	return errHiScore;
}

/**
 * 'run' the submit hiscore screens.
 *
 * @param state Current state.
 * @return new state.
 */
+ (int) submit:(int) state {
	GLfloat y = 150.0;
	
	if (mFade == 1) {
		mIdx++;
		if (mIdx > 9) {
			mFade = 3;
		}
	} else if (mFade == 2) {
		mIdx--;
		if (mIdx < 1) {
			if (errHiScore == NO) {
				[HiScoreMgr clearSubmittable];
			}

			if (errHiScore == YES) {
				[Modal initModal:MODALTYPEHISCORESUBMIT andNextState:STATEINTROINIT];
				return STATEMODALRUN;
			}
			
			int im;
			
			switch (position) {
				case 0:
					im = MODALTYPENORANK;
					break;
				
				case 1:
					im = MODALTYPERANK1;
					break;
				
				case 2:
					im = MODALTYPERANK2;
					break;
				
				case 3:
					im = MODALTYPERANK3;
					break;
				
				default:
					im = MODALTYPEOTHERRANK;
					break;
			}
					
			[Modal initModal:im andNextState:STATEINTROINIT];
			return STATEMODALRUN;
		}
	}
	
	[FontMgr beginRender];
	glColor4f(0.8, 0.8, 0.8, opaqueness[mIdx]);
	[Utility centerLine:"PLEASE WAIT" atY:y];

	y += 80;
	[HiScoreMgr showStatus:"SUBMITTING HI SCORE" step:0 atY:y];
	[FontMgr endRender];
	return state;
}

/**
 * Show a colorfied status.
 *
 * @param text Text to show.
 * @param textStep Step for this step.
 * @param y Y position.
 */
+ (void) showStatus:(char *) text step:(int) textStep atY:(GLfloat) y {
	if (step <= textStep) {
		glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
	} else {
		glColor4f(0.25, 0.75, 0.33, opaqueness[mIdx]);
	}

	[Utility centerLine:text atY:y];
}

/**
 * Submit 'engine'.
 */
+ (void) engine {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	running = YES;
	response = nil;
	errHiScore = NO;
	
	[HiScoreMgr submitHiScore];
	if ((reqErr == nil) && ([response characterAtIndex:0] == '+')) {
		if ([response characterAtIndex:1] != '-') {
			int pos = 0;
			
			for (int i = 1; i < [response length]; i++) {
				pos *= 10;
				pos |= (int) ([response characterAtIndex:i] - (unichar)'0');
			}
			
			if ((position == 0) || (pos <= position)) {
				position = pos;
			}
		}
		
		step++;
		[NSThread sleepForTimeInterval:1.0];
	} else {
		errHiScore = YES;
	}
	
	mFade = 2;
	[pool release];
	running = NO;
}

/**
 * Submit the hiscore to iPhoneChi.
 */
+ (void) submitHiScore {
	const char *signature;
	unsigned char md5[CC_MD5_DIGEST_LENGTH];
	unsigned char padding[8];
	int i;
	
	NSLocale *loc = [NSLocale currentLocale];
	UIDevice *dev = [UIDevice currentDevice];
	NSNumber *num = [NSNumber numberWithUnsignedLong:score];
	NSString *hemmeligt = @".K=+a2c$%RETdumt";
	
	NSString *data = [NSString stringWithFormat:@"%@%@%@%@", [loc objectForKey:NSLocaleCountryCode], [dev uniqueIdentifier], num, [dev name]];
	NSString *unHashed = [NSString stringWithFormat:@"%@%@", hemmeligt, data];
	signature = [unHashed UTF8String];
	CC_MD5(signature, strlen(signature), md5);

	for (i = 0; i < sizeof(padding); i++) {
		padding[i] = (unsigned char) random();
	}
		
	NSString *hex = [NSString stringWithFormat: @"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
					padding[0], padding[1], padding[2], padding[3], md5[0], md5[1], md5[2], md5[3],
					md5[4], md5[5], md5[6], md5[7], md5[8], md5[9], md5[10], md5[11],
					md5[12], md5[13], md5[14], md5[15], padding[4], padding[5], padding[6], padding[7]];
		
	NSString *payLoad = [NSString stringWithFormat:@"loc=%@&dev=%@&model=%@&score=%@&name=%@&sig=%@",
						 [HiScoreMgr urlEncodeString:[loc objectForKey:NSLocaleCountryCode]],
						 [HiScoreMgr urlEncodeString:[dev uniqueIdentifier]],
						 [HiScoreMgr urlEncodeString:[dev localizedModel]],
						 num,
						 [HiScoreMgr urlEncodeString:[dev name]],
						 hex];

#ifdef DEBUG
	NSString *url = @"http://127.0.0.1:93/ichains/receiver.php";
#else
	NSString *url = @"http://www.iphonechi.com/ichains/receiver.php";
#endif

	NSData *reqData = [NSData dataWithBytes:[payLoad cStringUsingEncoding:NSASCIIStringEncoding] length:[payLoad length]];
	NSMutableURLRequest *req = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:url] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:30.0];
	[req setHTTPMethod:@"POST"];
	[req setHTTPBody:reqData];

	reqErr = nil;
	NSData *resp = [NSURLConnection sendSynchronousRequest:req returningResponse:nil error:&reqErr];
	if (resp != nil) {
		response = [[[NSString alloc] initWithData:resp encoding:NSUTF8StringEncoding] autorelease];
	} else {
		response = [[[NSString alloc] initWithString:@"-No response from server"] autorelease];
	}
}

/**
 * Url encode a string.
 *
 * @param str String to URL encode.
 * @return encoded string.
 */
+ (NSString *) urlEncodeString:(NSString *) str {
	NSString *result = (NSString *) CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault,
																		   (CFStringRef)str,
                                                                           NULL,
                                                                           CFSTR(":/=,!$&'()*+;[]@#?"),
                                                                           kCFStringEncodingUTF8);
	return [result autorelease];
}

@end