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

# Find Lua
find_package(Lua REQUIRED)

# Find CoreML if available
find_library(COREML_LIBRARY CoreML)
find_library(FOUNDATION_LIBRARY Foundation)
find_library(UIKIT_LIBRARY UIKit)
find_library(WEBKIT_LIBRARY WebKit)

# Specify the output directory for the library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Collect all source files
file(GLOB_RECURSE LUA_SOURCES source/lfs.c)

file(GLOB_RECURSE CPP_SOURCES 
    source/library.cpp
    source/cpp/*.cpp
)

file(GLOB_RECURSE MM_SOURCES 
    source/cpp/ios/*.mm
    source/cpp/ios/ui/*.mm
    source/cpp/ios/advanced_bypass/*.mm
    source/cpp/ios/ai_features/*.mm
)

# Combine all sources
set(SOURCES
    ${LUA_SOURCES}
    ${CPP_SOURCES}
    ${MM_SOURCES}
)

# Define the library
add_library(roblox_executor SHARED ${SOURCES})

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

# Include directories
target_include_directories(roblox_executor PRIVATE 
    ${LUA_INCLUDE_DIR} 
    source
    source/cpp
    source/cpp/ios
)

# Link against required libraries
target_link_libraries(roblox_executor PRIVATE
    ${LUA_LIBRARIES}
    "-framework Foundation"
    "-framework UIKit"
    "-framework WebKit"
    "-framework CoreGraphics"
)

# Add CoreML if available
if(COREML_LIBRARY)
    target_link_libraries(roblox_executor PRIVATE "-framework CoreML")
    target_compile_definitions(roblox_executor PRIVATE HAS_CORE_ML=1)
else()
    target_compile_definitions(roblox_executor PRIVATE HAS_CORE_ML=0)
endif()

# Copy required resources
add_custom_command(TARGET roblox_executor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:roblox_executor>/Resources/Models"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Resources/Models"
        "$<TARGET_FILE_DIR:roblox_executor>/Resources/Models"
    COMMENT "Copying AI models and resources to output directory"
)

# Install the dylib
install(TARGETS roblox_executor
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Add additional iOS framework dependencies
target_link_libraries(roblox_executor PRIVATE
    "-framework CoreFoundation"
    "-framework JavaScriptCore"
    "-framework Security"
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
