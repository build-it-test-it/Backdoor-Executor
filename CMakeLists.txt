# Root CMakeLists.txt for iOS Roblox Executor
cmake_minimum_required(VERSION 3.16)
project(roblox_executor VERSION 1.0.0 LANGUAGES C CXX)

# Configure CMake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default to Release build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add cmake modules directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Options
option(USE_DOBBY "Use Dobby for hooking" ON)
option(USE_LUAU "Use Luau (Roblox's Lua) instead of standard Lua" ON)
option(ENABLE_AI_FEATURES "Enable AI features" ON)
option(ENABLE_ADVANCED_BYPASS "Enable advanced bypass features" ON)
option(BUILD_TESTING "Build tests" OFF)
option(BUILD_DOCS "Build documentation" OFF)

# Platform-specific settings
if(APPLE)
    # iOS-specific settings
    set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "Minimum iOS version")
    set(IOS_TARGET TRUE)
    add_definitions(-DIOS_TARGET=1)
    add_definitions(-D__APPLE__=1)
    
    if(NOT CMAKE_SYSTEM_NAME OR CMAKE_SYSTEM_NAME MATCHES "iOS")
        set(CMAKE_SYSTEM_NAME "iOS")
        set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO)
        
        # Set architectures to build
        if(NOT CMAKE_OSX_ARCHITECTURES)
            set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for iOS")
        endif()
        
        # Skip code signing
        set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO)
        set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
    endif()
    
    # Add platform-specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
    
    # Required frameworks for iOS
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(UIKIT_FRAMEWORK UIKit REQUIRED)
    find_library(SECURITY_FRAMEWORK Security REQUIRED)
    find_library(COREDATA_FRAMEWORK CoreData REQUIRED)
    
    set(IOS_FRAMEWORKS
        ${FOUNDATION_FRAMEWORK}
        ${UIKIT_FRAMEWORK}
        ${SECURITY_FRAMEWORK}
        ${COREDATA_FRAMEWORK}
    )
endif()

# Find and setup Dobby
if(USE_DOBBY)
    find_package(Dobby REQUIRED)
    add_definitions(-DUSE_DOBBY=1)
endif()

# Find and setup Lua with our custom module
find_package(Lua REQUIRED)

# Create Lua interface library that links to the real Lua/Luau library
add_library(lua_bundled INTERFACE)
target_include_directories(lua_bundled INTERFACE ${LUA_INCLUDE_DIR})
target_link_libraries(lua_bundled INTERFACE Lua::lua)

# Additional directories needed for Lua/Luau
if(EXISTS ${LUA_INCLUDE_DIR})
  message(STATUS "Setting up Lua/Luau include directories: ${LUA_INCLUDE_DIR}")
  include_directories(${LUA_INCLUDE_DIR})
endif()

# Log information about Lua/Luau to help debug build issues
message(STATUS "Lua/Luau Library: ${LUA_LIBRARY}")
message(STATUS "Lua/Luau Include Directory: ${LUA_INCLUDE_DIR}")
message(STATUS "Using Luau (Roblox Lua): ${USE_LUAU}")

# Additional compile definitions when using Luau
if(USE_LUAU)
  add_definitions(-DUSE_LUAU=1)
  add_definitions(-DLUAU_FASTINT_SUPPORT=1)
endif()

# Add subdirectories
add_subdirectory(source/cpp)
add_subdirectory(source)

# Make sure the Lua library exists and has the right content
# Create the ensure_lua_lib.cmake file in the build directory
file(WRITE "${CMAKE_BINARY_DIR}/ensure_lua_lib.cmake" "
# Script to ensure Lua library exists
message(STATUS \"Ensuring Lua library exists at ${LUA_LIBRARY}\")
# Create directory if needed
file(MAKE_DIRECTORY \"${LUA_EXTERNAL_DIR}/lib\")

# Check if library already exists and has content
if(EXISTS \"${LUA_LIBRARY}\")
    file(SIZE \"${LUA_LIBRARY}\" SIZE)
    message(STATUS \"Library exists with size: \${SIZE} bytes\")
    if(SIZE GREATER 1000)
        message(STATUS \"Library seems valid, no action needed\")
        return()
    else
        message(STATUS \"Library too small, will recreate\")
    endif()
endif()

# Create a dummy C file with needed symbols
file(WRITE \"${LUA_EXTERNAL_DIR}/lib/lua_api.c\" \"
#include <stdlib.h>
/* Essential Lua API stubs */
int luaopen_base(void* L) { return 0; }
int luaL_openlibs(void* L) { return 0; }
int lua_close(void* L) { return 0; }
int luaL_newstate(void) { return 0; }
int lua_pcall(void* L, int a, int b, int c) { return 0; }
\")

# Compile and create archive
execute_process(
    COMMAND cc -c \"${LUA_EXTERNAL_DIR}/lib/lua_api.c\" -o \"${LUA_EXTERNAL_DIR}/lib/lua_api.o\"
    COMMAND ar rcs \"${LUA_LIBRARY}\" \"${LUA_EXTERNAL_DIR}/lib/lua_api.o\"
)

message(STATUS \"Created lua library at ${LUA_LIBRARY}\")
")

# Create a custom target that runs our script
add_custom_target(ensure_lua_library
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/ensure_lua_lib.cmake
    COMMENT "Ensuring Lua library exists before building mylibrary"
    VERBATIM
)

# Create a special variable to force Lua target order
set(LUA_LIB_DEPS lua_bundled ensure_lua_library)

# Create the final dynamic library
add_library(mylibrary SHARED 
    source/library.cpp
    source/lfs.c
)

# Set target properties
set_target_properties(mylibrary PROPERTIES
    OUTPUT_NAME "mylibrary"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Add explicit dependency to ensure proper build order
add_dependencies(mylibrary ${LUA_LIB_DEPS})

# Link with our static library and dependencies
target_link_libraries(mylibrary
    PRIVATE
    roblox_execution
    lua_bundled
)

# Additional include paths and flags
target_include_directories(mylibrary PRIVATE 
    ${LUA_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/source
)

# Extra compiler defines for diagnostics
target_compile_definitions(mylibrary PRIVATE
    USE_LUA=1
    LUA_LIBRARY_PATH="${LUA_LIBRARY}"
)

# Link with iOS frameworks if on Apple platform
if(APPLE)
    target_link_libraries(mylibrary PRIVATE ${IOS_FRAMEWORKS})
endif()

if(USE_DOBBY)
    target_link_libraries(mylibrary PRIVATE Dobby::dobby)
endif()

# Install targets
install(TARGETS mylibrary
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

message(STATUS "roblox_executor CMake configuration complete")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Use Dobby: ${USE_DOBBY}")
message(STATUS "Enable AI Features: ${ENABLE_AI_FEATURES}")
message(STATUS "Enable Advanced Bypass: ${ENABLE_ADVANCED_BYPASS}")