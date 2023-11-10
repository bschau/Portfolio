//
//  GameMgr.m
//  iChains
//
//  Created by Brian Schau on 07/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
//	Analysis of original ChainRxn scoring system:
//	http://shakpwner.wordpress.com/2009/05/25/chainrxn-analysis-of-max-and-min-possible-scores/
//
#import "GameMgr.h"
#include "Constants.h"

@interface GameMgr (private)
+ (void) setupFade;
+ (BOOL) handleTap:(GLView *) glView;
+ (void) playerAction:(GLView *) glView;
+ (void) moveBalls;
+ (void) drawBalls;
+ (void) detectCollisions;
+ (BOOL) isInRange:(int) base andBall:(int) tgt;
+ (void) addToScore:(int) idx andPoints:(unsigned long) sc;
+ (void) pushToDead:(int) idx;
+ (void) renderStatus;
+ (int) longToString:(unsigned long) ul andDst:(char *) dst andLen:(int) len;
+ (int) isGameFinished;
+ (void) tapToPlay:(GLfloat) y;
+ (void) drawNewLevel;
+ (void) drawLevelCompleted;
+ (void) drawLevelFailed;
+ (void) drawMissionAccomplished;
@end

@implementation GameMgr
#define BALLSTATEDEAD 0
#define BALLSTATEALIVE (BALLSTATEDEAD + 1)
#define BALLSTATEGROW (BALLSTATEALIVE + 1)
#define BALLSTATESHRINK (BALLSTATEGROW + 1)
#define BALLSTATEMOVE (BALLSTATESHRINK + 1)
#define BALLGROWSPEED 3.0
#define BALLSHRINKSPEED 4.0
#define BALLSIZE 12.0
#define BALLSALIVE 37	// RenderFreq * 1 + a bit second
#define MAXSIZE 50
#define SCOREALIVE 30
#define INITIALSCORE 6
#define BALLCOLORS 6
#define PTSSTRLEN 40	// Length of points string (max)
#define MAXBALLS 61		// Player + 60 balls
#define MAXLEVEL 11		// Max no. of levels: 0 .. MAXLEVEL
#define CHAINBONUS 10000

struct Ball {
	int state;
	GLfloat x;
	GLfloat y;
	GLfloat size;
	GLfloat spdX;
	GLfloat spdY;
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	GLfloat shadeRed;
	GLfloat shadeGreen;
	GLfloat shadeBlue;
	int alive;
	int name;
	int parent;
	int chainDepth;
	unsigned long ptsEarned;
	int ptsAlive;
	char ptsString[PTSSTRLEN];
	GLfloat ptsX;
	GLfloat ptsY;
};

struct BallColor {
	GLfloat red;
	GLfloat green;
	GLfloat blue;
};
static GLfloat vBalls[7];
static struct BallColor ballColors[] = {
	{ 0.9, 0.5, 0.5 }, { 0.5, 0.9, 0.5 }, { 0.5, 0.5, 0.9 },
	{ 0.9, 0.9, 0.5 }, { 0.9, 0.5, 0.9 }, { 0.5, 0.9, 0.9 }
};
static int fontW[16] = { 8, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 13, 8, 8, 7, 6 };
static FontInfo font = { @"ingamefont.png", 16, 4, 4, (int)'0', &fontW[0], nil, 0, nil };
// ChainRxn levels:
//static int levelData[24] = {
//	5, 1, 10, 2, 15, 4, 20, 6, 25, 10, 30, 15,
//	35, 18, 40, 22, 45, 30, 50, 47, 55, 48, 60, 54 };
static struct {
	int noOfBalls;
	int goal;
} levelData[] = {
{ 5, 1 }, { 10, 2 }, { 15, 4 }, { 20, 6 }, { 25, 10 }, { 30, 15 },
{ 35, 18 }, { 40, 22 }, { 43, 30 }, { 47, 38 }, { 52, 45 }, { 55, 51 } };
static int blinkIdx, blinkTimer;
static BOOL exitGame, inSysDialog;
extern GLfloat opaqueness[11];
extern CGPoint touchPoint;
extern FontInfo titleFont;

/*****************************************************************************************
 * Variables below here needs to be persisted
 */
static int level, deadIdx, noOfBalls, chained, nextState;
static BOOL canFire, canTap, topScore;
struct Ball balls[MAXBALLS];
unsigned long runningTotal, score;
static int gFade, gIdx;
static int longestChain;
/*
 * End persistence block
 ****************************************************************************************/

/**
 * Open the Game Manager.
 */
+ (void) open {
	[FontMgr loadFont:&font];
}

/**
 * Close the Game Manager.
 */
+ (void) close {
	[FontMgr releaseFont:&font];
}

/**
 * Initialize tap system.
 */
+ (void) fromIntro {
	canTap = YES;	
	level = 0;
	runningTotal = 0;
	
	[GameMgr setupFade];
}

/**
 * Setup the current level.
 */
+ (void) setup {
	int i;
	
	score = 0;
	deadIdx = 1;
	longestChain = 0;
	blinkIdx = 0;
	blinkTimer = 0;
	
	noOfBalls = levelData[level].noOfBalls;
	chained = 0;

	canFire = YES;
	
	for (i = 0; i < MAXBALLS; i++) {
		memset(&balls[i].state, 0, sizeof(struct Ball));
		balls[i].state = BALLSTATEDEAD;
		balls[i].name = i;
		balls[i].parent = -1;
		balls[i].chainDepth = 0;
	}

	vBalls[2] = 0.0;		// z
	
	// Player
	balls[0].state = BALLSTATEMOVE;
	balls[0].red = 0.9;
	balls[0].green = 0.9;
	balls[0].blue = 0.9;
	balls[0].shadeRed = 0.5;
	balls[0].shadeGreen = 0.5;
	balls[0].shadeBlue = 0.5;
	balls[0].parent = -1;
	balls[0].ptsEarned = INITIALSCORE;
	
	int idx = 1, col = random() % BALLCOLORS;
	
	for (i = 0; i < noOfBalls; i++) {
		balls[idx].state = BALLSTATEMOVE;
		balls[idx].x = (GLfloat) (((random() % (304 / 8)) * 8) + 8);
		balls[idx].y = (GLfloat) (((random() % (464 / 8)) * 8) + 8);
		balls[idx].size = BALLSIZE;
		balls[idx].spdX = (GLfloat) ((random() % 3) + 1);
		if ((random() % 50) > 25) {
			balls[idx].spdX = -balls[idx].spdX;
		}

		balls[idx].spdY = (GLfloat) ((random() % 3) + 1);
		if ((random() % 50) > 25) {
			balls[idx].spdY = -balls[idx].spdY;
		}

		balls[idx].red = ballColors[col].red;
		balls[idx].green = ballColors[col].green;
		balls[idx].blue = ballColors[col].blue;
		balls[idx].shadeRed = balls[idx].red - 0.4;
		balls[idx].shadeGreen = balls[idx].green - 0.4;
		balls[idx].shadeBlue = balls[idx].blue - 0.4;

		col += random() % BALLCOLORS;
		if (col > (BALLCOLORS - 1)) {
			col -= BALLCOLORS;
		}
		
		idx++;
	}
	
	[GameMgr setupFade];
}

/**
 * Setup fade parameters.
 */
+ (void) setupFade {
	gFade = 1;
	gIdx = 0;
	exitGame = NO;
	inSysDialog = NO;
}

/**
 * Run the game.
 *
 * @param glView GL View.
 * @param state Current state.
 * @return next state.
 */
+ (int) run:(GLView *) glView andState:(int) state {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (gFade == 1) {
		gIdx++;
		if (gIdx > 9) {
			gFade = 0;
		}
	} else if (gFade == 2) {
		gIdx--;
		if (gIdx < 1) {
			gFade = 8;
		}
	}
	
	if (gFade) {
		glColor4f(opaqueness[gIdx], opaqueness[gIdx], opaqueness[gIdx], opaqueness[gIdx]);		
	}
	
	switch (state) {
		case STATEGAMERUNNING:
			if (inSysDialog == NO) {
				[GameMgr playerAction:glView];
				if (exitGame == YES) {
					return STATEINTROCOLDBOOT;
				}
			
				[GameMgr moveBalls];
				[GameMgr detectCollisions];
				[GameMgr drawBalls];
				[GameMgr renderStatus];
				return [GameMgr isGameFinished];
			}
			
			[GameMgr drawBalls];
			[GameMgr renderStatus];
			return STATEGAMERUNNING;

		case STATEGAMENEWLEVEL:
			[GameMgr drawNewLevel];
			if ([GameMgr handleTap:glView] == YES) {
				[GameMgr setup];
				return STATEGAMERUNNING;
			}
			
			return STATEGAMENEWLEVEL;
			
		case STATEGAMELEVELDONE:
			[GameMgr moveBalls];
			[GameMgr drawBalls];
			[GameMgr renderStatus];
			
			if (gFade == 8) {
				gFade = 1;
				return nextState;
			}
			return STATEGAMELEVELDONE;
			
		case STATEGAMELEVELCOMPLETED:
			[GameMgr drawLevelCompleted];
			if ([GameMgr handleTap:glView] == YES) {
				return STATEGAMESWITCHLEVEL;
			}
			return STATEGAMELEVELCOMPLETED;
			
		case STATEGAMELEVELFAILED:
			[GameMgr drawLevelFailed];
			if ([GameMgr handleTap:glView] == YES) {
				[GameMgr setup];
				return STATEGAMERUNNING;
			}
			return STATEGAMELEVELFAILED;
			
		case STATEGAMEMISSIONACCOMPLISHED:
			[GameMgr drawMissionAccomplished];
			if ([GameMgr handleTap:glView] == YES) {
				return STATEINTROCOLDBOOT;
			}
			return STATEGAMEMISSIONACCOMPLISHED;
			
		case STATEGAMESWITCHLEVEL:
			level++;
			[GameMgr setup];
			return STATEGAMERUNNING;
	}
	
	return 0;
}

/**
 * Handle 'tap'.
 *
 * @return YES if tapped and faded down, NO otherwise.
 */
+ (BOOL) handleTap:(GLView *) glView {
	if (canTap == YES) {
		if ([glView getTouch:&touchPoint]) {
			[SoundMgr playEffect:SOUNDBUTTONPRESS];
			gFade = 2;
			canTap = NO;
		}
	} else {
		if (gIdx == 0) {
			gFade = 1;
			canTap = YES;
			return YES;
		}
	}
	
	return NO;
}

/**
 * Handle player action.
 *
 * @param glView GLView.
 */
+ (void) playerAction:(GLView *) glView {
	if (canFire) {
		if ([glView getTouch:&touchPoint]) {
			canFire = NO;
			balls[0].state = BALLSTATEGROW;
			balls[0].x = touchPoint.x;
			balls[0].y = 480.0 - touchPoint.y;
			balls[0].size = 1.0;
			[SoundMgr playEffect:SOUNDFIRE];
		}
	} else {
		if ([glView getTouch:&touchPoint]) {
			[[[UIAlertView alloc] initWithTitle:@"Interrupt?" message:@"Do you want to exit the current game?" delegate:self cancelButtonTitle:@"No" otherButtonTitles:@"Yes", nil] show]; 
			inSysDialog = YES;
			return;
		}
		
		switch (balls[0].state) {
			case BALLSTATEGROW:
				if (balls[0].size < MAXSIZE) {
					balls[0].size += BALLGROWSPEED;
				} else {
					balls[0].state = BALLSTATEALIVE;
					balls[0].alive = BALLSALIVE;
				}
				break;

			case BALLSTATEALIVE:
				if (balls[0].alive > 0) {
					balls[0].alive--;
				} else {
					balls[0].state = BALLSTATESHRINK;
				}
				break;

			case BALLSTATESHRINK:
				if (balls[0].size > 0.0) {
					balls[0].size -= BALLSHRINKSPEED;
				} else {
					balls[0].state = BALLSTATEDEAD;
				}
				break;
		}		
	}
}
/**
 * Interrupt mechanism.
 */
+ (void) alertView:(UIAlertView *) alertView clickedButtonAtIndex:(NSInteger) buttonIndex {
	if (buttonIndex == 1) {
		exitGame = YES;
	}
	
	inSysDialog = NO;
}

/**
 * Move the balls.
 */
+ (void) moveBalls {
	int i;
	
	for (i = 1; i <= noOfBalls; i++) {
		switch (balls[i].state) {
			case BALLSTATEMOVE:
				balls[i].x += balls[i].spdX;
				balls[i].y += balls[i].spdY;
				if ((balls[i].x < (BALLSIZE / 2.0)) || (balls[i].x > (320.0 - (BALLSIZE / 2.0)))) {
					balls[i].spdX = -balls[i].spdX;
				}
				if ((balls[i].y < (BALLSIZE / 2.0)) || (balls[i].y > (480.0 - (BALLSIZE / 2.0)))) {
					balls[i].spdY = -balls[i].spdY;
				}
				break;

			case BALLSTATEGROW:
				if (balls[i].size < MAXSIZE) {
					balls[i].size += BALLGROWSPEED;
				} else {
					balls[i].state = BALLSTATEALIVE;
					balls[i].alive = BALLSALIVE;
				}
				break;

			case BALLSTATEALIVE:
				if (balls[i].alive > 0) {
					balls[i].alive--;
				} else {
					balls[i].state = BALLSTATESHRINK;
				}
				break;

			case BALLSTATESHRINK:
				if (balls[i].size > 0.0) {
					balls[i].size -= BALLSHRINKSPEED;
				} else {
					balls[i].state = BALLSTATEDEAD;
					balls[i].ptsEarned = INITIALSCORE;
					balls[i].parent = -1;
				}
				break;
		}
		
		if (balls[i].ptsAlive > 0) {
			balls[i].ptsAlive--;
		}
	}
}

/**
 * Draw the balls.
 */
+ (void)drawBalls {
	glEnable(GL_POINT_SMOOTH);
	glVertexPointer(3, GL_FLOAT, 7 * sizeof(GLfloat), vBalls);
	glColorPointer(4, GL_FLOAT, 7 * sizeof(GLfloat), &vBalls[3]);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	int i;	
	GLfloat size;

	vBalls[6] = opaqueness[gIdx];
	
	for (i = 0; i < (noOfBalls + 1); i++) {
		if ((balls[i].state != BALLSTATEDEAD) && (balls[i].size > 0.0)) {
			vBalls[0] = balls[i].x;
			vBalls[1] = balls[i].y;
			vBalls[3] = balls[i].shadeRed;
			vBalls[4] = balls[i].shadeGreen;
			vBalls[5] = balls[i].shadeBlue;

			glDisable(GL_BLEND);
			glPointSize(balls[i].size);
			glDrawArrays(GL_POINTS, 0, 1);

			vBalls[3] = balls[i].red;
			vBalls[4] = balls[i].green;
			vBalls[5] = balls[i].blue;

			if ((size = balls[i].size - 2.0) > 0.0) {
				glEnable(GL_BLEND);
				glPointSize(size);
				glDrawArrays(GL_POINTS, 0, 1);
			}

			if (balls[i].ptsAlive > 0) {
				glEnable(GL_BLEND);
				glDisableClientState(GL_COLOR_ARRAY);
				[FontMgr beginRender];
				glColor4f(0.1, 0.1, 0.1, opaqueness[gIdx]);
				[FontMgr render:&font andText:balls[i].ptsString atX:balls[i].ptsX andY:balls[i].ptsY];
				[FontMgr endRender];
				
				glVertexPointer(3, GL_FLOAT, 7 * sizeof(GLfloat), vBalls);
				glColorPointer(4, GL_FLOAT, 7 * sizeof(GLfloat), &vBalls[3]);
				glEnableClientState(GL_COLOR_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);
			}
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_POINT_SMOOTH);
}

/**
 * See if any balls have collided with any chains.
 */
+ (void) detectCollisions {
	unsigned long sc = 0;
	int idx, i, z;
	
	for (idx = 1; idx <= noOfBalls; idx++) {
		if (balls[idx].state == BALLSTATEMOVE) {
			for (i = 0; i <= noOfBalls; i++) {
				if (i == idx) {
					continue;
				}
					
				if ((balls[i].state >= BALLSTATEALIVE) && (balls[i].state <= BALLSTATESHRINK)) {
					if ([GameMgr isInRange:i andBall:idx]) {
						balls[idx].state = BALLSTATEGROW;
						balls[idx].parent = balls[i].name;
						
						i = balls[i].name;
						for (z = 1; z <= noOfBalls; z++) {
							if (balls[z].name == i) {
								i = z;
								balls[idx].chainDepth = balls[i].chainDepth + 1;
								if (balls[idx].chainDepth > longestChain) {
									longestChain = balls[idx].chainDepth;
								}
								
								break;
							}
						}
						
						z = balls[idx].chainDepth + 1;

						sc = (z * z * z) * 100;
						if (balls[0].state == BALLSTATEDEAD) {
							sc += (sc / 2);
						}

						chained++;
						[GameMgr addToScore:idx andPoints:sc];
						[GameMgr pushToDead:idx];
						[SoundMgr playEffect:SOUNDEXPLOSION];
					}
				}
			}			
		}
	}
}

/**
 * See if two given balls are "in range" (collides).
 *
 * @param base Base ball.
 * @param tgt Target ball.
 * @return YES if in range, NO otherwise.
 */
+ (BOOL) isInRange:(int) base andBall:(int) tgt {
	GLfloat x = balls[base].x - balls[tgt].x, y = balls[base].y - balls[tgt].y;
	GLfloat l = sqrt(abs((x * x) + (y * y)));
	
	if ((l - (balls[base].size / 2.0) - (balls[tgt].size) / 2.0) <= 0.0) {
		return YES;
	}
	
	return NO;
}

/**
 * Add points to score. Also setup ptsString etc.
 *
 * @param idx Ball index.
 * @param pts Points to add.
 */
+ (void) addToScore:(int) idx andPoints:(unsigned long) sc {
	char *ptr, *dst;
	int len;

	score += sc;
	balls[idx].ptsEarned = sc;
	balls[idx].ptsAlive = SCOREALIVE;
	
	len = [GameMgr longToString:sc andDst:&balls[idx].ptsString[0] andLen:PTSSTRLEN];
	ptr = &balls[idx].ptsString[PTSSTRLEN - len];
	
	*--ptr = ':';		// '+'
	dst = &balls[idx].ptsString[0];
	while (*ptr) {
		*dst++ = *ptr++;
	}
	*dst = 0;
	
	len = [FontMgr getStringWidth:&font andString:&balls[idx].ptsString[0]];
	balls[idx].ptsX = balls[idx].x - (GLfloat) (len / 2);
	balls[idx].ptsY = 480.0 - (balls[idx].y - (GLfloat) (font.glyphSize / 2));
}

/**
 * Push this ball to the zone of dead balls.
 *
 * @param idx Index of ball.
 */
+ (void) pushToDead:(int) idx {
	struct Ball tmp;
	
	memmove(&tmp.state, &balls[deadIdx].state, sizeof(struct Ball));
	memmove(&balls[deadIdx].state, &balls[idx].state, sizeof(struct Ball));
	memmove(&balls[idx].state, &tmp.state, sizeof(struct Ball));
	
	deadIdx++;
}

/**
 * Render status (score, balls left).
 */
+ (void) renderStatus {
	char scTxt[PTSSTRLEN];
	char *ptr;
	int len;

	len = [GameMgr longToString:score andDst:scTxt andLen:PTSSTRLEN];
	ptr = scTxt + PTSSTRLEN - len;
	
	len = [FontMgr getStringWidth:&font andString:ptr];
	
	[FontMgr beginRender];
	glColor4f(0.8, 0.8, 0.8, opaqueness[gIdx]);
	[FontMgr render:&font andText:ptr atX:(320 - len) andY:16];

	len = (levelData[level].goal) - chained;
	if (len > 0) {
		[FontMgr render:&font andText:";" atX:0 andY:16];	
		if (len == 1) {
			[FontMgr render:&font andText:"<=>?" atX:16 andY:16];	// 'Last'
		} else if (len > 1) {
			len = [GameMgr longToString:(unsigned long)len andDst:scTxt andLen:PTSSTRLEN];
			ptr = scTxt + PTSSTRLEN - len;
			[FontMgr render:&font andText:ptr atX:16 andY:16];		
		}
	} else {
		int x = 0;
		
		for (len = 0; len < 6; len++) {
			if (len == blinkIdx) {
				glColor4f(ballColors[len].red, ballColors[len].green, ballColors[len].blue, 0.5);
			} else {
				glColor4f(ballColors[len].red, ballColors[len].green, ballColors[len].blue, 1.0);
			}
			[FontMgr render:&font andText:";" atX:x andY:16];
			x += 16;
		}
		
		blinkTimer++;
		if (blinkTimer > 2) {
			blinkTimer = 0;
			blinkIdx++;
			if (blinkIdx > 5) {
				blinkIdx = 0;
			}
		}
	}
	
	[FontMgr endRender];
}

/**
 * Convert an unsigned long to a string and store in dst.
 *
 * @param ul Unsigned long.
 * @param dst Where to store string.
 * @param len Length of dst.
 * @return characters written.
 */
+ (int) longToString:(unsigned long) ul andDst:(char *) dst andLen:(int) len {
	int written = 1;
	char *ptr = dst + len - 1;
	
	*ptr = 0;
	if (ul == 0) {
		*--ptr = '0';
		written++;
	} else {
		while ((ul > 0) && (--ptr >= dst)) {
			*ptr = (ul % 10) + '0';
			ul /= 10;
			written++;
		}
	}
	
	return written;
}

/**
 * Is game finished?
 *
 * @return new state.
 */
+ (int) isGameFinished {
	int i, len;
	
	if (balls[0].state != BALLSTATEDEAD) {
		return STATEGAMERUNNING;
	}
		
	for (i = 1; i <= noOfBalls; i++) {
		if ((balls[i].state >= BALLSTATEALIVE) && (balls[i].state <= BALLSTATESHRINK)) {
			return STATEGAMERUNNING;
		}
	}
		
	len = (levelData[level].goal) - chained;

	if (len > 0) {
		nextState = STATEGAMELEVELFAILED;
	} else {
		runningTotal += score;
		runningTotal += CHAINBONUS * (longestChain * level);
		
		topScore = [HiScoreMgr addScore:runningTotal];
			
		if (level == MAXLEVEL) {
			nextState = STATEGAMEMISSIONACCOMPLISHED;
		} else {
			nextState = STATEGAMELEVELCOMPLETED;
		}
	}
		
	gFade = 2;
	return STATEGAMELEVELDONE;
}

/**
 * Draw 'tap to play' message at Y.
 *
 * @param y Y position.
 */
+ (void) tapToPlay:(GLfloat) y {
	glColor4f(0.75, 0.75, 0.33, opaqueness[gIdx]);
	[Utility centerLine:"TAP TO PLAY" atY:y];
}

/**
 * Draw "Level X, Get X out of X balls, Tap to play" message.
 */
+ (void) drawNewLevel {
	char buffer[100];

	[FontMgr beginRender];
	glColor4f(0.25, 0.33, 0.75, opaqueness[gIdx]);
	snprintf(buffer, sizeof(buffer), "LEVEL %i", level + 1);
	[Utility centerLine:buffer atY:176.0];

	glColor4f(0.8, 0.8, 0.8, opaqueness[gIdx]);
	snprintf(buffer, sizeof(buffer), "GET %i OUT OF %i BALLS", levelData[level].goal, levelData[level].noOfBalls);
	[Utility centerLine:buffer atY:246.0];

	[GameMgr tapToPlay:336.0];
	[FontMgr endRender];
}

/**
 * Draw Level Completed message.
 */
+ (void) drawLevelCompleted {
	char buffer[100];
	
	[FontMgr beginRender];
	glColor4f(0.25, 0.75, 0.33, opaqueness[gIdx]);
	[Utility centerLine:"LEVEL COMPLETED!" atY:68.0];

	glColor4f(0.8, 0.8, 0.8, opaqueness[gIdx]);
	snprintf(buffer, sizeof(buffer), "LEVEL SCORE: %s", [NumberFormatter format:score]);
	[Utility centerLine:buffer atY:148.0];
	
	if (longestChain == 0) {
		[Utility centerLine:"NO CHAIN BONUS" atY:188.0];
	} else {
		snprintf(buffer, sizeof(buffer), "CHAIN BONUS: %s", [NumberFormatter format:(longestChain * CHAINBONUS)]);
		[Utility centerLine:buffer atY:188.0];
	}
	
	snprintf(buffer, sizeof(buffer), "TOTAL SCORE: %s", [NumberFormatter format:runningTotal]);
	[Utility centerLine:buffer atY:228.0];

	glColor4f(0.25, 0.33, 0.75, opaqueness[gIdx]);
	if ((level + 1) == MAXLEVEL) {
		[Utility centerLine:"NOW PLAY LAST LEVEL" atY:308.0];
	} else {
		snprintf(buffer, sizeof(buffer), "NOW PLAY LEVEL %i", level + 2);
		[Utility centerLine:buffer atY:308.0];
	}
	snprintf(buffer, sizeof(buffer), "GET %i OUT OF %i BALLS", levelData[level].goal, levelData[level + 1].noOfBalls);
	[Utility centerLine:buffer atY:348.0];
	
	[GameMgr tapToPlay:428.0];
	[FontMgr endRender];
}

/**
 * Draw Level Failed message.
 */
+ (void) drawLevelFailed {
	int ballsLeft = levelData[level].goal - chained;
	char buffer[100];
	
	[FontMgr beginRender];
	glColor4f(0.75, 0.25, 0.33, opaqueness[gIdx]);
	[Utility centerLine:"LEVEL FAILED!" atY:112.0];

	glColor4f(0.25, 0.33, 0.75, opaqueness[gIdx]);
	snprintf(buffer, sizeof(buffer), "%i MORE BALL%s ...", ballsLeft, (ballsLeft == 1) ? "" : "S");
	[Utility centerLine:buffer atY:192.0];

	glColor4f(0.8, 0.8, 0.8, opaqueness[gIdx]);
	snprintf(buffer, sizeof(buffer), "REPLAY LEVEL %i", level + 1);
	[Utility centerLine:buffer atY:272.0];
	snprintf(buffer, sizeof(buffer), "GET %i OUT OF %i BALLS", levelData[level].goal, levelData[level].noOfBalls);
	[Utility centerLine:buffer atY:312.0];

	[GameMgr tapToPlay:392.0];
	[FontMgr endRender];
}

/**
 * Draw mission accomplished message.
 */
+ (void) drawMissionAccomplished {
	GLfloat y = (topScore == YES ? 88.0 : 128.0);
	char buffer[100];
	
	[FontMgr beginRender];
	glColor4f(0.25, 0.75, 0.33, opaqueness[gIdx]);
	[Utility centerLine:"MISSION ACCOMPLISHED!" atY:y];

	y += 80.0;
	glColor4f(0.8, 0.8, 0.8, opaqueness[gIdx]);
	snprintf(buffer, sizeof(buffer), "LEVEL SCORE: %s", [NumberFormatter format:score]);
	[Utility centerLine:buffer atY:y];

	y += 40.0;
	if (longestChain == 0) {
		[Utility centerLine:"NO CHAIN BONUS" atY:y];
	} else {
		snprintf(buffer, sizeof(buffer), "CHAIN BONUS: %s", [NumberFormatter format:(longestChain * CHAINBONUS)]);
		[Utility centerLine:buffer atY:y];
	}
	
	y += 40.0;
	snprintf(buffer, sizeof(buffer), "TOTAL SCORE: %s", [NumberFormatter format:runningTotal]);
	[Utility centerLine:buffer atY:y];

	if (topScore) {
		y += 80;
		[Utility centerLine:"NEW RECORD SCORE!!" atY:y];
	}
	
	y += 80.0;
	glColor4f(0.75, 0.75, 0.33, opaqueness[gIdx]);
	[Utility centerLine:"TAP TO CONTINUE" atY:y];
	[FontMgr endRender];
}

/**
 * Persist game state to file.
 *
 * @param state Current state.
 */
+ (void) saveState:(int) state {
	StateMgr *sm = [[StateMgr alloc] init];
	int version = 2;
	
	[sm openForWriting:@"iChainsState.xml"];
	[sm write:&version andLength:sizeof(int)];
	[sm write:&state andLength:sizeof(int)];
	[sm write:&level andLength:sizeof(int)];
	[sm write:&deadIdx andLength:sizeof(int)];
	[sm write:&noOfBalls andLength:sizeof(int)];
	[sm write:&chained andLength:sizeof(int)];
	[sm write:&nextState andLength:sizeof(int)];
	[sm write:&canFire andLength:sizeof(BOOL)];
	[sm write:&canTap andLength:sizeof(BOOL)];
	[sm write:&topScore andLength:sizeof(BOOL)];
	[sm write:&balls[0] andLength:MAXBALLS * sizeof(struct Ball)];
	[sm write:&runningTotal andLength:sizeof(unsigned long)];
	[sm write:&score andLength:sizeof(unsigned long)];
	[sm write:&gFade andLength:sizeof(int)];
	[sm write:&gIdx andLength:sizeof(int)];
	[sm write:&longestChain andLength:sizeof(int)];
	[sm close:[sm getError]];
	[sm release];
}

/**
 * Load state from file.
 *
 * @return State.
 */
+ (int) loadState {
	StateMgr *sm = [[StateMgr alloc] init];
	int version = 0, state = STATEINTROINIT;
	
	[sm openForReading:@"iChainsState.xml"];
	[sm read:&version andLength:sizeof(int)];
	if (version == 2) {
		[sm read:&state andLength:sizeof(int)];
		[sm read:&level andLength:sizeof(int)];
		[sm read:&deadIdx andLength:sizeof(int)];
		[sm read:&noOfBalls andLength:sizeof(int)];
		[sm read:&chained andLength:sizeof(int)];
		[sm read:&nextState andLength:sizeof(int)];
		[sm read:&canFire andLength:sizeof(BOOL)];
		[sm read:&canTap andLength:sizeof(BOOL)];
		[sm read:&topScore andLength:sizeof(BOOL)];
		[sm read:&balls[0] andLength:MAXBALLS * sizeof(struct Ball)];
		[sm read:&runningTotal andLength:sizeof(unsigned long)];
		[sm read:&score andLength:sizeof(unsigned long)];
		[sm read:&gFade andLength:sizeof(int)];
		[sm read:&gIdx andLength:sizeof(int)];
		[sm read:&longestChain andLength:sizeof(int)];
		if ([sm getError] == YES) {
			state = STATEINTROINIT;
		}
	}
	
	[sm close:YES];
	[sm release];
	
	return state;	
}

//#ifdef DEBUG
//+ (void) specialSetup {
//	[GameMgr fromIntro];
//	level = 10;
//}
//#endif

@end