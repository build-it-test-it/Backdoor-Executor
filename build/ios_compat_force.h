// Force-include header for iOS compatibility
// This header ensures critical defines are available to all source files

#pragma once

// Define CI_BUILD
#ifndef CI_BUILD
#define CI_BUILD
#endif

// Redefine any Objective-C syntax that might be problematic
#define @interface class
#define @implementation
#define @end };
#define @property
#define @protocol(x) (void*)0
#define @selector(x) nullptr

// Define common types
typedef void* id;
typedef void* Class;
typedef void* SEL;
typedef void* IMP;

// Define common Foundation types
typedef void* NSString;
typedef void* NSData;
typedef void* NSArray;
typedef void* NSDictionary;

// Force include into all source files that might use iOS code
#include "ios_compat.h"
