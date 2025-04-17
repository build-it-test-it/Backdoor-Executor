# LuaConfig.cmake - Find Lua package configuration
# This file is used by find_package to provide configuration for the Lua package

# Define variables
set(LUA_FOUND TRUE)
set(LUA_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(LUA_LIBRARIES "lua_built_in")

# Report what we found
message(STATUS "Found Lua: ${LUA_INCLUDE_DIR}")
