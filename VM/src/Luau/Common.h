// Fixed Common.h for compilation purposes
#pragma once

#include <stddef.h>
#include <stdint.h>

// Fast flag system stubs
#define LUAU_FASTFLAGVARIABLE(name) namespace FFlag { bool name = true; }
#define LUAU_FASTFLAG(name) FFlag::name

// Compatibility macros for iOS 15+
#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define LUAU_PLATFORM_IOS 1
#define LUAU_TARGET_IOS 1
#include <dispatch/dispatch.h>
#endif
#endif

namespace Luau {
    // Common types and utilities
    typedef int32_t int32;
    typedef uint32_t uint32;
    typedef int64_t int64;
    typedef uint64_t uint64;

    // Common macros
    #define LUAU_ASSERT(x) ((void)0)
    #define LUAU_UNREACHABLE() ((void)0)
    #define LUAU_NOINLINE __attribute__((noinline))
    #define LUAU_FORCEINLINE inline __attribute__((always_inline))
    #define LUAU_NORETURN __attribute__((noreturn))

    // Memory management helpers for iOS compatibility
    inline void* luau_malloc(size_t size) {
        return ::malloc(size);
    }
    
    inline void luau_free(void* ptr) {
        ::free(ptr);
    }
    
    // iOS compatibility utilities
    #if defined(LUAU_PLATFORM_IOS)
    inline bool isRunningOnSimulator() {
        #if TARGET_IPHONE_SIMULATOR
        return true;
        #else
        return false;
        #endif
    }
    #endif
}
