//
//  GLView.h
//  iStar
//
//  Created by Brian Schau on 05/05/09.
//  Copyright Folera.Com 2009. All rights reserved.
//
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <QuartzCore/QuartzCore.h>

@interface GLView : UIView {}

- (void)setupView:(CGRect *)rect;
- (void)prologue;
- (void)epilogue;
- (BOOL)getTouch:(CGPoint *)dst;

@end
