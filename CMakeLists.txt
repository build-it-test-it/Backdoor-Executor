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

# Luau paths configuration
# These can be overridden when calling CMake with -DLUAU_ROOT=path, etc.
set(LUAU_ROOT "${CMAKE_SOURCE_DIR}/external/luau" CACHE PATH "Root directory of Luau installation")
set(LUAU_INCLUDE_DIR "${LUAU_ROOT}/VM/include" CACHE PATH "Directory containing Luau headers")
set(LUAU_VM_LIBRARY "${LUAU_ROOT}/build/libLuau.VM.a" CACHE FILEPATH "Path to Luau VM library")
set(LUAU_COMPILER_LIBRARY "${LUAU_ROOT}/build/libLuau.Compiler.a" CACHE FILEPATH "Path to Luau Compiler library (optional)")

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

# Use pre-installed Luau libraries
message(STATUS "Using pre-installed Luau libraries")

# Define paths to Luau files (user can override these)
if(NOT DEFINED LUAU_ROOT)
    set(LUAU_ROOT "${CMAKE_SOURCE_DIR}/external/luau" CACHE PATH "Root directory of Luau installation")
endif()

if(NOT DEFINED LUAU_INCLUDE_DIR)
    set(LUAU_INCLUDE_DIR "${LUAU_ROOT}/VM/include" CACHE PATH "Directory containing Luau headers")
endif()

if(NOT DEFINED LUAU_VM_LIBRARY)
    # Try common library names
    foreach(LIB_PATH 
        "${LUAU_ROOT}/build/libLuau.VM.a"
        "${LUAU_ROOT}/build/Luau.VM.a"
    )
        if(EXISTS "${LIB_PATH}")
            set(LUAU_VM_LIBRARY "${LIB_PATH}" CACHE FILEPATH "Path to Luau VM library")
            break()
        endif()
    endforeach()
endif()

# Set LUA variables for compatibility with rest of the build
set(LUA_INCLUDE_DIR "${LUAU_INCLUDE_DIR}")
set(LUA_LIBRARY "${LUAU_VM_LIBRARY}")
set(LUA_LIBRARIES "${LUAU_VM_LIBRARY}")

# Verify files exist
message(STATUS "Checking Luau files:")
message(STATUS "  Include directory: ${LUA_INCLUDE_DIR}")
message(STATUS "  Library: ${LUA_LIBRARY}")

# Create imported target for Luau VM
add_library(luau_vm STATIC IMPORTED)
set_target_properties(luau_vm PROPERTIES
    IMPORTED_LOCATION "${LUAU_VM_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${LUAU_INCLUDE_DIR}"
)

# Create compatibility alias
add_library(Lua::lua ALIAS luau_vm)

# Create Lua interface library
add_library(lua_bundled INTERFACE)
target_include_directories(lua_bundled INTERFACE ${LUA_INCLUDE_DIR})
target_link_libraries(lua_bundled INTERFACE Lua::lua)

# Add include directories for easier access
include_directories(${LUA_INCLUDE_DIR})

# Log information about Luau configuration
message(STATUS "Luau configuration:")
message(STATUS "  Include Directory: ${LUA_INCLUDE_DIR}")
message(STATUS "  VM Library: ${LUA_LIBRARY}")
if(EXISTS "${LUAU_COMPILER_LIBRARY}")
  message(STATUS "  Compiler Library: ${LUAU_COMPILER_LIBRARY}")
else()
  message(STATUS "  Compiler Library: Not found (optional)")
endif()

# Add Luau compiler definitions
add_definitions(-DUSE_LUAU=1)
add_definitions(-DLUAU_FASTINT_SUPPORT=1)

# Add subdirectories
add_subdirectory(source/cpp)
add_subdirectory(source)

# Verify that required Luau files exist
if(NOT EXISTS "${LUAU_INCLUDE_DIR}/lua.h")
    message(FATAL_ERROR "Luau headers not found at ${LUAU_INCLUDE_DIR}. Please install Luau in the expected location or specify -DLUAU_INCLUDE_DIR=/path/to/headers")
endif()

if(NOT EXISTS "${LUAU_VM_LIBRARY}")
    message(FATAL_ERROR "Luau VM library not found at ${LUAU_VM_LIBRARY}. Please install Luau in the expected location or specify -DLUAU_VM_LIBRARY=/path/to/library")
endif()

# Log file sizes to verify they are valid
file(SIZE "${LUAU_VM_LIBRARY}" LUAU_VM_SIZE)
message(STATUS "Confirmed Luau VM library exists (${LUAU_VM_SIZE} bytes)")

if(EXISTS "${LUAU_COMPILER_LIBRARY}")
    file(SIZE "${LUAU_COMPILER_LIBRARY}" LUAU_COMPILER_SIZE)
    message(STATUS "Found Luau Compiler library (${LUAU_COMPILER_SIZE} bytes)")
endif()

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

# Link with our static library and dependencies
target_link_libraries(mylibrary
    PRIVATE
    roblox_execution
)

# Link directly with Luau libraries
target_link_libraries(mylibrary PRIVATE Lua::lua)

# Additional include paths and flags
target_include_directories(mylibrary PRIVATE 
    ${LUA_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/source
)

# Extra compiler defines for diagnostics
target_compile_definitions(mylibrary PRIVATE
    USE_LUA=1
    USE_LUAU=1
    LUAU_VM_LIBRARY_PATH="${LUAU_VM_LIBRARY}"
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