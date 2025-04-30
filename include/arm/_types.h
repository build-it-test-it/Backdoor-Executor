/*
 * Copyright (c) 2003-2012 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 *
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
#ifndef _ARM__TYPES_H_
#define _ARM__TYPES_H_

#if defined (__arm64__)
typedef int                       __darwin_ct_rune_t;     /* ct_rune_t */
typedef unsigned int              __darwin_wchar_t;       /* wchar_t */
typedef unsigned int              __darwin_rune_t;        /* rune_t */
typedef unsigned int              __darwin_wint_t;        /* wint_t */
typedef unsigned int              __darwin_clock_t;       /* clock_t */
typedef unsigned int              __darwin_socklen_t;     /* socklen_t */
typedef long                      __darwin_ssize_t;       /* ssize_t */
typedef long                      __darwin_time_t;        /* time_t */
typedef long                      __darwin_intptr_t;      /* intptr_t */
typedef unsigned long             __darwin_size_t;        /* size_t */
typedef unsigned long             __darwin_uintptr_t;     /* uintptr_t */
#elif defined (__arm__)
typedef int                       __darwin_ct_rune_t;     /* ct_rune_t */
typedef unsigned int              __darwin_wchar_t;       /* wchar_t */
typedef unsigned int              __darwin_rune_t;        /* rune_t */
typedef unsigned int              __darwin_wint_t;        /* wint_t */
typedef unsigned int              __darwin_clock_t;       /* clock_t */
typedef unsigned int              __darwin_socklen_t;     /* socklen_t */
typedef int                       __darwin_ssize_t;       /* ssize_t */
typedef long                      __darwin_time_t;        /* time_t */
typedef int                       __darwin_intptr_t;      /* intptr_t */
typedef unsigned int              __darwin_size_t;        /* size_t */
typedef unsigned int              __darwin_uintptr_t;     /* uintptr_t */
#else
#error Unknown architecture
#endif

#endif /* _ARM__TYPES_H_ */