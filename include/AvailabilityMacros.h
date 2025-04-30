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

#ifndef __AVAILABILITY_MACROS__
#define __AVAILABILITY_MACROS__

/* iOS version defines */
#define __IPHONE_2_0      20000
#define __IPHONE_2_1      20100
#define __IPHONE_2_2      20200
#define __IPHONE_3_0      30000
#define __IPHONE_3_1      30100
#define __IPHONE_3_2      30200
#define __IPHONE_4_0      40000
#define __IPHONE_4_1      40100
#define __IPHONE_4_2      40200
#define __IPHONE_4_3      40300
#define __IPHONE_5_0      50000
#define __IPHONE_5_1      50100
#define __IPHONE_6_0      60000
#define __IPHONE_6_1      60100
#define __IPHONE_7_0      70000
#define __IPHONE_7_1      70100
#define __IPHONE_8_0      80000
#define __IPHONE_8_1      80100
#define __IPHONE_8_2      80200
#define __IPHONE_8_3      80300
#define __IPHONE_8_4      80400
#define __IPHONE_9_0      90000
#define __IPHONE_9_1      90100
#define __IPHONE_9_2      90200
#define __IPHONE_9_3      90300
#define __IPHONE_10_0    100000
#define __IPHONE_10_1    100100
#define __IPHONE_10_2    100200
#define __IPHONE_10_3    100300
#define __IPHONE_11_0    110000
#define __IPHONE_11_1    110100
#define __IPHONE_11_2    110200
#define __IPHONE_11_3    110300
#define __IPHONE_11_4    110400
#define __IPHONE_12_0    120000
#define __IPHONE_12_1    120100
#define __IPHONE_12_2    120200
#define __IPHONE_12_3    120300
#define __IPHONE_12_4    120400
#define __IPHONE_13_0    130000
#define __IPHONE_13_1    130100
#define __IPHONE_13_2    130200
#define __IPHONE_13_3    130300
#define __IPHONE_13_4    130400
#define __IPHONE_13_5    130500
#define __IPHONE_13_6    130600
#define __IPHONE_13_7    130700
#define __IPHONE_14_0    140000
#define __IPHONE_14_1    140100
#define __IPHONE_14_2    140200
#define __IPHONE_14_3    140300
#define __IPHONE_14_4    140400
#define __IPHONE_14_5    140500
#define __IPHONE_14_6    140600
#define __IPHONE_14_7    140700
#define __IPHONE_14_8    140800
#define __IPHONE_15_0    150000
#define __IPHONE_15_1    150100
#define __IPHONE_15_2    150200
#define __IPHONE_15_3    150300
#define __IPHONE_15_4    150400
#define __IPHONE_15_5    150500
#define __IPHONE_15_6    150600
#define __IPHONE_15_7    150700
#define __IPHONE_16_0    160000
#define __IPHONE_16_1    160100
#define __IPHONE_16_2    160200
#define __IPHONE_16_3    160300
#define __IPHONE_16_4    160400
#define __IPHONE_16_5    160500
#define __IPHONE_16_6    160600
#define __IPHONE_16_7    160700
#define __IPHONE_17_0    170000
#define __IPHONE_17_1    170100
#define __IPHONE_17_2    170200
#define __IPHONE_17_3    170300
#define __IPHONE_17_4    170400
#define __IPHONE_18_0    180000
#define __IPHONE_18_1    180100
#define __IPHONE_18_2    180200

/* API availability macros */
#if defined(__has_feature) && defined(__has_attribute)
 #if __has_attribute(availability)
  #define __API_AVAILABLE(...) __attribute__((availability(__VA_ARGS__)))
  #define __API_DEPRECATED(...) __attribute__((availability(__VA_ARGS__)))
  #define __API_DEPRECATED_WITH_REPLACEMENT(...) __attribute__((availability(__VA_ARGS__)))
  #define __API_UNAVAILABLE(...) __attribute__((availability(__VA_ARGS__)))
 #endif
#endif

#ifndef __API_AVAILABLE
 #define __API_AVAILABLE(...)
#endif

#ifndef __API_DEPRECATED
 #define __API_DEPRECATED(...)
#endif

#ifndef __API_DEPRECATED_WITH_REPLACEMENT
 #define __API_DEPRECATED_WITH_REPLACEMENT(...)
#endif

#ifndef __API_UNAVAILABLE
 #define __API_UNAVAILABLE(...)
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.0, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.1, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.2, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.3, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.4, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.5, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_6_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.6, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_6_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_7_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.7, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_7_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_8_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.8, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_8_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_9_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.9, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_9_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.10, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.11, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.12, 
 * and will be available on all Mac OS X versions.
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_0_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.0, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_0_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_1_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.1, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_1_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_2_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.2, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_2_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_3_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.3, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_3_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_4_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.4, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_4_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_5_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.5, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_5_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_6_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.6, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_6_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.7, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_8_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.8, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_8_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_9_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.9, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_9_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_10_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.10, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_10_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_11_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.11, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_11_AND_LATER

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_12_AND_LATER
 * 
 * Used on declarations that were deprecated in Mac OS X 10.12, 
 * and will be deprecated on all Mac OS X versions.
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_12_AND_LATER

/* iOS availability macros */
#define __AVAILABILITY_INTERNAL__IPHONE_3_0 
#define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_0
#define __AVAILABILITY_INTERNAL__IPHONE_3_1
#define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_3_1
#define __AVAILABILITY_INTERNAL__IPHONE_3_2
#define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_3_2
#define __AVAILABILITY_INTERNAL__IPHONE_4_0
#define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_0
#define __AVAILABILITY_INTERNAL__IPHONE_4_1
#define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_1
#define __AVAILABILITY_INTERNAL__IPHONE_4_2
#define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_4_2
#define __AVAILABILITY_INTERNAL__IPHONE_4_3
#define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_4_3
#define __AVAILABILITY_INTERNAL__IPHONE_5_0
#define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_5_0
#define __AVAILABILITY_INTERNAL__IPHONE_5_1
#define __AVAILABILITY_INTERNAL__IPHONE_5_1_DEP__IPHONE_5_1
#define __AVAILABILITY_INTERNAL__IPHONE_6_0
#define __AVAILABILITY_INTERNAL__IPHONE_6_0_DEP__IPHONE_6_0
#define __AVAILABILITY_INTERNAL__IPHONE_6_1
#define __AVAILABILITY_INTERNAL__IPHONE_6_1_DEP__IPHONE_6_1
#define __AVAILABILITY_INTERNAL__IPHONE_7_0
#define __AVAILABILITY_INTERNAL__IPHONE_7_0_DEP__IPHONE_7_0
#define __AVAILABILITY_INTERNAL__IPHONE_7_1
#define __AVAILABILITY_INTERNAL__IPHONE_7_1_DEP__IPHONE_7_1
#define __AVAILABILITY_INTERNAL__IPHONE_8_0
#define __AVAILABILITY_INTERNAL__IPHONE_8_0_DEP__IPHONE_8_0
#define __AVAILABILITY_INTERNAL__IPHONE_8_1
#define __AVAILABILITY_INTERNAL__IPHONE_8_1_DEP__IPHONE_8_1
#define __AVAILABILITY_INTERNAL__IPHONE_8_2
#define __AVAILABILITY_INTERNAL__IPHONE_8_2_DEP__IPHONE_8_2
#define __AVAILABILITY_INTERNAL__IPHONE_8_3
#define __AVAILABILITY_INTERNAL__IPHONE_8_3_DEP__IPHONE_8_3
#define __AVAILABILITY_INTERNAL__IPHONE_8_4
#define __AVAILABILITY_INTERNAL__IPHONE_8_4_DEP__IPHONE_8_4
#define __AVAILABILITY_INTERNAL__IPHONE_9_0
#define __AVAILABILITY_INTERNAL__IPHONE_9_0_DEP__IPHONE_9_0
#define __AVAILABILITY_INTERNAL__IPHONE_9_1
#define __AVAILABILITY_INTERNAL__IPHONE_9_1_DEP__IPHONE_9_1
#define __AVAILABILITY_INTERNAL__IPHONE_9_2
#define __AVAILABILITY_INTERNAL__IPHONE_9_2_DEP__IPHONE_9_2
#define __AVAILABILITY_INTERNAL__IPHONE_9_3
#define __AVAILABILITY_INTERNAL__IPHONE_9_3_DEP__IPHONE_9_3
#define __AVAILABILITY_INTERNAL__IPHONE_10_0
#define __AVAILABILITY_INTERNAL__IPHONE_10_0_DEP__IPHONE_10_0
#define __AVAILABILITY_INTERNAL__IPHONE_10_1
#define __AVAILABILITY_INTERNAL__IPHONE_10_1_DEP__IPHONE_10_1
#define __AVAILABILITY_INTERNAL__IPHONE_10_2
#define __AVAILABILITY_INTERNAL__IPHONE_10_2_DEP__IPHONE_10_2
#define __AVAILABILITY_INTERNAL__IPHONE_10_3
#define __AVAILABILITY_INTERNAL__IPHONE_10_3_DEP__IPHONE_10_3
#define __AVAILABILITY_INTERNAL__IPHONE_11_0
#define __AVAILABILITY_INTERNAL__IPHONE_11_0_DEP__IPHONE_11_0
#define __AVAILABILITY_INTERNAL__IPHONE_11_1
#define __AVAILABILITY_INTERNAL__IPHONE_11_1_DEP__IPHONE_11_1
#define __AVAILABILITY_INTERNAL__IPHONE_11_2
#define __AVAILABILITY_INTERNAL__IPHONE_11_2_DEP__IPHONE_11_2
#define __AVAILABILITY_INTERNAL__IPHONE_11_3
#define __AVAILABILITY_INTERNAL__IPHONE_11_3_DEP__IPHONE_11_3
#define __AVAILABILITY_INTERNAL__IPHONE_11_4
#define __AVAILABILITY_INTERNAL__IPHONE_11_4_DEP__IPHONE_11_4
#define __AVAILABILITY_INTERNAL__IPHONE_12_0
#define __AVAILABILITY_INTERNAL__IPHONE_12_0_DEP__IPHONE_12_0
#define __AVAILABILITY_INTERNAL__IPHONE_12_1
#define __AVAILABILITY_INTERNAL__IPHONE_12_1_DEP__IPHONE_12_1
#define __AVAILABILITY_INTERNAL__IPHONE_12_2
#define __AVAILABILITY_INTERNAL__IPHONE_12_2_DEP__IPHONE_12_2
#define __AVAILABILITY_INTERNAL__IPHONE_12_3
#define __AVAILABILITY_INTERNAL__IPHONE_12_3_DEP__IPHONE_12_3
#define __AVAILABILITY_INTERNAL__IPHONE_12_4
#define __AVAILABILITY_INTERNAL__IPHONE_12_4_DEP__IPHONE_12_4
#define __AVAILABILITY_INTERNAL__IPHONE_13_0
#define __AVAILABILITY_INTERNAL__IPHONE_13_0_DEP__IPHONE_13_0
#define __AVAILABILITY_INTERNAL__IPHONE_13_1
#define __AVAILABILITY_INTERNAL__IPHONE_13_1_DEP__IPHONE_13_1
#define __AVAILABILITY_INTERNAL__IPHONE_13_2
#define __AVAILABILITY_INTERNAL__IPHONE_13_2_DEP__IPHONE_13_2
#define __AVAILABILITY_INTERNAL__IPHONE_13_3
#define __AVAILABILITY_INTERNAL__IPHONE_13_3_DEP__IPHONE_13_3
#define __AVAILABILITY_INTERNAL__IPHONE_13_4
#define __AVAILABILITY_INTERNAL__IPHONE_13_4_DEP__IPHONE_13_4
#define __AVAILABILITY_INTERNAL__IPHONE_13_5
#define __AVAILABILITY_INTERNAL__IPHONE_13_5_DEP__IPHONE_13_5
#define __AVAILABILITY_INTERNAL__IPHONE_13_6
#define __AVAILABILITY_INTERNAL__IPHONE_13_6_DEP__IPHONE_13_6
#define __AVAILABILITY_INTERNAL__IPHONE_13_7
#define __AVAILABILITY_INTERNAL__IPHONE_13_7_DEP__IPHONE_13_7
#define __AVAILABILITY_INTERNAL__IPHONE_14_0
#define __AVAILABILITY_INTERNAL__IPHONE_14_0_DEP__IPHONE_14_0
#define __AVAILABILITY_INTERNAL__IPHONE_14_1
#define __AVAILABILITY_INTERNAL__IPHONE_14_1_DEP__IPHONE_14_1
#define __AVAILABILITY_INTERNAL__IPHONE_14_2
#define __AVAILABILITY_INTERNAL__IPHONE_14_2_DEP__IPHONE_14_2
#define __AVAILABILITY_INTERNAL__IPHONE_14_3
#define __AVAILABILITY_INTERNAL__IPHONE_14_3_DEP__IPHONE_14_3
#define __AVAILABILITY_INTERNAL__IPHONE_14_4
#define __AVAILABILITY_INTERNAL__IPHONE_14_4_DEP__IPHONE_14_4
#define __AVAILABILITY_INTERNAL__IPHONE_14_5
#define __AVAILABILITY_INTERNAL__IPHONE_14_5_DEP__IPHONE_14_5
#define __AVAILABILITY_INTERNAL__IPHONE_14_6
#define __AVAILABILITY_INTERNAL__IPHONE_14_6_DEP__IPHONE_14_6
#define __AVAILABILITY_INTERNAL__IPHONE_14_7
#define __AVAILABILITY_INTERNAL__IPHONE_14_7_DEP__IPHONE_14_7
#define __AVAILABILITY_INTERNAL__IPHONE_14_8
#define __AVAILABILITY_INTERNAL__IPHONE_14_8_DEP__IPHONE_14_8
#define __AVAILABILITY_INTERNAL__IPHONE_15_0
#define __AVAILABILITY_INTERNAL__IPHONE_15_0_DEP__IPHONE_15_0
#define __AVAILABILITY_INTERNAL__IPHONE_15_1
#define __AVAILABILITY_INTERNAL__IPHONE_15_1_DEP__IPHONE_15_1
#define __AVAILABILITY_INTERNAL__IPHONE_15_2
#define __AVAILABILITY_INTERNAL__IPHONE_15_2_DEP__IPHONE_15_2
#define __AVAILABILITY_INTERNAL__IPHONE_15_3
#define __AVAILABILITY_INTERNAL__IPHONE_15_3_DEP__IPHONE_15_3
#define __AVAILABILITY_INTERNAL__IPHONE_15_4
#define __AVAILABILITY_INTERNAL__IPHONE_15_4_DEP__IPHONE_15_4
#define __AVAILABILITY_INTERNAL__IPHONE_15_5
#define __AVAILABILITY_INTERNAL__IPHONE_15_5_DEP__IPHONE_15_5
#define __AVAILABILITY_INTERNAL__IPHONE_15_6
#define __AVAILABILITY_INTERNAL__IPHONE_15_6_DEP__IPHONE_15_6
#define __AVAILABILITY_INTERNAL__IPHONE_15_7
#define __AVAILABILITY_INTERNAL__IPHONE_15_7_DEP__IPHONE_15_7
#define __AVAILABILITY_INTERNAL__IPHONE_16_0
#define __AVAILABILITY_INTERNAL__IPHONE_16_0_DEP__IPHONE_16_0
#define __AVAILABILITY_INTERNAL__IPHONE_16_1
#define __AVAILABILITY_INTERNAL__IPHONE_16_1_DEP__IPHONE_16_1
#define __AVAILABILITY_INTERNAL__IPHONE_16_2
#define __AVAILABILITY_INTERNAL__IPHONE_16_2_DEP__IPHONE_16_2
#define __AVAILABILITY_INTERNAL__IPHONE_16_3
#define __AVAILABILITY_INTERNAL__IPHONE_16_3_DEP__IPHONE_16_3
#define __AVAILABILITY_INTERNAL__IPHONE_16_4
#define __AVAILABILITY_INTERNAL__IPHONE_16_4_DEP__IPHONE_16_4
#define __AVAILABILITY_INTERNAL__IPHONE_16_5
#define __AVAILABILITY_INTERNAL__IPHONE_16_5_DEP__IPHONE_16_5
#define __AVAILABILITY_INTERNAL__IPHONE_16_6
#define __AVAILABILITY_INTERNAL__IPHONE_16_6_DEP__IPHONE_16_6
#define __AVAILABILITY_INTERNAL__IPHONE_16_7
#define __AVAILABILITY_INTERNAL__IPHONE_16_7_DEP__IPHONE_16_7
#define __AVAILABILITY_INTERNAL__IPHONE_17_0
#define __AVAILABILITY_INTERNAL__IPHONE_17_0_DEP__IPHONE_17_0
#define __AVAILABILITY_INTERNAL__IPHONE_17_1
#define __AVAILABILITY_INTERNAL__IPHONE_17_1_DEP__IPHONE_17_1
#define __AVAILABILITY_INTERNAL__IPHONE_17_2
#define __AVAILABILITY_INTERNAL__IPHONE_17_2_DEP__IPHONE_17_2
#define __AVAILABILITY_INTERNAL__IPHONE_17_3
#define __AVAILABILITY_INTERNAL__IPHONE_17_3_DEP__IPHONE_17_3
#define __AVAILABILITY_INTERNAL__IPHONE_17_4
#define __AVAILABILITY_INTERNAL__IPHONE_17_4_DEP__IPHONE_17_4
#define __AVAILABILITY_INTERNAL__IPHONE_18_0
#define __AVAILABILITY_INTERNAL__IPHONE_18_0_DEP__IPHONE_18_0
#define __AVAILABILITY_INTERNAL__IPHONE_18_1
#define __AVAILABILITY_INTERNAL__IPHONE_18_1_DEP__IPHONE_18_1
#define __AVAILABILITY_INTERNAL__IPHONE_18_2
#define __AVAILABILITY_INTERNAL__IPHONE_18_2_DEP__IPHONE_18_2

#define __AVAILABILITY_INTERNAL_REGULAR

#endif /* __AVAILABILITY_MACROS__ */