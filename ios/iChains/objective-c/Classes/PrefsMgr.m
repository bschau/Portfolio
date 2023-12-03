//
//  PrefsMgr.m
//  iChains
//
//  Created by Brian Schau on 7/28/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "PrefsMgr.h"
#include "Constants.h"

@interface PrefsMgr (private)
+ (void) setDefaults;
+ (void) buttonPressDown:(CGPoint *) p;
+ (void) buttonPressUp:(CGPoint *) p;
+ (void) switchScreen:(int) s;
+ (BOOL) isPressed:(GLfloat) base andTouch:(CGPoint *) p;
+ (BOOL) areaHitTest:(GLfloat) x andY:(GLfloat) y andWidth:(GLfloat) w andHeight:(GLfloat) h andTouch:(CGPoint *) p;
+ (void) drawButtons;
+ (void) drawButton:(int) buttonNo atX:(GLfloat) x;
+ (int) drawMainScreen;
+ (void) showCheckbox:(int) value andString:(char *) string atX:(GLfloat) x andY:(GLfloat) y;
@end

@implementation PrefsMgr
#define TIMERRESET 150
#define BUTTONOKX (320.0 - 64.0)
#define BUTTONCANCELX 0.0
#define BUTTONY (480.0 - 4.0)
#define TXTPREFERENCESY 160.0
#define CHKMUTESOUNDY (TXTPREFERENCESY + 40.0)
extern CGPoint touchPoint;
static int sMuteSound;
static int buttonPressed, muteSound;
static int mFade, mIdx, mWait, chkBoxX;
static GLfloat vertices[12];
static GLfloat box[24];

extern FontInfo titleFont;
extern GLfloat opaqueness[11];

/**
 * Open the Preferences Manager.
 */
+ (void) initPrefs {
	mFade = 1;
	mIdx = 0;
	mWait = 0;
	buttonPressed = -1;
	vertices[2] = 0.0;
	vertices[5] = 0.0;
	vertices[8] = 0.0;
	vertices[11] = 0.0;
	box[2] = 0.0;
	box[5] = 0.0;
	box[8] = 0.0;
	box[11] = 0.0;
	box[14] = 0.0;
	box[17] = 0.0;
	box[20] = 0.0;
	box[23] = 0.0;
	[PrefsMgr setDefaults];

	chkBoxX = (320 - [FontMgr getStringWidth:&titleFont andString:"MUTE SOUND"] - 32) / 2;
}

/**
 * Set the default values.
 */
+ (void) setDefaults {
	muteSound = sMuteSound;
}

/**
 * Run the Preferences Manager.
 *
 * @param glView GL View.
 * @return next state.
 */
+ (int) run:(GLView *) glView {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
	glBindTexture(GL_TEXTURE_2D, prefsId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	if (buttonPressed == -1) {
		if ([glView getBeginTouch:&touchPoint]) {
			[PrefsMgr buttonPressDown:&touchPoint];
		}
	}
	
	if ([glView getTouch:&touchPoint]) {
		[PrefsMgr buttonPressUp:&touchPoint];
	}
	
	return [PrefsMgr drawMainScreen];	
}

/**
 * Handle button press down.
 *
 * @param p CGPoint.
 */
+ (void) buttonPressDown:(CGPoint *) p {
	if ([PrefsMgr isPressed:BUTTONOKX andTouch:p]) {
		buttonPressed = 0;
	} else if ([PrefsMgr isPressed:BUTTONCANCELX andTouch:p]) {
		buttonPressed = 1;
	} else if ([PrefsMgr areaHitTest:(GLfloat) chkBoxX andY:CHKMUTESOUNDY andWidth:32.0 andHeight:32.0 andTouch:p] == YES) {
		buttonPressed = 2;
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
	if (([PrefsMgr isPressed:BUTTONOKX andTouch:p]) && (buttonPressed == 0)) {
		[SoundMgr playEffect:SOUNDBUTTONPRESS];

		mFade = 2;
		
		if (muteSound) {
			[SoundMgr mute];
		} else {
			if (sMuteSound) {
				[SoundMgr unmute];
			}
		}
		
		sMuteSound = muteSound;		
		[PrefsMgr savePrefs];
	} else if (([PrefsMgr isPressed:BUTTONCANCELX andTouch:p]) && (buttonPressed == 1)) {
		[SoundMgr playEffect:SOUNDBUTTONPRESS];		
		[PrefsMgr setDefaults];
		mFade = 2;
	} else if (([PrefsMgr areaHitTest:(GLfloat) chkBoxX andY:CHKMUTESOUNDY andWidth:32.0 andHeight:32.0 andTouch:p] == YES) && (buttonPressed == 2)) {
		[SoundMgr playEffect:SOUNDBUTTONPRESS];
		muteSound = !muteSound;
	}
	
	buttonPressed = -1;
}

/**
 * See if button is pressed.
 *
 * @param base Button x position.
 * @param p Touch.
 * @return YES if pressed, NO otherwise.
 */
+ (BOOL) isPressed:(GLfloat) base andTouch:(CGPoint *) p {
	return [PrefsMgr areaHitTest:base andY:BUTTONY andWidth:64.0 andHeight:40.0 andTouch:p];
}

/**
 * See if an area is pressed.
 *
 * @param x X position.
 * @param y Y position.
 * @param w Width.
 * @param h Height.
 * @param p Touch.
 * @return YES if pressed, NO otherwise.
 */
+ (BOOL) areaHitTest:(GLfloat) x andY:(GLfloat) y andWidth:(GLfloat) w andHeight:(GLfloat) h andTouch:(CGPoint *) p {
	GLfloat pY = p->y;
	
	if ((p->x >= x) && (p->x <= (x + w)) && (pY <= y) && (pY >= (y - h))) {
		return YES;
	}
	
	return NO;
}

/**
 * Draw button no at position x.
 *
 * @param buttonNo Button number.
 * @param x X position.
 */
+ (void) drawButton:(int) buttonNo atX:(GLfloat) x {
	if (buttonPressed == buttonNo) {
		buttonNo += 2;
	}
	
	vertices[0] = x;
	vertices[1] = -20.0;
	vertices[3] = vertices[0] + 64.0;	// width
	vertices[4] = vertices[1];
	vertices[6] = vertices[3];
	vertices[7] = vertices[1] + 64.0;	// height
	vertices[9] = vertices[0];
	vertices[10] = vertices[7];
	
	glTexCoordPointer(2, GL_FLOAT, 0, &prefsDefs[buttonNo * 8]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, verticeIdx);
}

/**
 * Draw "main" screen estate.
 *
 * @return next screen.
 */
+ (int) drawMainScreen {	
	if (mFade == 1) {
		mIdx++;
		if (mIdx > 9) {
			mFade = 8;
		}
	} else if (mFade == 2) {
		mIdx--;
		if (mIdx < 1) {
			return STATEINTROINIT;
		}
	}

	if (mFade) {
		glColor4f(opaqueness[mIdx], opaqueness[mIdx], opaqueness[mIdx], opaqueness[mIdx]);		
	}
	
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	[PrefsMgr drawButton:0 atX:BUTTONOKX];
	[PrefsMgr drawButton:1 atX:BUTTONCANCELX];

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		
	[FontMgr beginRender];

	glColor4f(0.8, 0.8, 0.8, opaqueness[mIdx]);
	[Utility centerLine:"PREFERENCES" atY:TXTPREFERENCESY];
	
	glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
	
	[PrefsMgr showCheckbox:muteSound andString:"MUTE SOUND" atX:chkBoxX andY:CHKMUTESOUNDY];
	[FontMgr endRender];
	
	return STATEPREFSRUN;
}

/**
 * Write a line with a checkbox and a string.
 *
 * @param value Value of checkbox.
 * @param string Accompanying string.
 * @param x X position.
 * @param y Y position.
 */
+ (void) showCheckbox:(int) value andString:(char *) string atX:(GLfloat) x andY:(GLfloat) y {
	glColor4f(0.75, 0.75, 0.25, opaqueness[mIdx]);
	if (value == 0) {
		[FontMgr render:&titleFont andText:"#" atX:x andY:y];
	} else {
		[FontMgr render:&titleFont andText:"$" atX:x andY:y];
	}

	glColor4f(0.25, 0.33, 0.75, opaqueness[mIdx]);
	[FontMgr render:&titleFont andText:string atX:(x + 32.0) andY:y];
}

/**
 * Load preferences from file.
 */
+ (void) loadPrefs {
	StateMgr *state = [[StateMgr alloc] init];
	int version = 0;
	
	sMuteSound = 0;
	[state openForReading:@"iChainsPrefs.bin"];
	[state read:&version andLength:sizeof(int)];
	if (version == 2) {
		[state read:&sMuteSound andLength:sizeof(int)];
		[state close:NO];
	} else {
		[state close:YES];
	}

	[state release];
}

/**
 * Save preferences to file.
 */
+ (void) savePrefs {
	StateMgr *state = [[StateMgr alloc] init];
	int version = 2;

	[state openForWriting:@"iChainsPrefs.bin"];
	[state write:&version andLength:sizeof(int)];
	[state write:&sMuteSound andLength:sizeof(int)];
	[state close:[state getError]];
	[state release];
}

/**
 * Is'er / Can'er.
 */
+ (BOOL) isSoundMuted {
	if (sMuteSound == 0) {
		return NO;
	}
	
	return YES;
}
 
@end