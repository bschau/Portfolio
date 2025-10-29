//
//  StarsMgr.m
//  iChains
//
//  Created by Brian Schau on 07/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import "StarsMgr.h"

@implementation StarsMgr

static const int STARSPACE = 40;
static const int STARFRAMES = 10;
static const int STARSX = 8; // 320 / STARSPACE
static const int STARSY = 12;	//480 / STARSPACE;
static const GLfloat starAnim[] = { 0.6, 0.7, 0.8, 0.9, 1.0, 0.9, 0.8, 0.7, 0.6, 0.5 };
static GLfloat *stars;
static int noOfStars;
static int *fadingStars;
static int fadeDelay;

/**
 * Initialize star heaven.
 */
+ (void) setup {
	int idx = 0, x, y, t;
	GLfloat yPos = 0.0, xPos;
	
	noOfStars = STARSX * STARSY;
	stars = (GLfloat *) malloc(7 * sizeof(GLfloat) * noOfStars);
	fadingStars = (int *) malloc(2 * sizeof(int) * STARSY);
	
	for (y = 0; y < STARSY; y++) {
		xPos = 0.0;
		for (x = 0; x < STARSX; x++) {
			t = (random() % (STARSPACE * STARSPACE));
			stars[idx++] = xPos + (GLfloat) (t % STARSPACE);
			stars[idx++] = yPos + (GLfloat) (t / STARSPACE);
			stars[idx++] = 0.0;
			
			stars[idx++] = 0.5;
			stars[idx++] = 0.5;
			stars[idx++] = 0.5;
			stars[idx++] = 1.0;
			xPos += (GLfloat) STARSPACE;
		}
		yPos += (GLfloat) STARSPACE;
	}
	
	idx = 0;
	for (int i = 0; i < STARSY; i++) {
		fadingStars[idx++] = random() % (320 / STARSPACE);
		fadingStars[idx++] = random() % (STARFRAMES / 2);
	}
	
	fadeDelay = 0;
}

/**
 * Animate (blink) stars.
 */
+ (void) animate {
	if (fadeDelay > 7) {
		fadeDelay = 0;
		
		int p = 0, idx, i, animIdx;
		int *fs = fadingStars;
		
		for (i = 0 ; i < STARSY; i++) {
			idx = *fs;
			animIdx = *(fs + 1);
			if (animIdx == (STARFRAMES - 1)) {
				*fs = random() % (320 / STARSPACE);
				*(fs + 1) = 0;
			} else {
				animIdx++;
				*(fs + 1) = animIdx;
				
				idx += p;
				idx *= 7;
				idx += 3;
				
				stars[idx++] = starAnim[animIdx];
				stars[idx++] = starAnim[animIdx];
				stars[idx++] = starAnim[animIdx];
			}
			
			p += (320 / STARSPACE);			
			fs += 2;
		}
	} else {
		fadeDelay++;
	}
}

/**
 * Draw stars to current view.
 */
+ (void) draw {
	glPointSize(1.0f);
	glVertexPointer(3, GL_FLOAT, 7 * sizeof(GLfloat), stars);
	glColorPointer(4, GL_FLOAT, 7 * sizeof(GLfloat), &stars[3]);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POINTS, 0, noOfStars);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

@end