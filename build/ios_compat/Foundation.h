// Foundation.h stub for CI builds
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Basic Foundation types
typedef void* NSString;
typedef void* NSData;
typedef void* NSArray;
typedef void* NSDictionary;
typedef void* NSObject;
typedef void* NSError;
typedef void* NSDate;
typedef void* NSURL;

// Objective-C runtime basics for Foundation
typedef void* id;
typedef void* Class;
typedef void* SEL;
typedef void* IMP;

// Commonly used Foundation constants
#define NSNotFound ((unsigned long)(-1))
#define YES 1
#define NO 0
typedef unsigned char BOOL;

// Foundation functions
void* NSStringFromClass(void* cls);
void* NSClassFromString(const char* name);
void* NSLog(const char* format, ...);

#ifdef __cplusplus
}
#endif
