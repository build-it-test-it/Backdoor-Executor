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
#ifndef _SYS__TYPES_H_
#define _SYS__TYPES_H_

#include <sys/cdefs.h>
#include <machine/_types.h>

/* Forward declarations */
struct mcontext;
struct mcontext64;
struct __darwin_mcontext;
struct __darwin_mcontext64;

#if __DARWIN_UNIX03
typedef __darwin_mcontext *mcontext_t;
typedef __darwin_mcontext64 *mcontext64_t;
#else /* !__DARWIN_UNIX03 */
typedef struct mcontext *mcontext_t;
typedef struct mcontext64 *mcontext64_t;
#endif /* __DARWIN_UNIX03 */

#ifndef _INTPTR_T
#define _INTPTR_T
typedef __darwin_intptr_t        intptr_t;
#endif

#ifndef _UINTPTR_T
#define _UINTPTR_T
typedef unsigned long           uintptr_t;
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef __darwin_size_t        size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef __darwin_ssize_t        ssize_t;
#endif

#ifndef _INT8_T
#define _INT8_T
typedef signed char           int8_t;
#endif
#ifndef _INT16_T
#define _INT16_T
typedef short                int16_t;
#endif
#ifndef _INT32_T
#define _INT32_T
typedef int                 int32_t;
#endif
#ifndef _INT64_T
#define _INT64_T
typedef long long           int64_t;
#endif

#ifndef _UINT8_T
#define _UINT8_T
typedef unsigned char       uint8_t;
#endif
#ifndef _UINT16_T
#define _UINT16_T
typedef unsigned short      uint16_t;
#endif
#ifndef _UINT32_T
#define _UINT32_T
typedef unsigned int        uint32_t;
#endif
#ifndef _UINT64_T
#define _UINT64_T
typedef unsigned long long  uint64_t;
#endif

#ifndef _INTMAX_T
#define _INTMAX_T
typedef long long           intmax_t;
#endif
#ifndef _UINTMAX_T
#define _UINTMAX_T
typedef unsigned long long  uintmax_t;
#endif

#ifndef _CLOCK_T
#define _CLOCK_T
typedef __darwin_clock_t    clock_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef __darwin_time_t     time_t;
#endif

#ifndef _USECONDS_T
#define _USECONDS_T
typedef __darwin_useconds_t useconds_t;
#endif

#ifndef _SUSECONDS_T
#define _SUSECONDS_T
typedef __darwin_suseconds_t suseconds_t;
#endif

#ifndef _RSIZE_T
#define _RSIZE_T
typedef __darwin_size_t        rsize_t;
#endif

#ifndef _ERRNO_T
#define _ERRNO_T
typedef int                 errno_t;
#endif

#endif /* _SYS__TYPES_H_ */