// Clean iOS compatibility header that properly separates Objective-C and C++
#pragma once

// First define the target platform
#ifndef IOS_TARGET
#define IOS_TARGET
#endif

// For Objective-C++ files (.mm)
#ifdef __OBJC__
    // Include full iOS frameworks
    #import <Foundation/Foundation.h>
    #import <UIKit/UIKit.h>
#else
    // For pure C++ files, only provide forward declarations
    #ifdef __cplusplus
    extern "C" {
    #endif
    
    // Forward declarations for Objective-C
    typedef void* objc_id;
    typedef struct objc_class* objc_Class;
    typedef struct objc_selector* objc_SEL;
    
    // Core types
    typedef objc_id objc_NSString;
    typedef objc_id objc_NSArray;
    typedef objc_id objc_NSDictionary;
    typedef objc_id objc_NSMutableArray;
    typedef objc_id objc_NSData;
    
    // UIKit types
    typedef objc_id objc_UIView;
    typedef objc_id objc_UIViewController;
    typedef objc_id objc_UIColor;
    typedef objc_id objc_UIWindow;
    
    // Core Graphics types
    typedef struct {
        double x, y;
    } objc_CGPoint;
    
    typedef struct {
        double width, height;
    } objc_CGSize;
    
    typedef struct {
        objc_CGPoint origin;
        objc_CGSize size;
    } objc_CGRect;
    
    #ifdef __cplusplus
    }
    #endif
#endif

// Common C/C++ includes that are safe everywhere
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
