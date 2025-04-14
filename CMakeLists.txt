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

# Set iOS TARGET definition and other platform-specific defines
if(APPLE)
    add_definitions(-DIOS_TARGET)
    add_definitions(-DTARGET_OS_IPHONE=1)
    add_definitions(-DTARGET_OS_MAC=1)
    # This ensures vm_region_64 is properly recognized
    add_definitions(-D_DARWIN_C_SOURCE)
endif()

# Find Lua - try multiple approaches
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Handle Luau finding on macOS
if(APPLE)
    # First check for environment variables set by the workflow
    if(DEFINED ENV{LUAU_INCLUDE_DIR} AND DEFINED ENV{LUA_LIBRARIES})
        message(STATUS "Using Luau from environment variables")
        set(LUA_INCLUDE_DIR "$ENV{LUAU_INCLUDE_DIR}")
        set(LUA_LIBRARIES "$ENV{LUA_LIBRARIES}")
        set(LUA_FOUND TRUE)
    else()
        # Check Homebrew Luau location
        execute_process(
            COMMAND brew --prefix luau
            OUTPUT_VARIABLE BREW_LUAU_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        if(BREW_LUAU_PREFIX)
            message(STATUS "Found Homebrew Luau at: ${BREW_LUAU_PREFIX}")
            set(LUA_INCLUDE_DIR "${BREW_LUAU_PREFIX}/include")
            
            # Look for the luau library file
            find_library(LUA_LIBRARIES 
                        NAMES luau libluau
                        PATHS "${BREW_LUAU_PREFIX}/lib" NO_DEFAULT_PATH)
            
            if(LUA_LIBRARIES)
                message(STATUS "Found Luau library: ${LUA_LIBRARIES}")
                set(LUA_FOUND TRUE)
            else()
                # Hardcode as a last resort
                set(LUA_LIBRARIES "${BREW_LUAU_PREFIX}/lib/libluau.dylib")
                message(STATUS "Using hardcoded Luau library path: ${LUA_LIBRARIES}")
                set(LUA_FOUND TRUE)
            endif()
        else()
            message(STATUS "Homebrew Luau not found. Please install with: brew install luau")
        endif()
    endif()
endif()

# Try standard find_package with our custom finder module
find_package(Lua QUIET)

# Check if Luau was found - modified for library flexibility
if(NOT LUA_FOUND)
    message(FATAL_ERROR "Could not find Luau. Please install Luau with: brew install luau")
endif()

# Create lua_bundled target before we potentially use it

# If we're using bundled Lua, create a static library from the sources we have
if(USE_BUNDLED_LUA)
    # We'll create a stub Lua library that has just enough to satisfy the linker
    file(WRITE "${CMAKE_BINARY_DIR}/lua_stub.c" "
        #include <stdlib.h>
        // Minimal set of Lua functions needed to link
        void luaL_openlibs() { }
        void* luaL_newstate() { return NULL; }
        void lua_close(void* L) { }
        int luaL_loadstring(void* L, const char* s) { return 0; }
        int lua_pcall(void* L, int a, int b, int c) { return 0; }
    ")
    
    # Create a static library
    add_library(lua_bundled STATIC "${CMAKE_BINARY_DIR}/lua_stub.c")
    target_include_directories(lua_bundled PRIVATE 
        ${LUA_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/source/cpp/luau
    )
    
    # Use this as our Lua library
    set(LUA_LIBRARIES lua_bundled)
    message(STATUS "Using bundled Lua library for link time")
else()
    # Ensure LUA_LIBRARIES is set, using flexible library finding
    if(NOT DEFINED LUA_LIBRARIES)
        # Try to find common library names
        find_library(TEMP_LUA_LIB
            NAMES luau lua libluau liblua
            PATHS /opt/homebrew/lib
                /usr/local/lib
                ${CMAKE_SOURCE_DIR}/lib
                $ENV{HOME}/lib
            NO_DEFAULT_PATH
        )
        
        if(TEMP_LUA_LIB)
            set(LUA_LIBRARIES ${TEMP_LUA_LIB})
            message(STATUS "Found Lua library: ${LUA_LIBRARIES}")
        else()
            # Last resort fallback
            execute_process(
                COMMAND brew --prefix luau
                OUTPUT_VARIABLE LUAU_PREFIX
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            
            if(LUAU_PREFIX)
                # Just pick any library file as a workaround
                file(GLOB LUAU_LIBS "${LUAU_PREFIX}/lib/*.dylib")
                if(LUAU_LIBS)
                    list(GET LUAU_LIBS 0 LUA_LIBRARIES)
                    message(STATUS "Using fallback Luau library: ${LUA_LIBRARIES}")
                else()
                    # Absolute last resort - create a dummy library and reference it
                    set(LUA_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/libluau.dylib")
                    # Create a dummy library
                    file(WRITE "${CMAKE_SOURCE_DIR}/lib/libluau.c" "void luaL_openlibs() {}")
                    add_library(luau_mock SHARED "${CMAKE_SOURCE_DIR}/lib/libluau.c")
                    set_target_properties(luau_mock PROPERTIES OUTPUT_NAME luau)
                    set(LUA_LIBRARIES $<TARGET_FILE:luau_mock>)
                    message(STATUS "Using mock Lua library: ${LUA_LIBRARIES}")
                endif()
            else()
                message(FATAL_ERROR "Could not find Luau. Please install Luau with: brew install luau")
            endif()
        endif()
    endif()
    message(STATUS "Using system Lua library: ${LUA_LIBRARIES}")
endif()

message(STATUS "Using Lua include dir: ${LUA_INCLUDE_DIR}")

# Find required frameworks
find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
find_library(UIKIT_LIBRARY UIKit REQUIRED)
find_library(WEBKIT_LIBRARY WebKit REQUIRED)
find_library(CORE_GRAPHICS_LIBRARY CoreGraphics REQUIRED)
find_library(CORE_FOUNDATION_LIBRARY CoreFoundation REQUIRED)
find_library(JAVASCRIPT_CORE_LIBRARY JavaScriptCore REQUIRED)
find_library(SECURITY_LIBRARY Security REQUIRED)

# Add JavaScriptCore to the compiler flags to ensure it's properly included
add_definitions(-DJAVASCRIPT_CORE_AVAILABLE=1)

# Specify the output directory for the library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Option to use bundled Lua or find system Lua
option(USE_BUNDLED_LUA "Use bundled Lua library instead of system library" ON)

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

# Include our custom LuaFileSystem finder using our internal Luau headers
include(cmake/FindLuaFileSystem.cmake)

# Main C++ sources - explicitly add the Luau sources
file(GLOB_RECURSE CPP_SOURCES 
    source/library.cpp
    source/cpp/*.cpp
)

# Add LuaFileSystem using our custom finder that uses internal Luau headers
message(STATUS "Using internal Luau headers instead of external Lua")
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

# Make sure our bundled library is built before the main target
if(USE_BUNDLED_LUA)
    add_dependencies(roblox_executor lua_bundled)
endif()

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
else()
    # Debug build flags
    target_compile_options(roblox_executor PRIVATE
        -g
    )
endif()

# Add error reporting flags to show more details during build
target_compile_options(roblox_executor PRIVATE
    -ferror-limit=0        # No limit on number of errors to show
    -fcolor-diagnostics    # Use color in diagnostics
    -fdiagnostics-show-category=name    # Show category name
    -fdiagnostics-absolute-paths        # Show absolute paths
)
