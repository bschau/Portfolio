//
//  iStarAppDelegate.h
//  iStar
//
//  Created by Brian Schau on 05/05/09.
//  Copyright Folera.Com 2009. All rights reserved.
//
#import "GameMgr.h"
#import "GfxMgr.h"
#import "PlayerMgr.h"
#import "StarsMgr.h"
#import "ThingMgr.h"

@interface iStarAppDelegate : NSObject <UIApplicationDelegate>{
	UIWindow *window;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end
