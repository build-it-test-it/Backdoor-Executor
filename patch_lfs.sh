#!/bin/bash
# Ensure proper Lua header inclusion order in lfs.c

# First check if lfs.c already has lua.h includes
if grep -q "#include.*lua.h" source/lfs.c; then
  # If it does, just add our wrapper before those includes
  sed -i '1i\// Include our compatibility wrapper\n#include "lua_wrapper.h"\n' source/lfs.c
else
  # If not, add both our wrapper and the required Lua headers
  sed -i '1i\// Include Lua headers\n#include "lua_wrapper.h"\n#include "cpp/luau/lua.h"\n#include "cpp/luau/lualib.h"\n' source/lfs.c
fi
