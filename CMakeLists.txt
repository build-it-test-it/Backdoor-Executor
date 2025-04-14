cmake_minimum_required(VERSION 3.13)  # Updated for better iOS support

# Project name
project(RobloxExecutor VERSION 1.0.0 LANGUAGES CXX C)

# Specify the required C++ standard (C++17 for better iOS compatibility)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable ObjectiveC and ObjectiveC++ support
enable_language(OBJC)
enable_language(OBJCXX)

# Set iOS target platform and architecture
set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "Minimum iOS deployment version")
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for iOS")

# Find Lua - try multiple approaches
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Handle Lua finding on macOS
if(APPLE)
    # First check for environment variables set by the workflow
    if(DEFINED ENV{LUA_DIR} AND DEFINED ENV{LUA_INCLUDE_DIR} AND DEFINED ENV{LUA_LIBRARIES})
        message(STATUS "Using Lua from environment variables")
        set(LUA_INCLUDE_DIR "$ENV{LUA_INCLUDE_DIR}")
        set(LUA_LIBRARIES "$ENV{LUA_LIBRARIES}")
        set(LUA_FOUND TRUE)
    else()
        # Check Homebrew Lua location as fallback
        execute_process(
            COMMAND brew --prefix lua
            OUTPUT_VARIABLE BREW_LUA_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        if(BREW_LUA_PREFIX)
            message(STATUS "Found Homebrew Lua at: ${BREW_LUA_PREFIX}")
            set(LUA_INCLUDE_DIR "${BREW_LUA_PREFIX}/include")
            
            # Look for the lua library file
            find_library(LUA_LIBRARIES 
                        NAMES lua lua5.4 lua5.3 liblua liblua5.4 liblua5.3
                        PATHS "${BREW_LUA_PREFIX}/lib" NO_DEFAULT_PATH)
            
            if(LUA_LIBRARIES)
                message(STATUS "Found Lua library: ${LUA_LIBRARIES}")
                set(LUA_FOUND TRUE)
            else()
                # Hardcode as a last resort
                set(LUA_LIBRARIES "${BREW_LUA_PREFIX}/lib/liblua.dylib")
                message(STATUS "Using hardcoded Lua library path: ${LUA_LIBRARIES}")
                set(LUA_FOUND TRUE)
            endif()
        else()
            message(STATUS "Homebrew Lua not found via brew --prefix lua")
        endif()
    endif()
endif()

# Try standard find_package
find_package(Lua QUIET)

# If not found, try FindLua51 or FindLua (both come with CMake)
if(NOT LUA_FOUND)
    find_package(Lua51 QUIET)
    if(LUA51_FOUND)
        set(LUA_INCLUDE_DIR ${LUA_INCLUDE_DIR})
        set(LUA_LIBRARIES ${LUA_LIBRARIES})
        set(LUA_FOUND TRUE)
    endif()
endif()

# Last attempt - look for lua manually
if(NOT LUA_FOUND AND NOT DEFINED LUA_LIBRARIES)
    # Try to find the libraries manually
    find_library(LUA_LIBRARIES NAMES lua lua54 lua5.4 lua53 lua5.3 liblua)
    find_path(LUA_INCLUDE_DIR NAMES lua.h 
              PATH_SUFFIXES lua lua5.4 lua5.3 include/lua)
              
    if(LUA_LIBRARIES AND LUA_INCLUDE_DIR)
        message(STATUS "Found Lua manually: ${LUA_LIBRARIES}")
        set(LUA_FOUND TRUE)
    endif()
endif()

# Check if Lua was found
if(NOT LUA_FOUND OR NOT DEFINED LUA_LIBRARIES)
    message(FATAL_ERROR "Could not find Lua. Please install Lua and its development files.")
endif()

message(STATUS "Using Lua include dir: ${LUA_INCLUDE_DIR}")
message(STATUS "Using Lua libraries: ${LUA_LIBRARIES}")

# Find required frameworks
find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
find_library(UIKIT_LIBRARY UIKit REQUIRED)
find_library(WEBKIT_LIBRARY WebKit REQUIRED)
find_library(CORE_GRAPHICS_LIBRARY CoreGraphics REQUIRED)
find_library(CORE_FOUNDATION_LIBRARY CoreFoundation REQUIRED)
find_library(JAVASCRIPT_CORE_LIBRARY JavaScriptCore REQUIRED)
find_library(SECURITY_LIBRARY Security REQUIRED)

# Specify the output directory for the library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Check for Dobby dependency (optional)
option(USE_DOBBY "Use Dobby for function hooking" ON)

if(USE_DOBBY)
    # Check if Dobby_DIR is set (from the workflow)
    if(DEFINED ENV{DOBBY_DIR})
        set(Dobby_DIR $ENV{DOBBY_DIR})
        message(STATUS "Using Dobby from DOBBY_DIR environment variable: ${Dobby_DIR}")
    endif()
    
    # Try to find the Dobby package
    find_package(Dobby QUIET)
    
    # If not found through find_package but DOBBY_DIR is set, set up manually
    if(NOT Dobby_FOUND AND DEFINED Dobby_DIR)
        message(STATUS "Setting up Dobby manually from ${Dobby_DIR}")
        set(Dobby_INCLUDE_DIRS "${Dobby_DIR}/include")
        set(Dobby_LIBRARIES "${Dobby_DIR}/lib/libdobby.a")
        set(Dobby_FOUND TRUE)
    endif()
    
    # Final check
    if(Dobby_FOUND)
        message(STATUS "Dobby library found. Building with hooking functionality.")
        add_definitions(-DHOOKING_AVAILABLE=1)
    else()
        message(STATUS "Dobby library not found. Building without hooking functionality.")
        add_definitions(-DNO_DOBBY_HOOKS)
        add_definitions(-DHOOKING_AVAILABLE=0)
    endif()
else()
    message(STATUS "Dobby support disabled. Building without hooking functionality.")
    add_definitions(-DNO_DOBBY_HOOKS)
    add_definitions(-DHOOKING_AVAILABLE=0)
endif()

# Set AI feature options - now using local training only
option(ENABLE_AI_FEATURES "Enable AI features" ON)
option(ENABLE_LOCAL_TRAINING "Enable local AI model training" ON)

# Include our custom LuaFileSystem finder
include(cmake/FindLuaFileSystem.cmake)

# Do not include lfs.c directly, we'll handle it separately
# Main C++ sources
file(GLOB_RECURSE CPP_SOURCES 
    source/library.cpp
    source/cpp/*.cpp
)

# Add the lfs target
add_lfs_target()

# Objective-C++ sources
file(GLOB_RECURSE MM_SOURCES 
    source/cpp/ios/*.mm
    source/cpp/ios/ui/*.mm
    source/cpp/ios/advanced_bypass/*.mm
)

# AI feature sources
if(ENABLE_AI_FEATURES)
    file(GLOB_RECURSE AI_SOURCES
        source/cpp/ios/ai_features/*.mm
        source/cpp/ios/ai_features/local_models/*.mm
        source/cpp/ios/ai_features/vulnerability_detection/*.mm
    )
endif()

# Remove any duplicate files
list(REMOVE_DUPLICATES MM_SOURCES)
if(ENABLE_AI_FEATURES)
    list(REMOVE_DUPLICATES AI_SOURCES)
endif()

# Combine all sources (excluding lfs.c which is built separately)
set(SOURCES
    ${CPP_SOURCES}
    ${MM_SOURCES}
)

# Add AI sources if enabled
if(ENABLE_AI_FEATURES)
    list(APPEND SOURCES ${AI_SOURCES})
endif()

# Define the library and add lfs.c as an object separately
add_library(roblox_executor SHARED ${SOURCES} $<TARGET_OBJECTS:lfs_obj>)

# Set the output name to match what the workflow expects
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "mylibrary"
    SUFFIX ".dylib"
)

# Set compile definitions
target_compile_definitions(roblox_executor PRIVATE
    BUILDING_DYLIB=1
    EXECUTOR_VERSION="1.0.0"
    IOS_TARGET=1
    _DARWIN_C_SOURCE=1
)

# Add AI-specific definitions
if(ENABLE_AI_FEATURES)
    target_compile_definitions(roblox_executor PRIVATE
        ENABLE_AI_FEATURES=1
        ENABLE_LOCAL_TRAINING=1
    )
else()
    target_compile_definitions(roblox_executor PRIVATE
        ENABLE_AI_FEATURES=0
    )
endif()

# Include directories - ensure Lua headers are available
target_include_directories(roblox_executor PRIVATE 
    ${LUA_INCLUDE_DIR}
    $ENV{LUA_INCLUDE_DIR}  # Also try from environment
    /opt/homebrew/opt/lua/include  # Explicit path for macOS GitHub runner
    /opt/homebrew/include  # Common Homebrew include path
    /usr/local/include  # Standard system include path
    source
    source/cpp
    source/cpp/ios
)

# Add Dobby include if found
if(Dobby_FOUND)
    target_include_directories(roblox_executor PRIVATE ${Dobby_INCLUDE_DIRS})
endif()

# Link against required libraries
target_link_libraries(roblox_executor PRIVATE
    ${LUA_LIBRARIES}
    "-framework Foundation"
    "-framework UIKit"
    "-framework WebKit"
    "-framework CoreGraphics"
    "-framework CoreFoundation"
    "-framework JavaScriptCore"
    "-framework Security"
)

# Add Dobby if found
if(Dobby_FOUND)
    target_link_libraries(roblox_executor PRIVATE ${Dobby_LIBRARIES})
endif()

# Create required directories for AI data
if(ENABLE_AI_FEATURES)
    add_custom_command(TARGET roblox_executor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:roblox_executor>/Resources/AIData"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:roblox_executor>/Resources/AIData/LocalModels"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:roblox_executor>/Resources/AIData/Vulnerabilities"
        COMMENT "Creating AI data directories"
    )
endif()

# Install the dylib
install(TARGETS roblox_executor
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Set build options for iOS
if(CMAKE_BUILD_TYPE MATCHES Release)
    # Optimization flags for release builds
    target_compile_options(roblox_executor PRIVATE
        -Os
        -fvisibility=hidden
        -fvisibility-inlines-hidden
    )
endif()
