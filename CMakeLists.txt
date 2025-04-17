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

# Configure Luau paths to use local VM folder
set(LUAU_ROOT "${CMAKE_SOURCE_DIR}/VM" CACHE PATH "Root directory of Luau installation")
set(LUAU_INCLUDE_DIR "${LUAU_ROOT}/include" CACHE PATH "Directory containing Luau headers")

# Build the Luau VM from local source
add_subdirectory(VM)

# Set LUA variables for compatibility with rest of the build
set(LUA_INCLUDE_DIR "${LUAU_INCLUDE_DIR}")

# Since we're compiling directly, there's no physical .a file path to set
# Instead, we'll use the target name
set(LUA_LIBRARY "luau_vm")
set(LUA_LIBRARIES "luau_vm")
set(LUAU_VM_LIBRARY "luau_vm")

message(STATUS "Using local VM folder for Luau implementation")
message(STATUS "  Include directory: ${LUA_INCLUDE_DIR}")
message(STATUS "  Library target: ${LUA_LIBRARY}")

# Create Lua interface library for backward compatibility
add_library(lua_bundled INTERFACE)
target_include_directories(lua_bundled INTERFACE ${LUA_INCLUDE_DIR})
target_link_libraries(lua_bundled INTERFACE luau_vm)

# Add include directories for easier access
include_directories(${LUA_INCLUDE_DIR})

# Add Luau compiler definitions
add_definitions(-DUSE_LUAU=1)
add_definitions(-DLUAU_FASTINT_SUPPORT=1)

# Instead of building VM as a separate library, include its headers directly
# Set paths to VM headers
include_directories(${CMAKE_SOURCE_DIR}/VM/include)

# Collect VM source files for direct compilation
file(GLOB_RECURSE VM_SOURCES
    ${CMAKE_SOURCE_DIR}/VM/src/*.cpp
)

# Add VM source files directly to our build
add_library(luau_vm STATIC ${VM_SOURCES})
target_include_directories(luau_vm PUBLIC 
    ${CMAKE_SOURCE_DIR}/VM/include
    ${CMAKE_SOURCE_DIR}/VM/src
)

# Set compiler flags/definitions for VM sources
if(APPLE)
    target_compile_options(luau_vm PRIVATE 
        -Wall 
        -fPIC
        -fno-exceptions 
        -fno-rtti
    )
    
    if(CMAKE_SYSTEM_NAME MATCHES "iOS")
        target_compile_options(luau_vm PRIVATE
            -fembed-bitcode
            -mios-version-min=13.0
        )
    endif()
endif()

target_compile_definitions(luau_vm PRIVATE 
    LUA_API=LUAI_FUNC
    LUALIB_API=LUAI_FUNC
    LUAU_FASTINT_SUPPORT=1
    USE_LUAU=1
)

# Set properties for the VM library
set_target_properties(luau_vm PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Create compatibility alias for Lua::lua
add_library(Lua::lua ALIAS luau_vm)

# Add remaining subdirectories
add_subdirectory(source/cpp)
add_subdirectory(source)

# For CI builds only - ensure compatibility and enable verbose output
if(DEFINED ENV{CI} OR DEFINED ENV{GITHUB_ACTIONS})
    message(STATUS "CI build detected, using local VM implementation")
    set(CMAKE_VERBOSE_MAKEFILE ON)
    
    # Print key configuration variables for debugging
    message(STATUS "=========== BUILD CONFIGURATION ===========")
    message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
    message(STATUS "LUAU_ROOT: ${LUAU_ROOT}")
    message(STATUS "LUAU_INCLUDE_DIR: ${LUAU_INCLUDE_DIR}")
    message(STATUS "LUA_INCLUDE_DIR: ${LUA_INCLUDE_DIR}")
    message(STATUS "LUA_LIBRARY: ${LUA_LIBRARY}")
    message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
    message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "=========================================")
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

# Link directly with Luau VM
target_link_libraries(mylibrary PRIVATE luau_vm)

# Additional include paths and flags
target_include_directories(mylibrary PRIVATE 
    ${LUA_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/source
)

# Extra compiler defines for diagnostics
target_compile_definitions(mylibrary PRIVATE
    USE_LUA=1
    USE_LUAU=1
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