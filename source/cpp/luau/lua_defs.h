// Essential definitions for Lua to work with iOS frameworks
#pragma once

// Core API declarations (already defined in lua_defs.h)
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API
#define LUALIB_API extern
#endif

// Define lua function attributes
#ifndef LUA_PRINTF_ATTR
#define LUA_PRINTF_ATTR(fmt, args)
#endif

// Define C++ attribute macros that might conflict
#ifndef LUA_NORETURN
#define LUA_NORETURN
#endif

// Make l_noret not depend on LUA_NORETURN
#undef l_noret
#define l_noret void

// Add defines for missing macros that cause compilation errors
#ifndef LUAI_USER_ALIGNMENT_T
#define LUAI_USER_ALIGNMENT_T double
#endif

#ifndef LUA_EXTRA_SIZE
#define LUA_EXTRA_SIZE 0
#endif

#ifndef LUA_SIZECLASSES
#define LUA_SIZECLASSES 32
#endif

#ifndef LUA_MEMORY_CATEGORIES
#define LUA_MEMORY_CATEGORIES 8
#endif

#ifndef LUA_UTAG_LIMIT
#define LUA_UTAG_LIMIT 16
#endif
