// Objective-C isolation header - Include this when you need iOS functionality
#pragma once

// This is a clean isolation layer that prevents conflicts between Objective-C and C++

// C++ safe declarations (non-Objective-C code)
#ifndef __OBJC__

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations of Objective-C types
typedef void* objc_id;
typedef struct objc_class* objc_Class;
typedef struct objc_selector* objc_SEL;

// Define common Objective-C types as opaque pointers
typedef objc_id objc_NSString;
typedef objc_id objc_NSArray;
typedef objc_id objc_NSDictionary;
typedef objc_id objc_UIView;
typedef objc_id objc_UIColor;
typedef objc_id objc_UIViewController;

// Define Core Graphics structures for C++
typedef struct {
    double x;
    double y;
} objc_CGPoint;

typedef struct {
    double width;
    double height;
} objc_CGSize;

typedef struct {
    objc_CGPoint origin;
    objc_CGSize size;
} objc_CGRect;

#ifdef __cplusplus
}
#endif

#endif // !__OBJC__

// Common C++ definitions that work in both contexts
#include <string>
#include <vector>
#include <functional>

// Define iOS target
#ifndef IOS_TARGET
#define IOS_TARGET
#endif

// Bridge functions that work in both Objective-C++ and C++
namespace ObjCBridge {
    // UI Functions
    bool ShowAlert(const char* title, const char* message);
    bool CreateFloatingButton(int x, int y, int width, int height, const char* title);
    
    // File Functions
    bool SaveFile(const char* path, const char* data, size_t length);
    char* LoadFile(const char* path, size_t* outLength);
}
