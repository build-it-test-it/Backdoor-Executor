# LuaConfig.cmake - Find Lua package configuration
# This file is used by find_package to provide configuration for the Lua package

# Forward to the improved FindLua.cmake which handles actual library discovery/build
include(${CMAKE_CURRENT_LIST_DIR}/FindLua.cmake)

# Report what we found
message(STATUS "LuaConfig using Lua from: ${LUA_INCLUDE_DIR}")
message(STATUS "LuaConfig using Lua libraries: ${LUA_LIBRARIES}")
