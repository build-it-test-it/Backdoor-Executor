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

#ifndef _SYS_APPLEAPIOPTS_H_
#define _SYS_APPLEAPIOPTS_H_

/* 
 * These options are being phased out in favor of AvailabilityMacros.h.
 * The API_TO_BE_DEPRECATED macro is being used to mark options that
 * are scheduled to be deprecated for the next release.
 */

#ifdef __APPLE_API_STANDARD
#define __APPLE_API_STANDARD_UNIX_CONFORMANCE 1
#endif /* __APPLE_API_STANDARD */

#ifdef __APPLE_API_STABLE
#define __APPLE_API_EVOLVING 1
#define __APPLE_API_UNSTABLE 1
#define __APPLE_API_PRIVATE 1
#define __APPLE_API_OBSOLETE 1
#endif /* __APPLE_API_STABLE */

#ifdef __APPLE_API_EVOLVING
#define __APPLE_API_UNSTABLE 1
#define __APPLE_API_PRIVATE 1
#define __APPLE_API_OBSOLETE 1
#endif /* __APPLE_API_EVOLVING */

#ifdef __APPLE_API_UNSTABLE
#define __APPLE_API_PRIVATE 1
#define __APPLE_API_OBSOLETE 1
#endif /* __APPLE_API_UNSTABLE */

#ifdef __APPLE_API_PRIVATE
#define __APPLE_API_OBSOLETE 1
#endif /* __APPLE_API_PRIVATE */

#ifdef __APPLE_API_STRICT_CONFORMANCE
#define __APPLE_API_STRICT_CONFORMANCE_UNIX 1
#endif /* __APPLE_API_STRICT_CONFORMANCE */

#endif /* !_SYS_APPLEAPIOPTS_H_ */