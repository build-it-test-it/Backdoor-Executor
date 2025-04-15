// Objective-C isolation header - Include this when you need iOS functionality
#pragma once

// This header safely includes all Objective-C headers and prevents conflicts with Lua

// Include real iOS/macOS headers
#ifdef __OBJC__
    #import <Foundation/Foundation.h>
    #import <UIKit/UIKit.h>
#else
    // Forward declarations for C++ files that need to reference but not access these types
    #ifdef __cplusplus
    extern "C" {
    #endif
    
    // Forward declare Objective-C types to prevent name conflicts
    typedef struct objc_object* id;
    typedef struct objc_class* Class;
    typedef struct objc_selector* SEL;
    
    // Forward declare common iOS types
    typedef id NSString;
    typedef id UIView;
    typedef id UIViewController;
    
    #ifdef __cplusplus
    }
    #endif
#endif

// Export bridge functions that can be called from Lua-using code
namespace ObjCBridge {
    // Safe wrapper functions that don't expose iOS types in their interface
    bool ShowAlert(const char* title, const char* message);
    bool SaveScript(const char* name, const char* script);
    const char* LoadScript(const char* name);
    
    // UI integration
    bool InjectFloatingButton();
    void ShowScriptEditor();
}
