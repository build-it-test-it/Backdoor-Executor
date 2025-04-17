# FindLua.cmake for iOS Roblox Executor
# This module finds Lua libraries and includes for use in the project

# Variables this module defines:
# LUA_FOUND - True if Lua was found
# LUA_INCLUDE_DIR - Directory containing Lua headers
# LUA_LIBRARIES - Libraries needed to use Lua

# Simple approach for CI builds - always use our built-in headers
message(STATUS "Using built-in Lua headers in cmake directory")
set(LUA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(LUA_LIBRARIES "lua_built_in")
set(LUA_FOUND TRUE)

# Handle the QUIETLY and REQUIRED arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lua DEFAULT_MSG LUA_LIBRARIES LUA_INCLUDE_DIR)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES)
