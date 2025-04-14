cmake_minimum_required(VERSION 3.13)

# Project name
project(RobloxExecutor VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect CI Build
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
    ${CMAKE_BINARY_DIR}  # For generated files
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

# Build Luau from source
file(GLOB_RECURSE LUAU_SOURCES 
    "source/cpp/luau/*.cpp"
)

# Add specific Luau C sources if needed
list(APPEND LUAU_SOURCES
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lapi.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lbaselib.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lbytecode.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lcode.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lcorolib.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/ldebug.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/ldo.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lfunc.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lgc.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/linit.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lmathlib.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lmem.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lobject.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/loslib.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lstate.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lstring.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lstrlib.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/ltable.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/ltablib.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/ltm.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lutf8lib.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lvmexecute.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lvmload.cpp"
    "${CMAKE_SOURCE_DIR}/source/cpp/luau/lvmutils.cpp"
)

# Remove duplicates
list(REMOVE_DUPLICATES LUAU_SOURCES)

# Debug output to check sources
message(STATUS "Building Luau from sources: ${LUAU_SOURCES}")

# Create Lua bundled library
add_library(lua_bundled STATIC ${LUAU_SOURCES})
target_include_directories(lua_bundled PUBLIC
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# On CI builds, we need to make sure all Lua symbols are available
if(CI_BUILD)
    target_compile_definitions(lua_bundled PRIVATE
        LUA_USE_LONGJMP=1
        LUA_API=__attribute__((visibility("default")))
        LUAI_FUNC=
        LUAI_DDEC=
        LUAI_DDEF=
    )
endif()

# Create a symlink target to ensure the library is available
add_custom_target(ensure_lua_path ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lua_bundled> ${CMAKE_BINARY_DIR}/lib/liblua.dylib
    DEPENDS lua_bundled
)

# Build lfs.c as a separate object
add_library(lfs_obj OBJECT source/lfs.c)
target_include_directories(lfs_obj PRIVATE
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
)
target_compile_definitions(lfs_obj PRIVATE 
    LUA_COMPAT_5_1=1
    LUA_API=__attribute__((visibility("default")))
    LUAI_FUNC=__attribute__((visibility("default")))
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

# Link with libraries
target_link_libraries(roblox_executor 
    lua_bundled
    roblox_execution
)

if(Dobby_FOUND)
    target_link_libraries(roblox_executor Dobby::dobby)
endif()

# Add dependencies
add_dependencies(roblox_executor lua_bundled ensure_lua_path)

# Set output name
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "roblox_executor"
    PREFIX ""
)

# Print build information
message(STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using bundled Lua library for link time")
include_directories(/ios_compat)
include_directories(${CMAKE_BINARY_DIR}/ios_compat)
include_directories(build/ios_compat)
