//
//  NumberFormatter.h
//  iChains
//
//  Created by Brian Schau on 7/23/09.
//  Copyright 2009 iPhoneChi.Com. All rights reserved.
//
#import <Foundation/Foundation.h>

@interface NumberFormatter : NSObject {}

+ (void) open;
+ (char *) format:(unsigned long) n;
+ (char *) format:(unsigned long) n andDst:(char *) dst;

@end
