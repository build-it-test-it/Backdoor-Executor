#pragma once

/**
 * @file ci_config.h
 * @brief Configuration macros for iOS builds
 */

// Always use real implementation
#define IS_CI_BUILD 0

/**
 * @def IOS_CODE(code)
 * @brief Macro for iOS-specific code
 * 
 * This macro helps conditionally compile iOS-specific code.
 * In real iOS builds, it uses the actual implementation.
 */
#define IOS_CODE(code) code

/**
 * @def IOS_CODE_ELSE(ios_code, ci_code)
 * @brief Macro for iOS-specific code with alternative implementation
 * 
 * This macro helps conditionally compile iOS-specific code with
 * an alternative implementation.
 */
#define IOS_CODE_ELSE(ios_code, ci_code) ios_code
