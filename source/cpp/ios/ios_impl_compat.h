// ios_impl_compat.h - Helper header for iOS implementation files
#pragma once

// Include standard headers
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

// Include our main compatibility headers
#include "../ios_compat.h"
#include "objc_isolation.h"
#include "../logging.hpp"

// Common macros for Objective-C++ implementations
#define SAFE_OBJC_CAST(Type, obj) (static_cast<Type>(obj))
#define WEAK_SELF __weak typeof(self) weakSelf = self

// Implementation-specific utilities
namespace iOS {
    // Utility to safely execute code on main thread
    template<typename Func>
    static void executeOnMainThread(Func&& func) {
        if ([NSThread isMainThread]) {
            func();
        } else {
            dispatch_async(dispatch_get_main_queue(), ^{
                func();
            });
        }
    }
    
    // Error handling for Objective-C errors
    inline void handleObjCError(NSError* error, const std::string& context) {
        if (error) {
            std::string errorMsg = [error.localizedDescription UTF8String];
            Logging::LogError("ObjC", context + ": " + errorMsg);
        }
    }
    
    // Convert NSArray to vector for common types
    template<typename T>
    std::vector<T> NSArrayToVector(NSArray* array);
    
    // Specialization for NSString to std::string
    template<>
    inline std::vector<std::string> NSArrayToVector<std::string>(NSArray* array) {
        std::vector<std::string> result;
        if (!array) return result;
        
        NSUInteger count = [array count];
        result.reserve(count);
        
        for (NSUInteger i = 0; i < count; i++) {
            NSString* str = array[i];
            result.push_back([str UTF8String]);
        }
        
        return result;
    }
    
    // Convert vector to NSArray for common types
    template<typename T>
    NSArray* VectorToNSArray(const std::vector<T>& vec);
    
    // Specialization for std::string to NSString
    template<>
    inline NSArray* VectorToNSArray<std::string>(const std::vector<std::string>& vec) {
        NSMutableArray* result = [NSMutableArray arrayWithCapacity:vec.size()];
        
        for (const auto& str : vec) {
            [result addObject:[NSString stringWithUTF8String:str.c_str()]];
        }
        
        return result;
    }
}

// Include other implementation-specific headers needed by iOS files
#include "../dobby_wrapper.cpp"
#include "../memory/mem.hpp"
