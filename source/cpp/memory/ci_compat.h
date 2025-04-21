#pragma once

// CI compatibility header
// This file provides compatibility definitions for continuous integration builds
// where certain platform-specific features may not be available

// Define CI_BUILD when building in CI environment
// #define CI_BUILD

// Disable certain features in CI builds
#ifdef CI_BUILD
    #define DISABLE_MEMORY_SCANNING
    #define DISABLE_HOOKS
    #define DISABLE_JIT
#endif

// Platform detection
#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define PLATFORM_IOS
    #elif TARGET_OS_MAC
        #define PLATFORM_MACOS
    #endif
#elif defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__ANDROID__)
    #define PLATFORM_ANDROID
#elif defined(__linux__)
    #define PLATFORM_LINUX
#endif

// Memory protection utilities for CI compatibility
#ifdef CI_BUILD
    #define MEMORY_PROTECT(addr, size, prot) (void)0
    #define MEMORY_UNPROTECT(addr, size) (void)0
#else
    // Real implementations will be provided elsewhere
    // These are just forward declarations
    void* MEMORY_PROTECT(void* addr, size_t size, int prot);
    bool MEMORY_UNPROTECT(void* addr, size_t size);
#endif
