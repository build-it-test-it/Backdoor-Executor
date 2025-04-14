//
// MethodSwizzling.h
// Provides iOS-specific method swizzling utilities to replace function hooking
//

#pragma once

#if defined(__APPLE__) || defined(IOS_TARGET)
#import <Foundation/Foundation.h>
#import <objc/runtime.h>

namespace iOS {

/**
 * @brief Utility class for method swizzling in Objective-C
 * 
 * This class provides a safer alternative to MSHookFunction for iOS
 * by using the Objective-C runtime to swizzle methods.
 */
class MethodSwizzling {
public:
    /**
     * @brief Swizzle class methods
     * @param cls The class containing the methods
     * @param originalSelector Original method selector
     * @param swizzledSelector Replacement method selector
     * @return True if swizzling succeeded
     */
    static bool SwizzleClassMethod(Class cls, SEL originalSelector, SEL swizzledSelector) {
        if (!cls || !originalSelector || !swizzledSelector) {
            return false;
        }
        
        Method originalMethod = class_getClassMethod(cls, originalSelector);
        Method swizzledMethod = class_getClassMethod(cls, swizzledSelector);
        
        if (!originalMethod || !swizzledMethod) {
            return false;
        }
        
        Class metaClass = objc_getMetaClass(class_getName(cls));
        if (class_addMethod(metaClass, originalSelector, method_getImplementation(swizzledMethod), method_getTypeEncoding(swizzledMethod))) {
            class_replaceMethod(metaClass, swizzledSelector, method_getImplementation(originalMethod), method_getTypeEncoding(originalMethod));
        } else {
            method_exchangeImplementations(originalMethod, swizzledMethod);
        }
        
        return true;
    }
    
    /**
     * @brief Swizzle instance methods
     * @param cls The class containing the methods
     * @param originalSelector Original method selector
     * @param swizzledSelector Replacement method selector
     * @return True if swizzling succeeded
     */
    static bool SwizzleInstanceMethod(Class cls, SEL originalSelector, SEL swizzledSelector) {
        if (!cls || !originalSelector || !swizzledSelector) {
            return false;
        }
        
        Method originalMethod = class_getInstanceMethod(cls, originalSelector);
        Method swizzledMethod = class_getInstanceMethod(cls, swizzledSelector);
        
        if (!originalMethod || !swizzledMethod) {
            return false;
        }
        
        if (class_addMethod(cls, originalSelector, method_getImplementation(swizzledMethod), method_getTypeEncoding(swizzledMethod))) {
            class_replaceMethod(cls, swizzledSelector, method_getImplementation(originalMethod), method_getTypeEncoding(originalMethod));
        } else {
            method_exchangeImplementations(originalMethod, swizzledMethod);
        }
        
        return true;
    }
};

} // namespace iOS

#endif // defined(__APPLE__) || defined(IOS_TARGET)
