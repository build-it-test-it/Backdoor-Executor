# Define CI_BUILD for all compiler instances
add_definitions(-DCI_BUILD)

cmake_minimum_required(VERSION 3.10)
project(RobloxExecutor VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include build directory for our stub headers
include_directories(${CMAKE_BINARY_DIR})

# Add subdirectories
add_subdirectory(source/cpp)

# Create the dynamic library with minimal sources
add_library(roblox_executor SHARED
    source/library.cpp
    source/lfs.c
)

# Link with roblox_execution library
target_link_libraries(roblox_executor roblox_execution)

# Set output name
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "roblox_executor"
    PREFIX ""
)
