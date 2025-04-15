cmake_minimum_required(VERSION 3.13)
project(RobloxExecutor VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set iOS specific flags
if(CMAKE_SYSTEM_NAME STREQUAL "iOS" OR APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "iOS deployment target" FORCE)
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "15.0" CACHE STRING "iOS deployment target" FORCE)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for iOS" FORCE)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Create directories for dependencies
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/dobby)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/dobby/include)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/dobby/lib)

# Remove CI_BUILD defines from source files
execute_process(
    COMMAND find ${CMAKE_SOURCE_DIR}/source -type f -name "*.h" -o -name "*.hpp" -o -name "*.cpp" -o -name "*.mm" | xargs sed -i -e "s/#define CI_BUILD//g"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Set CMake module path
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Download and Build Dobby (Required) if not already present
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/dobby/lib/libdobby.a)
    message(STATUS "Downloading and building Dobby (required dependency)...")
    
    # Clone Dobby repository
    execute_process(
        COMMAND git clone --depth=1 https://github.com/jmpews/Dobby.git ${CMAKE_BINARY_DIR}/dobby-src
        RESULT_VARIABLE DOBBY_CLONE_RESULT
    )
    
    if(NOT DOBBY_CLONE_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to clone Dobby repository. This dependency is required.")
    endif()
    
    # Build Dobby
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dobby-src/build)
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -DDOBBY_BUILD_SHARED_LIBRARY=OFF -DDOBBY_BUILD_STATIC_LIBRARY=ON ..
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/dobby-src/build
        RESULT_VARIABLE DOBBY_CONFIGURE_RESULT
    )
    
    if(NOT DOBBY_CONFIGURE_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to configure Dobby. This dependency is required.")
    endif()
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --config Release
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/dobby-src/build
        RESULT_VARIABLE DOBBY_BUILD_RESULT
    )
    
    if(NOT DOBBY_BUILD_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to build Dobby. This dependency is required.")
    endif()
    
    # Copy Dobby headers and library
    file(COPY ${CMAKE_BINARY_DIR}/dobby-src/include/ DESTINATION ${CMAKE_SOURCE_DIR}/external/dobby/include)
    file(GLOB DOBBY_LIBS ${CMAKE_BINARY_DIR}/dobby-src/build/libdobby.a)
    
    foreach(LIB_FILE ${DOBBY_LIBS})
        file(COPY ${LIB_FILE} DESTINATION ${CMAKE_SOURCE_DIR}/external/dobby/lib)
    endforeach()
    
    message(STATUS "Dobby successfully built and installed to external/dobby")
else()
    message(STATUS "Using existing Dobby installation in external/dobby")
endif()

# Set Dobby paths explicitly
set(DOBBY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/dobby/include)
set(DOBBY_LIBRARY ${CMAKE_SOURCE_DIR}/external/dobby/lib/libdobby.a)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
    ${DOBBY_INCLUDE_DIR}
)

# Find required packages
find_package(Lua QUIET)
find_package(LuaFileSystem QUIET)

# Add LuaFileSystem if not found
if(NOT TARGET lfs_obj AND EXISTS ${CMAKE_SOURCE_DIR}/source/lfs.c)
    message(STATUS "Using bundled LuaFileSystem implementation")
    add_library(lfs_obj OBJECT ${CMAKE_SOURCE_DIR}/source/lfs.c)
    target_include_directories(lfs_obj PRIVATE
        ${CMAKE_SOURCE_DIR}/source/cpp/luau
        ${CMAKE_SOURCE_DIR}/source
    )
    target_compile_definitions(lfs_obj PRIVATE LUA_COMPAT_5_1=1)
endif()

# Dobby wrapper implementation
set(CMAKE_DOBBY_WRAPPER ${CMAKE_SOURCE_DIR}/source/cpp/dobby_wrapper.cpp)
if(NOT EXISTS ${CMAKE_DOBBY_WRAPPER})
    file(WRITE ${CMAKE_DOBBY_WRAPPER} [[
#include "../hooks/hooks.hpp"
#include <iostream>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "dobby.h"

// Tracking for hooked functions
namespace {
    std::mutex g_hookMutex;
    std::unordered_map<void*, void*> g_hookedFunctions;
}

namespace Hooks {
    // Implementation of HookEngine using Dobby
    bool HookEngine::Initialize() {
        std::cout << "Initializing Dobby hook engine..." << std::endl;
        return true;
    }
    
    bool HookEngine::RegisterHook(void* targetAddr, void* hookAddr, void** originalAddr) {
        std::lock_guard<std::mutex> lock(g_hookMutex);
        
        // Check if already hooked
        if (g_hookedFunctions.find(targetAddr) != g_hookedFunctions.end()) {
            std::cout << "Function at " << targetAddr << " is already hooked" << std::endl;
            if (originalAddr) {
                *originalAddr = g_hookedFunctions[targetAddr];
            }
            return true;
        }
        
        // Apply the hook using Dobby
        int result = DobbyHook(targetAddr, hookAddr, originalAddr);
        if (result == 0) {
            // Successful hook
            std::cout << "Successfully hooked function at " << targetAddr << std::endl;
            
            // Store the original function pointer for reference
            if (originalAddr) {
                g_hookedFunctions[targetAddr] = *originalAddr;
            }
            return true;
        } else {
            std::cerr << "Failed to hook function at " << targetAddr << ", error code: " << result << std::endl;
            return false;
        }
    }
    
    bool HookEngine::UnregisterHook(void* targetAddr) {
        std::lock_guard<std::mutex> lock(g_hookMutex);
        
        // Check if the function is hooked
        if (g_hookedFunctions.find(targetAddr) == g_hookedFunctions.end()) {
            std::cout << "Function at " << targetAddr << " is not hooked" << std::endl;
            return false;
        }
        
        // Unhook using Dobby
        int result = DobbyUnHook(targetAddr);
        if (result == 0) {
            // Successful unhook
            std::cout << "Successfully unhooked function at " << targetAddr << std::endl;
            g_hookedFunctions.erase(targetAddr);
            return true;
        } else {
            std::cerr << "Failed to unhook function at " << targetAddr << ", error code: " << result << std::endl;
            return false;
        }
    }
    
    void HookEngine::ClearAllHooks() {
        std::lock_guard<std::mutex> lock(g_hookMutex);
        
        std::cout << "Clearing all hooks..." << std::endl;
        
        // Unhook all functions
        for (const auto& pair : g_hookedFunctions) {
            DobbyUnHook(pair.first);
        }
        
        // Clear the map
        g_hookedFunctions.clear();
        
        std::cout << "All hooks cleared" << std::endl;
    }

    namespace Implementation {
        // Direct implementation for hooks
        bool HookFunction(void* target, void* replacement, void** original) {
            return HookEngine::RegisterHook(target, replacement, original);
        }
        
        bool UnhookFunction(void* target) {
            return HookEngine::UnregisterHook(target);
        }
    }
}
]])
endif()

# Define source files by component
# Core files
set(CORE_SOURCES
    source/library.cpp
    source/lfs.c
    ${CMAKE_DOBBY_WRAPPER}
)

# Memory management
file(GLOB MEMORY_SOURCES
    source/cpp/memory/*.hpp
    source/cpp/memory/*.cpp
)

# Hooks
file(GLOB HOOKS_SOURCES
    source/cpp/hooks/*.hpp
    source/cpp/hooks/*.cpp
)

# UI Files
file(GLOB UI_SOURCES
    source/cpp/ios/ui/*.h
    source/cpp/ios/ui/*.cpp
)

# iOS implementation files
file(GLOB IOS_SOURCES
    source/cpp/ios/*.h
    source/cpp/ios/*.mm
    source/cpp/ios/*.cpp
)

# AI features
file(GLOB AI_SOURCES
    source/cpp/ios/ai_features/*.h
    source/cpp/ios/ai_features/*.mm
    source/cpp/ios/ai_features/*.cpp
    source/cpp/ios/ai_features/local_models/*.h
    source/cpp/ios/ai_features/local_models/*.mm
    source/cpp/ios/ai_features/local_models/*.cpp
    source/cpp/ios/ai_features/vulnerability_detection/*.h
    source/cpp/ios/ai_features/vulnerability_detection/*.mm
    source/cpp/ios/ai_features/vulnerability_detection/*.cpp
)

# Advanced bypass
file(GLOB BYPASS_SOURCES
    source/cpp/ios/advanced_bypass/*.h
    source/cpp/ios/advanced_bypass/*.mm
    source/cpp/ios/advanced_bypass/*.cpp
)

# Anti-detection
file(GLOB ANTI_DETECTION_SOURCES
    source/cpp/anti_detection/*.hpp
    source/cpp/anti_detection/*.cpp
)

# Execution
file(GLOB EXEC_SOURCES
    source/cpp/exec/*.hpp
    source/cpp/exec/*.cpp
)

# Combine all sources
set(SOURCES
    ${CORE_SOURCES}
    ${MEMORY_SOURCES}
    ${HOOKS_SOURCES}
    ${UI_SOURCES}
    ${IOS_SOURCES}
    ${AI_SOURCES}
    ${BYPASS_SOURCES}
    ${ANTI_DETECTION_SOURCES}
    ${EXEC_SOURCES}
)

# Create the dynamic library
add_library(roblox_executor SHARED ${SOURCES})

# Set library properties for iOS
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "mylibrary"
    PREFIX "lib"
    SUFFIX ".dylib"
)

# Define preprocessor macros - always enable features, no stubs
target_compile_definitions(roblox_executor PRIVATE
    ENABLE_AI_FEATURES=1
    ENABLE_LED_EFFECTS=1
    ENABLE_ANTI_DETECTION=1
    USE_DOBBY=1  # Always use Dobby now
    $<$<PLATFORM_ID:iOS>:IOS_TARGET>
    $<$<BOOL:${APPLE}>:__APPLE__>
)

# Set include directories for the target
target_include_directories(roblox_executor PRIVATE
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/ios
    ${CMAKE_SOURCE_DIR}/source/cpp/memory
    ${CMAKE_SOURCE_DIR}/source/cpp/hooks
    ${CMAKE_SOURCE_DIR}/source/cpp/exec
    ${CMAKE_SOURCE_DIR}/source/cpp/ios/ui
    ${CMAKE_SOURCE_DIR}/source/cpp/ios/ai_features
    ${CMAKE_SOURCE_DIR}/source/cpp/ios/advanced_bypass
    ${CMAKE_SOURCE_DIR}/source/cpp/anti_detection
    ${DOBBY_INCLUDE_DIR}
)

# Add LuaFileSystem
if(TARGET lfs_obj)
    target_sources(roblox_executor PRIVATE $<TARGET_OBJECTS:lfs_obj>)
endif()

# Link against Dobby - now required
target_link_libraries(roblox_executor PRIVATE ${DOBBY_LIBRARY})
message(STATUS "Using Dobby for function hooking (required): ${DOBBY_LIBRARY}")

# Link against system frameworks for iOS
if(APPLE)
    find_library(UIKIT UIKit REQUIRED)
    find_library(FOUNDATION Foundation REQUIRED)
    find_library(WEBKIT WebKit REQUIRED)
    find_library(COREGRAPHICS CoreGraphics REQUIRED)
    find_library(SECURITY Security)
    find_library(SYSTEMCONFIGURATION SystemConfiguration)
    
    target_link_libraries(roblox_executor PRIVATE
        ${UIKIT}
        ${FOUNDATION}
        ${WEBKIT}
        ${COREGRAPHICS}
        ${SECURITY}
        ${SYSTEMCONFIGURATION}
    )
endif()

# Copy the library to output for workflow check
add_custom_command(TARGET roblox_executor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/output
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:roblox_executor> ${CMAKE_SOURCE_DIR}/output/libmylibrary.dylib
    COMMENT "Copying library to output directory"
)

# Create AI data directories and ensure Resources structure is complete
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/output/Resources/AIData)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/output/Resources/AIData/LocalModels)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/output/Resources/AIData/Vulnerabilities)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/output/Resources/Models)

# Create base config if it doesn't exist
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/output/Resources/AIData/config.json)
    file(WRITE ${CMAKE_SOURCE_DIR}/output/Resources/AIData/config.json "{\"version\":\"1.0.0\",\"led_effects\":true,\"ai_features\":true,\"memory_optimization\":true}")
endif()

# Copy Resources to output directory
if(EXISTS ${CMAKE_SOURCE_DIR}/Resources)
    add_custom_command(TARGET roblox_executor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_SOURCE_DIR}/output/Resources
        COMMENT "Copying Resources to output directory"
    )
endif()

message(STATUS "Building iOS Roblox Executor with real implementations (no stubs)")
