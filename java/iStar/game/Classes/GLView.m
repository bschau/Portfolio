//
//  GLView.m
//  iStar
//
//  Created by Brian Schau on 05/05/09.
//  Copyright Folera.Com 2009. All rights reserved.
//
#import "GLView.h"

@interface GLView (private)
- (id)initGLES;
- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;
@end

@implementation GLView
static GLint backingWidth, backingHeight;
static EAGLContext *context;
static GLuint viewRenderbuffer, viewFramebuffer, depthRenderbuffer;
static BOOL touched = NO;
static CGPoint touchAt;

/**
 * Return layer class.
 */
+ (Class)layerClass {
	return [CAEAGLLayer class];
}

/**
 * Initialize view from frame.
 *
 * @param frame Frame to use.
 * @return view.
 */
- (id)initWithFrame:(CGRect)frame {
	self = [super initWithFrame:frame];
	if (self != nil) {
		self = [self initGLES];
	}
	return self;
}
/**
 * Initialize view from xib.
 *
 * @param xib XIB coder.
 * @return view.
 */
- (id)initWithCoder:(NSCoder*)coder {
	if ((self = [super initWithCoder:coder])) {
		self = [self initGLES];
	}	
	return self;
}

/**
 * Initialize OpenGL ES subsystem.
 *
 * @return layer class or nil.
 */
-(id)initGLES {
	CAEAGLLayer *eaglLayer = (CAEAGLLayer*) self.layer;
	
	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
										kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
										nil];
	
	context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
	if ((!context) || (![EAGLContext setCurrentContext:context]) || (![self createFramebuffer])) {
		[self release];
		return nil;
	}
	
	return self;
}

/**
 * Setup view.
 */
- (void)setupView:(CGRect *)rect {
	glViewport(0, 0, rect->size.width, rect->size.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrthof(0.0f, 320.0f, 0.0f, 480.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

/**
 * Layout subview.
 */
- (void)layoutSubviews {
	[EAGLContext setCurrentContext:context];
	[self destroyFramebuffer];
	[self createFramebuffer];
}

/**
 * Create the framebuffer.
 *
 * @return YES if successful, NO otherwise.
 */
- (BOOL)createFramebuffer {
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);

	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	glGenRenderbuffersOES(1, &depthRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);

	if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
	return YES;
}

/**
 * Deallocate framebuffers.
 */
- (void)destroyFramebuffer {
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;

	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;
	
	glDeleteRenderbuffersOES(1, &depthRenderbuffer);
	depthRenderbuffer = 0;
}

/**
 * Prologue for each frame to be rendered.
 */
- (void)prologue {
	[EAGLContext setCurrentContext:context];
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * Epilogue for each frame rendered.
 */
- (void)epilogue {
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
	
	GLenum err = glGetError();
	if (err) {
		NSLog(@"%x error", err);
	}
}

/**
 * Deallocate class.
 */
- (void)dealloc {
	if ([EAGLContext currentContext] == context) {
		[EAGLContext setCurrentContext:nil];
	}
	
	[context release];
	context = nil;
	
	[super dealloc];
}

/**
 * Handle touches.
 *
 * @param touches Touches.
 * @param event Current event.
 */
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *t = [[[event allTouches] allObjects] objectAtIndex:0];
	
	touchAt = [t locationInView:t.view];
	touched = YES;
}

/**
 * Get current touch (if any).
 *
 * @param dst Where to store touch.
 * @return true if touched, false otherwise.
 */
- (BOOL)getTouch:(CGPoint *)dst {
	if (!touched) {
		return NO;
	}
	
	memmove(dst, &touchAt, sizeof(CGPoint));
	touched = NO;
	return YES;
}

@end
