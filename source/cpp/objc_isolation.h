// Objective-C isolation header
// This header provides isolation between C++ and Objective-C/iOS frameworks

#pragma once

// This section only applies when compiling Objective-C++ code
#ifdef __OBJC__
    #import <Foundation/Foundation.h>
    #import <UIKit/UIKit.h>
    #import <QuartzCore/QuartzCore.h>
    #import <CoreGraphics/CoreGraphics.h>
#else
    // Forward declarations for Objective-C types in C++ code
    #ifdef __cplusplus
        // Common UIKit/Foundation type forward declarations
        typedef void* UIView;
        typedef void* UIButton;
        typedef void* UIViewController;
        typedef void* UIColor;
        typedef void* UIImage;
        typedef void* UIWindow;
        typedef void* UIGestureRecognizer;
        typedef void* UILongPressGestureRecognizer;
        typedef void* UIApplication;
        
        // QuartzCore types
        typedef void* CALayer;
        typedef void* CABasicAnimation;
        
        // CoreGraphics types
        typedef double CGFloat;
        typedef struct { CGFloat x; CGFloat y; } CGPoint;
        typedef struct { CGFloat width; CGFloat height; } CGSize;
        typedef struct { CGPoint origin; CGSize size; } CGRect;
        
        // Foundation types
        typedef void* NSString;
        typedef void* NSArray;
        typedef void* NSDictionary;
        typedef void* NSError;
        typedef void* NSFileManager;
        typedef void* NSBundle;
        typedef int BOOL;
        typedef void* NSData;
        
        // Needed enum types
        typedef enum {
            UIImpactFeedbackStyleLight,
            UIImpactFeedbackStyleMedium,
            UIImpactFeedbackStyleHeavy
        } UIImpactFeedbackStyle;
    #endif // __cplusplus
#endif // __OBJC__
