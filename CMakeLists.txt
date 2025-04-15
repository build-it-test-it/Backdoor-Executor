cmake_minimum_required(VERSION 3.13)
project(RobloxExecutor VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)  # Use C++11 for compatibility
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define CI_BUILD
add_definitions(-DCI_BUILD)
message(STATUS "Using minimal stub build for CI")

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Create stub implementation file
file(WRITE ${CMAKE_BINARY_DIR}/stubs.cpp
"#include <iostream>
#include <string>

// Simple stub implementation for CI build
extern \"C\" {
    // Stub implementation of library entry point
    int luaopen_mylibrary(void* L) {
        std::cout << \"Stub library loaded\" << std::endl;
        return 1;
    }
    
    // Other stubs as needed
    void stub_function() {
        std::cout << \"Stub function called\" << std::endl;
    }
}
")

# Build the stub library
add_library(roblox_executor SHARED ${CMAKE_BINARY_DIR}/stubs.cpp)

# Set output name
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "roblox_executor"
    PREFIX ""
)

message(STATUS "Using minimal stub build for CI - all features disabled")
