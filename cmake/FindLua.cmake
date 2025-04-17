# FindLua.cmake for iOS Roblox Executor
# This module finds the Lua/Luau libraries or builds them if not found
# Inspired by the FindDobby.cmake approach

# Variables this module defines:
# LUA_FOUND - True if Lua was found
# LUA_INCLUDE_DIR - Directory containing Lua headers
# LUA_LIBRARIES - Libraries needed to use Lua

# Set up paths
set(LUA_EXTERNAL_DIR "${CMAKE_BINARY_DIR}/external/lua")
set(LUA_INCLUDE_DIR "${LUA_EXTERNAL_DIR}/include")
set(LUA_LIBRARY "${LUA_EXTERNAL_DIR}/lib/liblua.a")
set(LUA_LIBRARIES "${LUA_LIBRARY}")

# Create directories
file(MAKE_DIRECTORY ${LUA_EXTERNAL_DIR})
file(MAKE_DIRECTORY ${LUA_EXTERNAL_DIR}/include)
file(MAKE_DIRECTORY ${LUA_EXTERNAL_DIR}/lib)

# We'll always build our own Lua for consistent behavior
message(STATUS "Building Lua from source for consistent behavior")

# Clone and build Lua from the repository
include(ExternalProject)

set(LUA_BUILD_DIR ${CMAKE_BINARY_DIR}/lua-build)

# Use standard Lua 5.4 or Luau based on configuration
if(USE_LUAU)
    # Configuration for Luau (Roblox's Lua)
    message(STATUS "Configured to build Luau (Roblox's Lua variant)")
    ExternalProject_Add(
        lua_external
        GIT_REPOSITORY https://github.com/Roblox/luau.git
        GIT_TAG master
        PREFIX ${LUA_BUILD_DIR}
        CMAKE_ARGS 
            -DCMAKE_BUILD_TYPE=Release
            -DLUAU_BUILD_TESTS=OFF
        # Build Luau VM and compiler
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release --target Luau.VM Luau.Compiler
        # Copy headers and libraries to our external directory
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
            <SOURCE_DIR>/VM/include
            ${LUA_INCLUDE_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy
            <BINARY_DIR>/Luau.VM${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${LUA_LIBRARY}
    )
else()
    # Configuration for standard Lua 5.4
    message(STATUS "Configured to build standard Lua 5.4")
    ExternalProject_Add(
        lua_external
        URL https://www.lua.org/ftp/lua-5.4.6.tar.gz
        URL_HASH SHA256=7d5ea1b9cb6aa0b59ca3dde1c6adcb57ef83a1ba8e5432c0ecd06bf439b3ad88
        PREFIX ${LUA_BUILD_DIR}
        PATCH_COMMAND ""
        CONFIGURE_COMMAND ""
        # Build Lua as a static library with position-independent code
        BUILD_COMMAND ${CMAKE_COMMAND} -E chdir <SOURCE_DIR> ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} "MYCFLAGS=-fPIC" make generic
        BUILD_IN_SOURCE 1
        # Copy headers and library to our external directory
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
            <SOURCE_DIR>/src/lua.h
            ${LUA_INCLUDE_DIR}/lua.h
        COMMAND ${CMAKE_COMMAND} -E copy
            <SOURCE_DIR>/src/luaconf.h
            ${LUA_INCLUDE_DIR}/luaconf.h
        COMMAND ${CMAKE_COMMAND} -E copy
            <SOURCE_DIR>/src/lualib.h
            ${LUA_INCLUDE_DIR}/lualib.h
        COMMAND ${CMAKE_COMMAND} -E copy
            <SOURCE_DIR>/src/lauxlib.h
            ${LUA_INCLUDE_DIR}/lauxlib.h
        COMMAND ${CMAKE_COMMAND} -E copy
            <SOURCE_DIR>/src/lua.hpp
            ${LUA_INCLUDE_DIR}/lua.hpp
        COMMAND ${CMAKE_COMMAND} -E copy
            <SOURCE_DIR>/src/liblua.a
            ${LUA_LIBRARY}
    )
endif()

# Set found flag after configuring the build
set(LUA_FOUND TRUE)

# Create imported target for Lua
add_library(lua_imported STATIC IMPORTED GLOBAL)
add_dependencies(lua_imported lua_external)
set_target_properties(lua_imported PROPERTIES
    IMPORTED_LOCATION "${LUA_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
)

# Create an alias for the imported target
add_library(Lua::lua ALIAS lua_imported)

# Output paths for debugging
message(STATUS "Lua will be built at: ${LUA_BUILD_DIR}")
message(STATUS "Lua headers will be at: ${LUA_INCLUDE_DIR}")
message(STATUS "Lua library will be at: ${LUA_LIBRARY}")

# Handle the QUIETLY and REQUIRED arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lua DEFAULT_MSG LUA_INCLUDE_DIR LUA_LIBRARIES)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES)
