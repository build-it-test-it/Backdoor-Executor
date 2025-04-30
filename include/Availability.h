/*
 * Copyright (c) 2007-2016 by Apple Inc.. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef __AVAILABILITY__
#define __AVAILABILITY__

/* 
 * API Availability macros
 * 
 * These macros are used to mark API as available or unavailable on
 * certain OS versions.
 */

#ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
    #define __IOS_AVAILABLE(x) __attribute__((availability(ios,introduced=x)))
    #define __IOS_DEPRECATED(x,y) __attribute__((availability(ios,introduced=x,deprecated=y)))
    #define __OSX_AVAILABLE(x)
    #define __OSX_DEPRECATED(x,y)
#else
    #define __IOS_AVAILABLE(x)
    #define __IOS_DEPRECATED(x,y)
    #define __OSX_AVAILABLE(x) __attribute__((availability(macosx,introduced=x)))
    #define __OSX_DEPRECATED(x,y) __attribute__((availability(macosx,introduced=x,deprecated=y)))
#endif

/* 
 * Macros for defining which versions/platform a given symbol can be used.
 *
 * @see http://clang.llvm.org/docs/AttributeReference.html#availability
 */
 
/*
 * For iOS availability specify a version number in the range [major].[minor]
 * Range is in the format [introduced]-[deprecated] with the version number.
 */
#define __API_AVAILABLE(...) __attribute__((availability(__VA_ARGS__)))
#define __API_DEPRECATED(...) __attribute__((availability(__VA_ARGS__)))
#define __API_UNAVAILABLE(...) __attribute__((availability(__VA_ARGS__)))

/* Compatibility with older versions of availability macros */
#define __OSX_AVAILABLE_STARTING(_osx, _ios) __API_AVAILABLE(macos(_osx), ios(_ios))
#define __OSX_AVAILABLE_BUT_DEPRECATED(_osxIntro, _osxDep, _iosIntro, _iosDep) \
    __API_DEPRECATED("No longer supported", macos(_osxIntro,_osxDep), ios(_iosIntro,_iosDep))

/* 
 * Macros for defining nullability of parameters and return values
 */
#ifndef __has_feature
    #define __has_feature(x) 0
#endif

#if __has_feature(nullability)
    #define _Nullable nullable
    #define _Nonnull nonnull
    #define _Null_unspecified null_unspecified
#else
    #define _Nullable
    #define _Nonnull
    #define _Null_unspecified
#endif

#endif /* __AVAILABILITY__ */