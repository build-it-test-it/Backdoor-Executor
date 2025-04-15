/*
** Configuration header for Luau
*/

#pragma once

#include <stddef.h>

// Predefined: use assert.h for checks in Runtime and VM
// You can override this by defining LUA_CORE and setting LUA_USE_ASSERTION
#if !defined(LUA_CORE) || defined(LUA_USE_ASSERTION)
#include <assert.h>
#define lua_assert(x) assert(x)
#else
#define lua_assert(x) ((void)0)
#endif

// Predefined: function visibility
// You can override this by defining LUA_API/LUAI_FUNC macros directly
#ifndef LUA_API
#define LUA_API extern
#endif

// Only define LUAI_FUNC, LUAI_DDEC, LUAI_DDEF if not already defined
// This allows the build system to provide these definitions
#ifndef LUAI_FUNC
// For CI builds, use default visibility
#if 0
#define LUAI_FUNC extern
#else
// Otherwise use the regular visibility for iOS builds
#define LUAI_FUNC __attribute__((visibility("hidden"))) extern
#endif
#endif

#ifndef LUAI_DDEC
#define LUAI_DDEC extern
#endif

#ifndef LUAI_DDEF
#define LUAI_DDEF
#endif

#define LUAI_DATA extern

// Common configuration: 64-bit systems can do byte-by-byte equality checks without aliasing violations
#define LUA_USE_MEMCMP 1

// Prevent automatic cast userdata types between compatible types
// When disabled, this allows userdata values with the same metatable to be considered compatible for rawequal checks
#define LUA_USERDATA_STRICT_EQ 1

// Compile-time configuration for Luau VM & compiler
#define LUA_BITFIELD_ENCODE_ARRAY_CAPACITY 1 // encode array capacity in the jump's bytecode encoding
#define LUA_CUSTOM_EXECUTION 0               // allow execution to continue after luaD_step returns
#define LUA_ISTRYMETA 8                      // TM_EQ, TM_ADD, TM_SUB, TM_MUL, TM_DIV, TM_MOD, TM_POW, TM_UNM
#define LUA_MASKTYPETESTED (1 << LUA_TNUMBER) | (1 << LUA_TSTRING) | (1 << LUA_TBOOLEAN) | (1 << LUA_TNIL) | (1 << LUA_TTABLE) | (1 << LUA_TUSERDATA) | (1 << LUA_TLIGHTUSERDATA) | \
                           (1 << LUA_TTHREAD) | (1 << LUA_TVECTOR)
#define LUA_MINSTACK 20            // minimum stack size
#define LUAI_MAXCSTACK 8000        // maximum size of C stack
#define LUAI_MAXCALLS 20000        // maximum depth for nested C calls
#define LUAI_MAXCCALLS 200         // maximum depth for nested C calls when calling a function
#define LUAI_MAXVARS 200           // maximum number of local variables per function
#define LUAI_MAXUPVALUES 60        // maximum number of upvalues per function
#define LUAI_MEM_LIMIT 64          // memory limit in MB (used in debugging)
#define LUAI_HARDSTACKLIMIT 256000 // maximum Lua stack size in bytes when performing a GC allocation (used in debugging)
#define LUA_CUSTOM_EXECUTION 0     // redefine this to execute custom bytecode in place of luaV_execute; used in Roblox VM instrumentation
#define LUA_EXCEPTION_HOOK 0       // redefine to 1 to call luau_callhook on C++ exceptions; used in Roblox to catch exceptions in hook tail
#define LUA_RAISE_HOOK 0           // redefine to 1 to call luau_callhook on runtime errors; used in Roblox to capture errors in hook tail
#define LUA_HISTORY_SIZE 1         // length of history chain for os.clock() delta computation
#define LUA_AUTODEBUG_CHECKS 0     // additional consistency checks, slightly expensive
#define LUA_BITSINT 32             // number of bits in an integer
#define LUA_MAXNUM 1e127           // used in luaV_execute range checks
#define LUA_BUILTIN_RNG 0          // use builtin random number generator (not provided in open source)
#define LUA_JIT_DISABLED 1         // disable JIT engine entirely

// Other common constants
#define LUAI_MAXSHORTLEN 40
#define LUA_BUFFERSIZE 512
#define LUA_IDSIZE 60

// Compatibility with C++
#define LUA_COMPAT_DEBUGLIBNAME 1 // compatibility with old debug library name
