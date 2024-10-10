cmake_minimum_required(VERSION 3.10)  # Update the minimum version to avoid deprecation warnings

# Project name
project(MyLuaLibrary)

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 11)

# Find Lua
find_package(Lua REQUIRED)

# Specify the output directory for the library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Add source files
set(SOURCES source/library.cpp)  # Include the path to your C++ source file here

# Create the library
add_library(mylibrary SHARED ${SOURCES})

# Link against the Lua library
target_link_libraries(mylibrary ${LUA_LIBRARIES})

# Include directories for Lua
target_include_directories(mylibrary PRIVATE ${LUA_INCLUDE_DIR})

# Link against LuaFileSystem
target_link_libraries(mylibrary lfs)  # Assuming lfs is the name for LuaFileSystem
