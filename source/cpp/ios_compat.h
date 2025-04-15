// Master compatibility header for iOS frameworks
#pragma once

// Standard includes
#include <string>
#include <vector>
#include <functional>
#include <memory>

// For iOS builds, include the actual frameworks
#ifdef __APPLE__
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <Security/Security.h>
#import <SystemConfiguration/SystemConfiguration.h>

// Define our platform identification
#define IOS_TARGET
#endif

// Real macros for iOS code execution
#define IOS_CODE(code) do { code } while(0)
#define IOS_CODE_ELSE(ios_code, ci_code) ios_code

// Real ObjC syntax definitions (these are handled natively on iOS)
#ifdef __APPLE__
// These are defined by the native iOS frameworks
#else
// Fallback definitions for non-Apple platforms during compilation
#define NS_ASSUME_NONNULL_BEGIN
#define NS_ASSUME_NONNULL_END
#define NS_SWIFT_NAME(name)
#define NS_REFINED_FOR_SWIFT
#define NS_SWIFT_UNAVAILABLE(msg)
#define API_AVAILABLE(...)
#define API_UNAVAILABLE(...)
#endif

// Helper macros for iOS versioning
#ifdef __APPLE__
  #define IOS_VERSION_GREATER_THAN_OR_EQUAL_TO(v) ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
  #define IOS_VERSION_LESS_THAN(v) ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
#else
  // Default implementations for non-Apple platforms
  #define IOS_VERSION_GREATER_THAN_OR_EQUAL_TO(v) (false)
  #define IOS_VERSION_LESS_THAN(v) (true)
#endif

// Define iOS version availability
#define IOS_AVAILABLE __attribute__((availability(ios,introduced=13.0)))
#define IOS_DEPRECATED __attribute__((availability(ios,deprecated=15.0)))

// Thread safety annotations
#define THREAD_SAFE __attribute__((thread_safety_analysis))
#define REQUIRES_LOCK(x) __attribute__((requires_lock(x)))
