/*
 * Copyright (c) 2000-2018 Apple Inc. All rights reserved.
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
/*
 * Copyright 1995 NeXT Computer, Inc. All rights reserved.
 */
/*
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.3 (Berkeley) 1/5/94
 */

#ifndef	_MACHTYPES_H_
#define	_MACHTYPES_H_

#ifndef __ASSEMBLER__

#include <arm/_types.h>
#include <sys/_types/_va_list.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/_types/_time_t.h>
#include <sys/_types/_timespec.h>
#include <sys/_types/_time_value.h>

#define __darwin_arm_thread_state64_t __arm64_thread_state64_t
#define __darwin_arm_thread_state_t __arm_thread_state_t
#define __darwin_arm_exception_state_t __arm_exception_state_t
#define __darwin_arm_exception_state64_t __arm64_exception_state64_t
#define __darwin_arm_debug_state_t __arm_debug_state_t
#define __darwin_arm_debug_state32_t __arm_debug_state32_t
#define __darwin_arm_debug_state64_t __arm_debug_state64_t
#define __darwin_arm_neon_state64_t __arm64_neon_state64_t
#define __darwin_arm_neon_state_t __arm_neon_state_t
#define __darwin_arm_vfp_state_t __arm_vfp_state_t

#define __DARWIN_OPAQUE_ARM_THREAD_STATE64 1
#define __DARWIN_OPAQUE_ARM_THREAD_STATE 1
#define __DARWIN_OPAQUE_ARM_EXCEPTION_STATE 1
#define __DARWIN_OPAQUE_ARM_EXCEPTION_STATE64 1
#define __DARWIN_OPAQUE_ARM_DEBUG_STATE 1
#define __DARWIN_OPAQUE_ARM_DEBUG_STATE32 1
#define __DARWIN_OPAQUE_ARM_DEBUG_STATE64 1
#define __DARWIN_OPAQUE_ARM_NEON_STATE64 1
#define __DARWIN_OPAQUE_ARM_NEON_STATE 1
#define __DARWIN_OPAQUE_ARM_VFP_STATE 1

#if defined (__arm64__)
typedef unsigned long           register_t;
#elif defined (__arm__)
typedef unsigned int            register_t;
#else
#error Unknown architecture
#endif

#if defined (__arm64__)
typedef unsigned long           user_addr_t;
typedef unsigned long           user_size_t;
typedef long                    user_ssize_t;
typedef unsigned long long      user_offset_t;
typedef unsigned int            user_id_t;
typedef unsigned int            user_long_t;
typedef unsigned int            user_ulong_t;
typedef unsigned int            user_time_t;
typedef long long               user_off_t;
typedef unsigned long long      user_addr_t;
#elif defined (__arm__)
typedef uint32_t                user_addr_t;
typedef uint32_t                user_size_t;
typedef int32_t                 user_ssize_t;
typedef int64_t                 user_offset_t;
typedef uint32_t                user_id_t;
typedef uint32_t                user_long_t;
typedef uint32_t                user_ulong_t;
typedef uint32_t                user_time_t;
typedef int64_t                 user_off_t;
#else
#error Unknown architecture
#endif

#ifdef KERNEL
typedef int64_t                 user_long_t;
typedef uint64_t                user_ulong_t;
#endif /* KERNEL */

/* This defines the size of syscall arguments after copying into the kernel: */
typedef user_addr_t             syscall_arg_t;

#endif /* __ASSEMBLER__ */

#endif	/* _MACHTYPES_H_ */