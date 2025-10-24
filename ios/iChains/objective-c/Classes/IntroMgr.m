//
//  IntroMgr.m
//  iChains
//
//  Created by Brian Schau on 21/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "IntroMgr.h"
#include "Constants.h"

@interface IntroMgr (private)
+ (void) glSetup;
+ (void) glTeardown;
+ (void) buttonPressDown:(CGPoint *) p;
+ (void) buttonPressUp:(CGPoint *) p;
+ (void) switchScreen:(int) s;
+ (BOOL) isPressed:(GLfloat) base andTouch:(CGPoint *) p;
+ (void) drawFooter;
+ (void) drawButton:(int) buttonNo atX:(GLfloat) x;
+ (void) drawMainScreen;
@end

@implementation IntroMgr
#define SCRCREDENTIALS 0
#define SCRHISCORE (SCRCREDENTIALS + 1)
#define SCRRULES1 (SCRHISCORE + 1)
#define SCRRULES2 (SCRRULES1 + 1)
#define TIMERRESET 150
#define BUTTONWEBX 14.0
#define BUTTONSUBMITX (14.0 + ((64.0 + 12.0) * 1))
#define BUTTONPREFSX (14.0 + ((64.0 + 12.0) * 2))
#define BUTTONPLAYX (14.0 + ((64.0 + 12.0) * 3))
#define BUTTONY (480 - 4.0)
#define LOGOY (480.0 - 70.0)
#define RUNINTRO 1
#define RUNGAME 2
#define RUNSUBMITSCORE 3
#define RUNPREFS 4
extern CGPoint touchPoint;
static int buttonPressed, screen, nextScreen;
static int hFade, hIdx, fFade, fIdx, mFade, mIdx, mWait;
static int runWhat;
static int logoIdx[7] = { 0, 1, 2, 3, 0, 4, 5 };
static GLfloat logoWidth[6] = { 8.0, 26.0, 29.0, 27.0, 27.0, 27.0 };
static GLfloat vertices[12];
extern FontInfo titleFont;
extern GLfloat opaqueness[11];

/**
 * Cold boot.
 */
+ (void) coldBoot {
	hFade = 1;
	hIdx = 0;

	vertices[2] = 0.0;
	vertices[5] = 0.0;
	vertices[8] = 0.0;
	vertices[11] = 0.0;
}

/**
 * Open the Intro Manager.
 */
+ (void) initIntro {
	fFade = 1;
	fIdx = 0;
	mFade = 1;
	mIdx = 0;
	mWait = 0;
	screen = SCRCREDENTIALS;
	nextScreen = -1;
	buttonPressed = -1;
	runWhat = RUNINTRO;
}

/**
 * Run the Intro.
 *
 * @param glView GL View.
 * @return next state.
 */
+ (int) run:(GLView *) glView {
	[IntroMgr glSetup];

	if (buttonPressed == -1) {
		if ([glView getBeginTouch:&touchPoint]) {
			[IntroMgr buttonPressDown:&touchPoint];
		}
	}

	if ([glView getTouch:&touchPoint]) {
		[IntroMgr buttonPressUp:&touchPoint];
	}
	
	[IntroMgr drawFooter];
	[IntroMgr glTeardown];
	[IntroMgr drawMainScreen];
	
	if (runWhat != RUNINTRO) {
		if ((mFade == 8) && (fFade == 8)) {
			if (runWhat == RUNGAME) {
				return STATEGAMENEWLEVEL;
			} else if (runWhat == RUNPREFS) {
				return STATEPREFSINIT;
			}
			
			return STATESUBMITHISCOREINIT;
		}
	}
	
	return STATEINTRORUN;
}

/**
 * Setup OpenGL paramters.
 */
+ (void) glSetup {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
	glBindTexture(GL_TEXTURE_2D, introId);
	glColor4f(1.0, 1.0, 1.0, 1.0);
}

/**
 * Tear down OpenGL parameters.
 */
+ (void) glTeardown {
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

/**
 * Handle button press down.
 *
 * @param p CGPoint.
 */
+ (void) buttonPressDown:(CGPoint *) p {
	if ([IntroMgr isPressed:BUTTONWEBX andTouch:p]) {
		buttonPressed = 6;
	} else if ([IntroMgr isPressed:BUTTONSUBMITX andTouch:p]) {
		buttonPressed = 7;
	} else if ([IntroMgr isPressed:BUTTONPREFSX andTouch:p]) {
		buttonPressed = 8;
	} else if ([IntroMgr isPressed:BUTTONPLAYX andTouch:p]) {
		buttonPressed = 9;
	} else {
		buttonPressed = -1;
	}	
}

/**
 * Handle button press up.
 *
 * @param p CGPoint.
 */
+ (void) buttonPressUp:(CGPoint *) p {
	if (([IntroMgr isPressed:BUTTONWEBX andTouch:p]) && (buttonPressed == 6)) {
		[SoundMgr playEffect:SOUNDBUTTONPRESS];
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://www.iphonechi.com"]];
	} else if (([IntroMgr isPressed:BUTTONSUBMITX andTouch:p]) && (buttonPressed == 7)) {
		[IntroMgr switchScreen:RUNSUBMITSCORE];
	} else if (([IntroMgr isPressed:BUTTONPREFSX andTouch:p]) && (buttonPressed == 8)) {
		[IntroMgr switchScreen:RUNPREFS];		
	} else if (([IntroMgr isPressed:BUTTONPLAYX andTouch:p]) && (buttonPressed == 9)) {
		[IntroMgr switchScreen:RUNGAME];
		hFade = 2;
	}
	
	buttonPressed = -1;
}

/**
 * Switch to another main screen.
 *
 * @param s Screen to switch to.
 */
+ (void) switchScreen:(int) s {
	[SoundMgr playEffect:SOUNDBUTTONPRESS];
	fFade = 2;
	mFade = 2;
	runWhat = s;
}

/**
 * See if button is pressed.
 *
 * @param base Button x position.
 * @param p Touch.
 * @return YES if pressed, NO otherwise.
 */
+ (BOOL) isPressed:(GLfloat) base andTouch:(CGPoint *) p {
	if ((p->x >= base) && (p->x <= (base + 64.0)) && (p->y >= (BUTTONY - 40.0)) && (p->y <= BUTTONY)) {
		return YES;
	}

	return NO;
}
		
/**
 * Draw the header.
 */
+ (void) drawHeader {
	GLfloat x = 77.0;
	int i, idx;

	[IntroMgr glSetup];
	
	if (hFade == 1) {
		hIdx++;
		if (hIdx > 9) {
			hFade = 0;
		}
	} else if (hFade == 2) {
		hIdx--;
		if (hIdx < 1) {
			hFade = 8;
		}
	}
	
	if (hFade) {
		glColor4f(opaqueness[hIdx], opaqueness[hIdx], opaqueness[hIdx], opaqueness[hIdx]);		
	}
	
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	for (i = 0; i < 7; i++) {
		idx = logoIdx[i];
		vertices[0] = x;
		vertices[1] = LOGOY;
		vertices[3] = vertices[0] + 64.0;	// width
		vertices[4] = vertices[1];
		vertices[6] = vertices[3];
		vertices[7] = vertices[1] + 64.0;	// height
		vertices[9] = vertices[0];
		vertices[10] = vertices[7];

		glTexCoordPointer(2, GL_FLOAT, 0, &introDefs[idx * 8]);
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);
		
		x += logoWidth[idx] + 2.0;
	}
	
	[IntroMgr glTeardown];
}

/**
 * Draw the footer.
 */
+ (void) drawFooter {
	if (fFade == 1) {
		fIdx++;
		if (fIdx > 9) {
			fFade = 0;
		}
	} else if (fFade == 2) {
		fIdx--;
		if (fIdx < 1) {
			fFade = 8;
		}
	}
	
	if (fFade) {
		glColor4f(opaqueness[fIdx], opaqueness[fIdx], opaqueness[fIdx], opaqueness[fIdx]);		
	}
	
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	[IntroMgr drawButton:6 atX:BUTTONWEBX];
	[IntroMgr drawButton:7 atX:BUTTONSUBMITX];
	[IntroMgr drawButton:8 atX:BUTTONPREFSX];
	[IntroMgr drawButton:9 atX:BUTTONPLAYX];
}

/**
 * Draw button no at position x.
 *
 * @param buttonNo Button number.
 * @param x X position.
 */
+ (void) drawButton:(int) buttonNo atX:(GLfloat) x {
	if (buttonPressed == buttonNo) {
		buttonNo += 4;
	}
	
	vertices[0] = x;
	vertices[1] = -20.0;
	vertices[3] = vertices[0] + 64.0;	// width
	vertices[4] = vertices[1];
	vertices[6] = vertices[3];
	vertices[7] = vertices[1] + 64.0;	// height
	vertices[9] = vertices[0];
	vertices[10] = vertices[7];
		
	glTexCoordPointer(2, GL_FLOAT, 0, &introDefs[buttonNo * 8]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);
}

/**
 * Draw "main" screen estate.
 */
+ (void) drawMainScreen {
	GLfloat y;
	BOOL canSubmit;
	int pos;
	
	if (mFade == 1) {
		mIdx++;
		if (mIdx > 9) {
			mFade = 3;
			mWait = 0;
		}
	} else if (mFade == 2) {
		mIdx--;
		if (mIdx < 1) {
			if (runWhat != RUNINTRO) {
				mFade = 8;
			} else {
				mFade = 1;
				if (nextScreen > -1) {
					screen = nextScreen;
				} else {
					if (screen == SCRRULES2) {
						screen = SCRCREDENTIALS;
					} else if (screen < SCRRULES2) {
						screen++;
					}
				}
			}
		}
	} else if (mFade == 3) {
		mWait++;
		if (mWait > 150) {
			mWait = 0;
			mFade = 2;
		}
	}
		
	[FontMgr beginRender];
	switch (screen) {
		case SCRCREDENTIALS:
			glColor4f(0.8, 0.8, 0.8, opaqueness[mIdx]);
			[Utility centerLine:"CODING AND GRAPHICS" atY:150.0];
			[Utility centerLine:"MUSIC" atY:250.0];
			[Utility centerLine:"% IPHONECHI 2009, V3.0" atY:370.0];
			glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
			[Utility centerLine:"BRIAN SCHAU" atY:190.0];
			[Utility centerLine:"WILLIAM USHER" atY:290.0];
			break;

		case SCRHISCORE:
			canSubmit = [HiScoreMgr canSubmit];
			pos = [HiScoreMgr getPosition];
			
			if (canSubmit == YES) {
				y = 170.0;
			} else {
				if (pos > 0) {
					y = 170.0;
				} else {
					y = 230.0;
				}
			}
			
			glColor4f(0.8, 0.8, 0.8, opaqueness[mIdx]);
			[Utility centerLine:"PERSONAL BEST" atY:y];
			
			y += 40.0;
			glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
			[Utility centerLine:[NumberFormatter format:[HiScoreMgr getScore]] atY:y];
			
			if (canSubmit == YES) {
				y += 80.0;
				glColor4f(0.75, 0.33, 0.25, opaqueness[mIdx]);
				[Utility centerLine:"COMPETE AGAINST THE WORLD!" atY:y];
				
				y += 40.0;
				glColor4f(0.25, 0.75, 0.33, opaqueness[mIdx]);
				[Utility centerLine:"SUBMIT THIS SCORE" atY:y];
			} else {				
				if (pos > 0) {
					y += 80.0;

					glColor4f(0.25, 0.75, 0.33, opaqueness[mIdx]);
					[Utility centerLine:"RANK ON TOP 100" atY:y];
					
					y += 40.0;
					if (pos == 1) {
						glColor4f(0.85, 0.85, 0.10, opaqueness[mIdx]);
						[Utility centerLine:"1ST PLACE!" atY:y];
					} else if (pos == 2) {
						glColor4f(0.75, 0.75, 0.75, opaqueness[mIdx]);
						[Utility centerLine:"2ND PLACE!" atY:y];
					} else if (pos == 3) {
						glColor4f(0.65, 0.49, 0.24, opaqueness[mIdx]);
						[Utility centerLine:"3RD PLACE!" atY:y];
					} else {
						char buffer[40];
						
						glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
						snprintf(buffer, sizeof(buffer), "%iTH PLACE ...", pos);
						[Utility centerLine:buffer atY:y];		
					}
				}
			}
			break;

		case SCRRULES1:
			y = 150.0;
			glColor4f(0.8, 0.8, 0.8, opaqueness[mIdx]);
			[Utility centerLine:"RULES" atY:y];
			
			glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
			y += 40.0;
			[Utility centerLine:"SCORE POINTS BY CREATING" atY:y];
			y += 40.0;
			[Utility centerLine:"LONGER AND LONGER CHAINS" atY:y];
			y += 40.0;
			[Utility centerLine:"OF EXPLODING BALLS." atY:y];
			y += 40.0;
			[Utility centerLine:"TAP ANY WHERE TO START" atY:y];
			y += 40.0;
			[Utility centerLine:"THE INITIAL CHAIN." atY:y];
			break;			

		case SCRRULES2:
			y = 150.0;
			glColor4f(0.8, 0.8, 0.8, opaqueness[mIdx]);
			[Utility centerLine:"RULES" atY:y];
			
			glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
			y += 40.0;
			[Utility centerLine:"EACH LEVEL HAS A NUMBER" atY:y];
			y += 40.0;
			[Utility centerLine:"OF BALLS WHICH MUST BE" atY:y];
			y += 40.0;
			[Utility centerLine:"REMOVED BEFORE GOING ON" atY:y];
			y += 40.0;
			[Utility centerLine:"TO THE NEXT LEVEL." atY:y];
			y += 40.0;
			[Utility centerLine:"THERE ARE 12 LEVELS." atY:y];
			break;
	}
	
	[FontMgr endRender];
}

@end
