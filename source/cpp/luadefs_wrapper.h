/**
 * Lua definitions wrapper to handle macro redefinitions gracefully
 */
#pragma once

// Undefine any existing macros that might conflict with Lua headers
#ifdef LUALIB_API
#undef LUALIB_API
#endif

#ifdef LUAI_FUNC
#undef LUAI_FUNC
#endif

// Now include the real Lua headers
#include "../VM/include/lua.h"
#include "../VM/include/luaconf.h"
#include "../VM/include/lualib.h"
#include "../VM/src/lstate.h"
