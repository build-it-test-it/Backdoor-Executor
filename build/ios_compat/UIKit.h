// UIKit.h stub for CI builds
#pragma once

#include "Foundation.h"

#ifdef __cplusplus
extern "C" {
#endif

// Basic UIKit types
typedef void* UIView;
typedef void* UIViewController;
typedef void* UIButton;
typedef void* UITextField;
typedef void* UITextView;
typedef void* UILabel;
typedef void* UIColor;
typedef void* UIFont;
typedef void* UIImage;
typedef void* UIScreen;
typedef void* UIWindow;
typedef void* UIApplication;

// Structures
typedef struct {
    float x;
    float y;
    float width;
    float height;
} CGRect;

typedef struct {
    float x;
    float y;
} CGPoint;

typedef struct {
    float width;
    float height;
} CGSize;

// Factory functions (would be class methods in ObjC)
UIColor* UIColor_redColor(void);
UIColor* UIColor_blueColor(void);
UIColor* UIColor_greenColor(void);
UIColor* UIColor_blackColor(void);
UIColor* UIColor_whiteColor(void);
UIColor* UIColor_clearColor(void);

// Common UIKit functions
UIView* UIView_init(CGRect frame);
void UIView_addSubview(UIView* view, UIView* subview);
void UIView_setBackgroundColor(UIView* view, UIColor* color);

#ifdef __cplusplus
}
#endif
