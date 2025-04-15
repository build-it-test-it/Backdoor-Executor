cmake_minimum_required(VERSION 3.13)
project(RobloxExecutor VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# Find required packages
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package(Lua QUIET)
find_package(LuaFileSystem QUIET)

# Define source files by component
# Core files
set(CORE_SOURCES
    source/library.cpp
    source/lfs.c
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

# Set library properties
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "libmylibrary"
    PREFIX ""
)

# Define preprocessor macros
target_compile_definitions(roblox_executor PRIVATE
    ENABLE_AI_FEATURES=1
    ENABLE_LED_EFFECTS=1
    ENABLE_ANTI_DETECTION=1
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
)

# Add LuaFileSystem
if(TARGET lfs_obj)
    target_sources(roblox_executor PRIVATE $<TARGET_OBJECTS:lfs_obj>)
endif()

# Find Dobby for hooking
find_package(Dobby QUIET)
if(Dobby_FOUND)
    target_link_libraries(roblox_executor PRIVATE Dobby::dobby)
    target_compile_definitions(roblox_executor PRIVATE USE_DOBBY=1)
    message(STATUS "Using Dobby for function hooking")
else()
    message(STATUS "Dobby not found, building without hooking functionality")
    target_compile_definitions(roblox_executor PRIVATE NO_DOBBY_HOOKS=1)
endif()

# Link against system frameworks for iOS
if(APPLE)
    find_library(UIKIT UIKit)
    find_library(FOUNDATION Foundation)
    find_library(WEBKIT WebKit)
    find_library(COREGRAPHICS CoreGraphics)
    
    target_link_libraries(roblox_executor PRIVATE
        ${UIKIT}
        ${FOUNDATION}
        ${WEBKIT}
        ${COREGRAPHICS}
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

message(STATUS "Building iOS Roblox Executor with enhanced features")
