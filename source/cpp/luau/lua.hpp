// Compatibility lua.hpp for Luau
// This file provides compatibility with standard Lua's lua.hpp for C++ code
// It includes the necessary Luau headers with C++ compatibility

#pragma once

// Use extern "C" for C++ compatibility
#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "luaconf.h"

#ifdef __cplusplus
}
#endif
