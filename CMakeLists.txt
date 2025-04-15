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
)

# Main source files
set(SOURCES
    source/library.cpp
    source/lfs.c
)

# Create the dynamic library
add_library(roblox_executor SHARED ${SOURCES})

# Set library properties
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "libmylibrary"
    PREFIX ""
)

# Set include directories for the target
target_include_directories(roblox_executor PRIVATE
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/ios
    ${CMAKE_SOURCE_DIR}/source/cpp/memory
)

# Copy the library to output for workflow check
add_custom_command(TARGET roblox_executor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/output
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:roblox_executor> ${CMAKE_SOURCE_DIR}/output/libmylibrary.dylib
    COMMENT "Copying library to output directory"
)

# Create AI data directories
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/output/Resources/AIData)
file(WRITE ${CMAKE_SOURCE_DIR}/output/Resources/AIData/config.json "{\"version\":\"1.0.0\"}")

message(STATUS "Building iOS Roblox Executor")
