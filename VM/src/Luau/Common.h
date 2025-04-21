// Stub Common.h for compilation purposes
#pragma once

#include <stdint.h>
#include <stddef.h>

// Fast flag system stubs
#define LUAU_FASTFLAGVARIABLE(name) namespace FFlag { bool name = true; }
#define LUAU_FASTFLAG(name) FFlag::name

namespace Luau {
    // Common types and utilities
    typedef int32_t int32;
    typedef uint32_t uint32;
    typedef int64_t int64;
    typedef uint64_t uint64;

    // Common macros
    #define LUAU_ASSERT(x) ((void)0)
    #define LUAU_UNREACHABLE() ((void)0)
    #define LUAU_NOINLINE
    #define LUAU_FORCEINLINE inline
    #define LUAU_NORETURN
}