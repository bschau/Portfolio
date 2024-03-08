//
//  ThingMgr.h
//  iStar
//
//  Created by Brian Schau on 5/18/09.
//  Copyright 2009 Folera.Com. All rights reserved.
//
#import <Foundation/Foundation.h>

@interface ThingMgr : NSObject {}

+ (void)classInit;
+ (void)reset;
+ (void)add:(id)t;
+ (void)draw;
+ (int)getNumThings;
+ (id)get:(int)idx;

@end
