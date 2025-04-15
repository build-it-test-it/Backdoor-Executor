cmake_minimum_required(VERSION 3.13)
project(RobloxExecutor VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable CI build detection
add_definitions(-DCI_BUILD)

# Set output directories - use standard directories that should already exist
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include existing sources
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
)

# Build the dynamic library directly - minimal changes
add_library(roblox_executor SHARED source/library.cpp)

# Set output name
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "libmylibrary"
    PREFIX ""
)

# Copy to the expected output location
add_custom_command(TARGET roblox_executor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:roblox_executor> ${CMAKE_SOURCE_DIR}/output/libmylibrary.dylib
)

message(STATUS "Building iOS Roblox Executor")
