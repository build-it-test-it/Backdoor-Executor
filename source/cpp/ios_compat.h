// Master header for iOS compatibility in CI builds
#pragma once

#include "ci_config.h"

// In CI builds, use our compatibility headers
#if IS_CI_BUILD
    #include <ios_compat/Foundation.h>
    #include <ios_compat/UIKit.h>
    #include <ios_compat/objc_runtime.h>
    
    // Define Objective-C directives as no-ops
    #define NS_ASSUME_NONNULL_BEGIN
    #define NS_ASSUME_NONNULL_END
    #define NS_SWIFT_NAME(name)
    #define NS_REFINED_FOR_SWIFT
    #define NS_SWIFT_UNAVAILABLE(msg)
    #define API_AVAILABLE(...)
    #define API_UNAVAILABLE(...)
    
    // Define ObjC syntax as C++ equivalents
    #define @interface struct
    #define @end };
    #define @implementation
    #define @property
    #define @selector(x) sel_registerName(#x)
    #define @protocol(x) (void*)0
    
    // String literals
    #define @"string" "string"
    
    // ObjC casts
    #define __bridge 
    
    // ObjC arrays, dictionaries
    #define @[] nullptr
    #define @{} nullptr
    
    // Block syntax
    #define 
^
 [=]
    
    // Nil check
    #define NSNotFound -1
#else
    // For real iOS builds, include the actual headers
    #import <Foundation/Foundation.h>
    #import <UIKit/UIKit.h>
    #import <objc/runtime.h>
#endif
