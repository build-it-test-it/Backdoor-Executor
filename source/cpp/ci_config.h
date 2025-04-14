#pragma once

/**
 * @file ci_config.h
 * @brief Configuration macros for handling CI builds vs real iOS builds
 */

// Detect CI build from CMake or manual definition
#if defined(CI_BUILD) || defined(BUILD_CI)
    #define IS_CI_BUILD 1
#else
    #define IS_CI_BUILD 0
#endif

/**
 * @def IOS_CODE(code)
 * @brief Macro for iOS-specific code that shouldn't run in CI
 * 
 * This macro helps conditionally compile iOS-specific code.
 * In CI builds, it evaluates to a stub implementation or empty block.
 * In real iOS builds, it uses the actual implementation.
 */
#if IS_CI_BUILD
    #define IOS_CODE(code) do { /* stub for CI */ } while(0)
#else
    #define IOS_CODE(code) code
#endif

/**
 * @def IOS_CODE_ELSE(ios_code, ci_code)
 * @brief Macro for iOS-specific code with alternative CI implementation
 * 
 * This macro helps conditionally compile iOS-specific code with
 * an alternative implementation for CI builds.
 */
#if IS_CI_BUILD
    #define IOS_CODE_ELSE(ios_code, ci_code) ci_code
#else
    #define IOS_CODE_ELSE(ios_code, ci_code) ios_code
#endif
