cmake_minimum_required(VERSION 3.13)

# Project name
project(RobloxExecutor VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable CI build detection
if(DEFINED ENV{CI} OR DEFINED BUILD_CI)
  set(CI_BUILD TRUE)
  add_definitions(-DCI_BUILD)
  message(STATUS "CI Build detected - using conditional compilation")
else()
  set(CI_BUILD FALSE)
  message(STATUS "Normal build detected")
endif()

# Create output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Create iOS compatibility headers
if(CI_BUILD)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ios_compat)
  message(STATUS "Creating iOS compatibility headers for CI build")
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
    ${CMAKE_BINARY_DIR}  # For generated files
    ${CMAKE_BINARY_DIR}/ios_compat  # For iOS compatibility headers
)

# Find dependencies
find_package(Dobby QUIET)
if(NOT Dobby_FOUND)
    message(STATUS "Dobby not found, using stub implementation for CI")
    add_definitions(-DNO_DOBBY)
    
    # Create a stub dobby.h in build directory for CI
    file(WRITE ${CMAKE_BINARY_DIR}/dobby.h 
    "// Stub for Dobby in CI\n#pragma once\n\nextern \"C\" {\nvoid* DobbyHook(void* address, void* replacement, void** original);\nint DobbyDestroy(void* address);\n}\n")
    
    # Add the build directory to include path
    include_directories(${CMAKE_BINARY_DIR})
endif()

# Only include Luau source files that actually exist
file(GLOB LUAU_SOURCES 
    "source/cpp/luau/*.cpp"
)

# Debug output to check which Luau files we're including
message(STATUS "Building Luau from sources: ${LUAU_SOURCES}")

# Create Lua bundled library (only if we have sources)
if(LUAU_SOURCES)
    add_library(lua_bundled STATIC ${LUAU_SOURCES})
    target_include_directories(lua_bundled PUBLIC
        ${CMAKE_SOURCE_DIR}/source/cpp/luau
    )

    # On CI builds, we need to make sure all Lua symbols are available
    # Fix: Make sure each define is separate and properly escaped
    target_compile_definitions(lua_bundled PRIVATE
        "LUA_USE_LONGJMP=1"
        "LUA_API=__attribute__((visibility(\"default\")))"
        "LUAI_FUNC=__attribute__((visibility(\"default\")))"
        "LUAI_DDEC=__attribute__((visibility(\"default\")))"
        "LUAI_DDEF=__attribute__((visibility(\"default\")))"
    )

    # Create a symlink target to ensure the library is available
    add_custom_target(ensure_lua_path ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lua_bundled> ${CMAKE_BINARY_DIR}/lib/liblua.dylib
        DEPENDS lua_bundled
    )
else()
    message(FATAL_ERROR "No Luau source files found! Cannot continue build.")
endif()

# Build lfs.c as a separate object
add_library(lfs_obj OBJECT source/lfs.c)
target_include_directories(lfs_obj PRIVATE
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
)
# Fix: Make sure each define is separate and properly escaped
target_compile_definitions(lfs_obj PRIVATE 
    "LUA_COMPAT_5_1=1"
    "LUA_API=__attribute__((visibility(\"default\")))"
    "LUAI_FUNC=__attribute__((visibility(\"default\")))"
)
set_target_properties(lfs_obj PROPERTIES 
    C_STANDARD 99 
    POSITION_INDEPENDENT_CODE ON
)

# Add subdirectories
add_subdirectory(source/cpp)

# Create the dynamic library
add_library(roblox_executor SHARED
    source/library.cpp
    $<TARGET_OBJECTS:lfs_obj>
)

# Link with libraries - use WHOLE_ARCHIVE to ensure all symbols are included
target_link_libraries(roblox_executor 
    "-Wl,-force_load,$<TARGET_FILE:lua_bundled>"  # Force include all symbols from lua_bundled
    roblox_execution
)

if(Dobby_FOUND)
    target_link_libraries(roblox_executor Dobby::dobby)
endif()

# Add dependencies to ensure correct build order
add_dependencies(roblox_executor lua_bundled ensure_lua_path)

# Set output name
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "roblox_executor"
    PREFIX ""
)

# Debug output
message(STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using bundled Lua library for link time")
message(STATUS "Lua library path: $<TARGET_FILE:lua_bundled>")
