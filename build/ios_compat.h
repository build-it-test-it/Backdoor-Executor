// Master compatibility header for iOS frameworks in CI builds
#pragma once

// Define CI_BUILD
#ifndef CI_BUILD
#define CI_BUILD
#endif

// Special macros for conditional compilation
#define IOS_CODE(code) do { /* iOS code skipped in CI build */ } while(0)
#define IOS_CODE_ELSE(ios_code, ci_code) ci_code

// Include compatibility headers
#include <ios_compat/Foundation.h>
#include <ios_compat/UIKit.h>
#include <ios_compat/objc_runtime.h>
#include <ios_compat/mach_vm.h>

// Stub ObjC syntax for CI builds
#define NS_ASSUME_NONNULL_BEGIN
#define NS_ASSUME_NONNULL_END
#define NS_SWIFT_NAME(name)
#define NS_REFINED_FOR_SWIFT
#define NS_SWIFT_UNAVAILABLE(msg)
#define API_AVAILABLE(...)
#define API_UNAVAILABLE(...)

// Stub @directives
#define @interface struct
#define @end };
#define @implementation // no-op
#define @property // no-op
#define @protocol(x) (void*)0
#define @selector(x) sel_registerName(#x)

// String literals
#define @"string" "string"

// ObjC objects
#define @[] nullptr
#define @{} nullptr

// Block syntax (this is a complex transformation but simplified for CI)
#define 
^
 [=]

// Import directives become includes in CI build
#define import include
