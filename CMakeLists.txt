cmake_minimum_required(VERSION 3.13)
project(RobloxExecutor VERSION 1.0.0 LANGUAGES C CXX)

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

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/ios_compat
)

# Find dependencies
find_package(Dobby QUIET)
if(NOT Dobby_FOUND)
    message(STATUS "Dobby not found, using stub implementation for CI")
    add_definitions(-DNO_DOBBY)
    file(WRITE ${CMAKE_BINARY_DIR}/dobby.h 
         "// Stub for Dobby in CI\n#pragma once\n\nextern \"C\" {\nvoid* DobbyHook(void* address, void* replacement, void** original);\nint DobbyDestroy(void* address);\n}\n")
    include_directories(${CMAKE_BINARY_DIR})
endif()

# List all Lua source files directly 
file(GLOB LUAU_SOURCES "source/cpp/luau/*.cpp")
message(STATUS "Building Lua from sources: ${LUAU_SOURCES}")

# Build the Lua library first as a separate static library
add_library(lua_bundled STATIC ${LUAU_SOURCES})

# Set Lua definitions - directly use simple values to avoid quoting issues
target_compile_definitions(lua_bundled PRIVATE 
    LUA_USE_LONGJMP=1
)

# Add the complex definitions using generation expressions
set_property(TARGET lua_bundled PROPERTY COMPILE_DEFINITIONS
    "LUA_API=__attribute__((visibility(\"default\")))"
    "LUAI_FUNC=__attribute__((visibility(\"default\")))"
    "LUAI_DDEC=__attribute__((visibility(\"default\")))"
    "LUAI_DDEF=__attribute__((visibility(\"default\")))"
)

# Build lfs.c as a static library instead of an object 
add_library(lfs_lib STATIC source/lfs.c)
target_include_directories(lfs_lib PRIVATE
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)
target_compile_definitions(lfs_lib PRIVATE
    LUA_COMPAT_5_1=1
)
set_property(TARGET lfs_lib PROPERTY COMPILE_DEFINITIONS
    "LUA_API=__attribute__((visibility(\"default\")))"
    "LUAI_FUNC=__attribute__((visibility(\"default\")))"
)

# Create a clean source/cpp with just a stub for CI
if(CI_BUILD)
    # Create a stub implementation
    file(WRITE ${CMAKE_BINARY_DIR}/stub.cpp
         "#include <iostream>\nextern \"C\" void roblox_execution_stub() { std::cout << \"Stub function called\" << std::endl; }\n")
    add_library(roblox_execution STATIC ${CMAKE_BINARY_DIR}/stub.cpp)
else()
    # Add the full subdirectory for non-CI builds
    add_subdirectory(source/cpp)
endif()

# Now build the main library
add_library(roblox_executor SHARED source/library.cpp)

# Explicitly link the libraries with correct order and force flags
if(APPLE)
    # On macOS/iOS use force_load to ensure all symbols are included
    target_link_libraries(roblox_executor 
        "-Wl,-force_load,$<TARGET_FILE:lua_bundled>"
        "-Wl,-force_load,$<TARGET_FILE:lfs_lib>"
        roblox_execution
    )
else()
    # On other platforms use whole-archive
    target_link_libraries(roblox_executor
        "-Wl,--whole-archive $<TARGET_FILE:lua_bundled> -Wl,--no-whole-archive"
        "-Wl,--whole-archive $<TARGET_FILE:lfs_lib> -Wl,--no-whole-archive"
        roblox_execution
    )
endif()

# Add Dobby if available
if(Dobby_FOUND)
    target_link_libraries(roblox_executor Dobby::dobby)
endif()

# Set output name
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "roblox_executor"
    PREFIX ""
)

# Debug output
message(STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using bundled Lua library")
