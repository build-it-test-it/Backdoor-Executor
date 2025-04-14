#!/bin/bash

# 1. Add direct linking to lua_bundled in the main CMakeLists.txt
# Replace just the target_link_libraries section
sed -i 's/target_link_libraries(roblox_executor roblox_execution)/target_link_libraries(roblox_executor roblox_execution lua_bundled)/' CMakeLists.txt

# 2. Make sure the LUAU_SOURCES variable is properly populated
# Let's modify the CMakeLists.txt to explicitly list important Luau source files
sed -i '/file(GLOB LUAU_SOURCES/c\file(GLOB LUAU_SOURCES \n    "source/cpp/luau/*.cpp"\n)' CMakeLists.txt

# Add some verbose output about which Lua sources are being included
sed -i '/add_library(lua_bundled STATIC/i\# List Lua sources for debugging\nmessage(STATUS "LUAU_SOURCES: ${LUAU_SOURCES}")' CMakeLists.txt

# 3. Make sure the proper include paths are set for Lua
sed -i '/include_directories(/a\    ${CMAKE_SOURCE_DIR}/source/cpp/luau' CMakeLists.txt

# 4. Add PUBLIC keyword to Lua target_include_directories
sed -i 's/target_include_directories(lua_bundled/target_include_directories(lua_bundled PUBLIC/' CMakeLists.txt
