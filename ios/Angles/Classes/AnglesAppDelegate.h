//
//  AnglesAppDelegate.h
//  Angles
//
//  Created by Brian Schau on 9/30/09.
//  Copyright iPhoneChi 2009. All rights reserved.
//
#import <UIKit/UIKit.h>

@class AnglesViewController;

@interface AnglesAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    AnglesViewController *viewController;
	UIImageView *splashView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet AnglesViewController *viewController;

@end

