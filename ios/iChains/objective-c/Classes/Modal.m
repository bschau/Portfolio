//
//  Modal.m
//  iChains
//
//  Created by Brian Schau on 8/11/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "Modal.h"

@interface Modal (private)
+ (GLfloat) renderLowScore:(GLfloat) y;
+ (GLfloat) renderHiScoreSubmit:(GLfloat) y;
+ (GLfloat) renderMemoryWarning:(GLfloat) y;
+ (GLfloat) renderRank:(GLfloat) y;
@end

@implementation Modal
extern CGPoint touchPoint;
static int mFade, mIdx, mType, nextState;
static GLfloat msgX;
static BOOL canTap;
extern FontInfo titleFont;
extern GLfloat opaqueness[11];
static char *msg = NULL;
static char ranked[40];
static char *lsMsg1 = "YOU MUST PLAY THE GAME AT";
static char *lsMsg2 = "LEAST ONCE BEFORE TRYING TO";
static char *lsMsg3 = "SUBMIT YOUR HI SCORE.";
static char *mwMsg1 = "A LOW MEMORY CONDITION HAS";
static char *mwMsg2 = "BEEN DETECTED. IF YOU WERE";
static char *mwMsg3 = "PLAYING AN ATTEMPT TO SAVE";
static char *mwMsg4 = "THE GAME HAS BEEN MADE.";
static char *rText = "YOUR RANK ON TOP 100";
static char *rRank1 = "1ST PLACE!";
static char *rRank2 = "2ND PLACE!";
static char *rRank3 = "3RD PLACE!";
static char *rOutside = "NOT ON LIST :-(";

/**
 * Initialize the modal screen.
 *
 * @param type Type of modal screen.
 */
+ (void) initModal:(int) type andNextState:(int) ns {
	int w = 0;
	
	mType = type;
	nextState = ns;
	switch (type) {
		case MODALTYPELOWSCORE:
			w = [FontMgr getLargestWidthOfStrings:&titleFont, lsMsg1, lsMsg2, lsMsg3, NULL];
			break;

		case MODALTYPEHISCORESUBMIT:
			if ([HiScoreMgr getHiScoreErr] == YES) {
				msg = "FAILED TO SUBMIT HISCORE";
			} else {
				msg = NULL;
			}
			w = [FontMgr getStringWidth:&titleFont andString:msg];
			break;

		case MODALTYPEMEMORYWARNING:
			w = [FontMgr getLargestWidthOfStrings:&titleFont, mwMsg1, mwMsg2, mwMsg3, mwMsg4, NULL];
			break;

		case MODALTYPENORANK:
			w = [FontMgr getLargestWidthOfStrings:&titleFont, rText, rOutside, NULL];
			break;

		case MODALTYPERANK1:
			w = [FontMgr getLargestWidthOfStrings:&titleFont, rText, rRank1, NULL];
			break;

		case MODALTYPERANK2:
			w = [FontMgr getLargestWidthOfStrings:&titleFont, rText, rRank2, NULL];
			break;

		case MODALTYPERANK3:
			w = [FontMgr getLargestWidthOfStrings:&titleFont, rText, rRank3, NULL];
			break;

		case MODALTYPEOTHERRANK:
			snprintf(ranked, sizeof(ranked), "%iTH PLACE ...", [HiScoreMgr getPosition]);
			w = [FontMgr getLargestWidthOfStrings:&titleFont, rText, ranked, NULL];
			break;
	}
	
	msgX = (GLfloat) ((320 - w) / 2);
	mFade = 1;
	mIdx = 0;
	canTap = YES;
}

/**
 * Run the modal screen.
 *
 * @param glView GL View.
 * @return next state.
 */
+ (int) run:(GLView *) glView {
	GLfloat y = 0.0;

	switch (mType) {
		case MODALTYPEMEMORYWARNING:
			y = 150.0;
			break;

		case MODALTYPELOWSCORE:
			y = 170.0;
			break;

		case MODALTYPENORANK:
		case MODALTYPERANK1:
		case MODALTYPERANK2:
		case MODALTYPERANK3:
		case MODALTYPEOTHERRANK:
			y = 190.0;
			break;

		default:
			y = 210.0;
			break;
			
	}
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	if (canTap == YES) {
		if ([glView getTouch:&touchPoint]) {
			[SoundMgr playEffect:SOUNDBUTTONPRESS];
			mFade = 2;
			canTap = NO;
		}
	}
	
	if (mFade == 1) {
		mIdx++;
		if (mIdx > 9) {
			mFade = 8;
		}
	} else if ((mFade == 2) || (mFade == 3)) {
		mIdx--;
		if (mIdx < 1) {
			return nextState;
		}
	}
	
	if (mFade) {
		glColor4f(opaqueness[mIdx], opaqueness[mIdx], opaqueness[mIdx], opaqueness[mIdx]);		
	}

	[FontMgr beginRender];

	switch (mType) {
		case MODALTYPELOWSCORE:
			y = [Modal renderLowScore:y];
			break;

		case MODALTYPEHISCORESUBMIT:
			y = [Modal renderHiScoreSubmit:y];
			break;

		case MODALTYPENORANK:
		case MODALTYPERANK1:
		case MODALTYPERANK2:
		case MODALTYPERANK3:
		case MODALTYPEOTHERRANK:
			y = [Modal renderRank:y];
			break;			

		default:
			y = [Modal renderMemoryWarning:y];
			break;
	}
	
	y += 80.0;
	glColor4f(0.75, 0.75, 0.33, opaqueness[mIdx]);
	if (mType == MODALTYPEMEMORYWARNING) {
		[Utility centerLine:"TAP TO EXIT" atY:y];
	} else {
		[Utility centerLine:"TAP TO CONTINUE" atY:y];
	}
	
	[FontMgr endRender];
	return STATEMODALRUN;
}

/**
 * Render the Low Score help screen.
 *
 * @param Y Y position.
 * @return last Y position.
 */
+ (GLfloat) renderLowScore:(GLfloat) y {
	glColor4f(0.8, 0.8, 0.8, opaqueness[mIdx]);
	[Utility centerLine:"CANNOT SUBMIT" atY:y];
	
	glColor4f(0.75, 0.33, 0.25, opaqueness[mIdx]);
	
	y += 80.0;
	[FontMgr render:&titleFont andText:lsMsg1 atX:msgX andY:y];
	y += 40.0;
	[FontMgr render:&titleFont andText:lsMsg2 atX:msgX andY:y];
	y += 40.0;
	[FontMgr render:&titleFont andText:lsMsg3 atX:msgX andY:y];
	
	return y;
}

/**
 * Render the Hi Score submit status screen.
 *
 * @param Y Y position.
 * @return last Y position.
 */
+ (GLfloat) renderHiScoreSubmit:(GLfloat) y {
	glColor4f(0.8, 0.8, 0.8, opaqueness[mIdx]);
	[Utility centerLine:"CANNOT SUBMIT" atY:y];

	if (msg) {
		glColor4f(0.75, 0.33, 0.25, opaqueness[mIdx]);
	} else {
		glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
	}
	
	y += 80.0;
	[FontMgr render:&titleFont andText:msg atX:msgX andY:y];
	
	return y;
}

/**
 * Render the Memory Warning screen.
 *
 * @param Y Y position.
 * @return last Y position.
 */
+ (GLfloat) renderMemoryWarning:(GLfloat) y {
	glColor4f(0.8, 0.8, 0.8, opaqueness[mIdx]);
	[Utility centerLine:"MEMORY WARNING" atY:y];
	
	glColor4f(0.75, 0.33, 0.25, opaqueness[mIdx]);
	
	y += 80.0;
	[FontMgr render:&titleFont andText:mwMsg1 atX:msgX andY:y];
	y += 40.0;
	[FontMgr render:&titleFont andText:mwMsg2 atX:msgX andY:y];
	y += 40.0;
	[FontMgr render:&titleFont andText:mwMsg3 atX:msgX andY:y];
	y += 40.0;
	[FontMgr render:&titleFont andText:mwMsg4 atX:msgX andY:y];
	
	return y;
}

/**
 * Render rank on Top 100 screen.
 *
 * @param Y Y position.
 * @return last Y position.
 */
+ (GLfloat) renderRank:(GLfloat) y {
	glColor4f(0.25, 0.75, 0.33, opaqueness[mIdx]);
	[Utility centerLine:rText atY:y];
	
	y += 40.0;
	if (mType == MODALTYPENORANK) {
		glColor4f(0.85, 0.85, 0.10, opaqueness[mIdx]);
		[Utility centerLine:rOutside atY:y];
	} else if (mType == MODALTYPERANK1) {
		glColor4f(0.85, 0.85, 0.10, opaqueness[mIdx]);
		[Utility centerLine:rRank1 atY:y];
	} else if (mType == MODALTYPERANK2) {
		glColor4f(0.75, 0.75, 0.75, opaqueness[mIdx]);
		[Utility centerLine:rRank2 atY:y];
	} else if (mType == MODALTYPERANK3) {
		glColor4f(0.65, 0.49, 0.24, opaqueness[mIdx]);
		[Utility centerLine:rRank3 atY:y];
	} else {
		glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
		[Utility centerLine:ranked atY:y];
	}
	
	return y;
}

@end