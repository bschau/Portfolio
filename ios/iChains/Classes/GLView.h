//
//  GLView.h
//  iChains
//
//  Created by Brian Schau on 07/06/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <QuartzCore/QuartzCore.h>

@interface GLView : UIView {}

- (void) setupView:(CGRect *) rect;
- (void) prologue;
- (void) epilogue;
- (BOOL) getTouch:(CGPoint *) dst;
- (BOOL) getBeginTouch:(CGPoint *) dst;

@end