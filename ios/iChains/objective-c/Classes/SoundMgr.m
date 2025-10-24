//
//  SoundMgr.m
//  iChains
//
//  Created by Brian Schau on 7/9/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "SoundMgr.h"

@interface SoundMgr (private)
+ (void) initAndPlaySound:(int) start andLength:(int) length;
+ (void) audioPlayerDidFinishPlaying:(AVAudioPlayer *) player successfully:(BOOL) flag;
+ (void) engine;
@end

@implementation SoundMgr
#define EBUTTONPRESSIDX 0
#define EBUTTONPRESSLEN 1
#define EFIREIDX 1
#define EFIRELEN 1
#define EEXPLOSIONIDX (EFIREIDX + EBUTTONPRESSLEN)
#define EEXPLOSIONLEN 14

static UInt32 otherMusicPlaying = 0;
static AVAudioPlayer *bgmPlayer = nil;
static NSString *bgmPath = nil;
static BOOL muted, runThread, running;
static AVAudioPlayer **effects = nil;
static int noOfEffects, triggerSound;
static struct {
	int length;
	NSString *file;
	NSString *type;
	NSString *path;
	NSData *data;
} effectsArray[] = {
{ EBUTTONPRESSLEN, @"buttonpress", @"caf", nil, nil },
{ EFIRELEN, @"fire", @"caf", nil, nil },
{ EEXPLOSIONLEN, @"explosion", @"caf", nil, nil },
{ 0, nil, nil, nil, nil },
};

/**
 * Open the sound manager.
 */
+ (void) open {
	UInt32 size = sizeof(otherMusicPlaying);
	int idx, i, j, len;
	NSString *path;
	
	AudioSessionInitialize(NULL, NULL, NULL, NULL);
	AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &size, &otherMusicPlaying);

	if (otherMusicPlaying) {
		return;
	}
	
	noOfEffects = 0;
	for (i = 0; effectsArray[i].length; i++) {
		noOfEffects += effectsArray[i].length;
	}

	if ((effects = malloc(sizeof(AVAudioPlayer *) * noOfEffects)) == nil) {
		otherMusicPlaying = 1;
		return;
	}
	
	for (i = 0; i < noOfEffects; i++) {
		effects[i] = nil;
	}
	
	idx = 0;
	for (i = 0; effectsArray[i].length; i++) {
		path = [[NSBundle mainBundle] pathForResource:effectsArray[i].file ofType:effectsArray[i].type inDirectory:@"/"];
		effectsArray[i].path = path;
		len = effectsArray[i].length;
		NSData *data = [[NSData alloc] initWithContentsOfFile:path];
		effectsArray[i].data = data;
		if (!data) {
			otherMusicPlaying = 1;
			return;
		}

		for (j = 0; j < len; j++) {
			effects[idx] = [[AVAudioPlayer alloc] initWithData:data error:nil];
			
			if (effects[idx]) {
				[effects[idx] prepareToPlay];
				[effects[idx] setDelegate:self];
				[effects[idx] setVolume:1.0];
				[effects[idx] setNumberOfLoops:0];
			} else {
				otherMusicPlaying = 1;
				return;
			}
			idx++;
		}		
	}

	runThread = YES;
	triggerSound = -1;
	[NSThread detachNewThreadSelector:@selector(engine) toTarget:self withObject:nil];
}

/**
 * Close the sound manager.
 */
+ (void) close {
	int i;

	runThread = NO;
	
	[bgmPlayer stop];
	[bgmPlayer release];
	[bgmPath release];
	
	while (running == YES) {
		sleep(1);
	}
	
	if (effects) {
		for (i = 0; i < noOfEffects; i++) {
			if (effects[i] != nil) {
				[effects[i] stop];
				[effects[i] release];
			}
		}
		
		free(effects);
	}
	
	for (i = 0; effectsArray[i].length; i++) {
		[effectsArray[i].path release];
		[effectsArray[i].data release];
	}
}

/**
 * Play background music.
 *
 * @param startMuted Start in muted state?
 */
+ (void) startBackgroundMusic:(BOOL) startMuted {
	if (!otherMusicPlaying) {
		bgmPath = [[NSBundle mainBundle] pathForResource:@"TranquilTranceExpress" ofType:@"mp3" inDirectory:@"/"];
		if (bgmPath) {
			bgmPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:bgmPath] error:nil];
			
			if (bgmPlayer) {
				muted = NO;
				[bgmPlayer setVolume:1.0];
				[bgmPlayer setNumberOfLoops:-1];
				[bgmPlayer setDelegate:self];
				if ((muted = startMuted) == YES) {
					[bgmPlayer pause];
				} else {
					[bgmPlayer play];
				}
			}
		}
	}
}

/**
 * Unmute.
 */
+ (void) unmute {
	if ((!otherMusicPlaying) && (bgmPlayer)) {
		[bgmPlayer play];
		muted = NO;
	}
}

/**
 * Mute.
 */
+ (void) mute {
	if ((!otherMusicPlaying) && (bgmPlayer)) {
		[bgmPlayer pause];
		muted = YES;
	}
}

/**
 * Play sound effect (if possible).
 *
 * @param name Sound effect name.
 */
+ (void) playEffect:(int) name {
	triggerSound = name;
}

/**
 * Initialize and play sound.
 *
 * @param start Start index into effects array.
 * @param length Number of available sound slots.
 * @param resource Resource to play.
 * @param type Type of resource.
 */
+ (void) initAndPlaySound:(int) start andLength:(int) length {
	if (!muted) {
		for (int i = start; i < (start + length); i++) {
			if (!effects[i].playing) {
				effects[i].currentTime = 0.0;
				[effects[i] play];
				break;
			}
		}
	}
}

/**
 * Audio Player call back.
 */
+ (void) audioPlayerDidFinishPlaying:(AVAudioPlayer *) player successfully:(BOOL) flag {}

/**
 * Sound 'engine'.
 */
+ (void) engine {
	NSAutoreleasePool *pool;
	int s;

	running = YES;
	pool = [[NSAutoreleasePool alloc] init];
	
	while (runThread) {
		[pool drain];
		s = triggerSound;
		triggerSound = -1;
		switch (s) {
			case SOUNDBUTTONPRESS:
				[SoundMgr initAndPlaySound:EBUTTONPRESSIDX andLength:EBUTTONPRESSLEN];
				break;

			case SOUNDEXPLOSION:
				[SoundMgr initAndPlaySound:EEXPLOSIONIDX andLength:EEXPLOSIONLEN];
				break;

			case SOUNDFIRE:
				[SoundMgr initAndPlaySound:EFIREIDX andLength:EFIRELEN];
				break;

			default:
				usleep(200000);
				break;
		}
	}

	[pool release];
	running = NO;
}

@end