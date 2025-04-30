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

#ifndef _SYS__ENDIAN_H_
#define _SYS__ENDIAN_H_

#include <sys/cdefs.h>

#define __DARWIN_BYTE_ORDER __DARWIN_LITTLE_ENDIAN
#define __DARWIN_LITTLE_ENDIAN 1234
#define __DARWIN_BIG_ENDIAN 4321

#if defined(__GNUC__)

#if defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
/* Little endian */
#define __DARWIN_BYTE_ORDER __DARWIN_LITTLE_ENDIAN
#elif !defined(__LITTLE_ENDIAN__) && defined(__BIG_ENDIAN__)
/* Big endian */
#define __DARWIN_BYTE_ORDER __DARWIN_BIG_ENDIAN
#else
#error "Both __LITTLE_ENDIAN__ and __BIG_ENDIAN__ cannot be defined simultaneously"
#endif

#else /* !__GNUC__ */

#include <machine/endian.h>
#if defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)
/* Little endian */
#define __DARWIN_BYTE_ORDER __DARWIN_LITTLE_ENDIAN
#elif !defined(_LITTLE_ENDIAN) && defined(_BIG_ENDIAN)
/* Big endian */
#define __DARWIN_BYTE_ORDER __DARWIN_BIG_ENDIAN
#else
#error "Either _LITTLE_ENDIAN or _BIG_ENDIAN must be defined"
#endif

#endif /* __GNUC__ */

#endif /* _SYS__ENDIAN_H_ */