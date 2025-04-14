# CMakeLists.txt for source/cpp

# Define CI_BUILD for all files
add_definitions(-DCI_BUILD)

# Include Lua headers
include_directories(
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# We need at least one source file - use our stub if memory dir is empty
set(STUB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/stubs/empty_stub.cpp"
)

# Check for memory/*.cpp files
file(GLOB MEMORY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/*.c"
)

# Use glob for all valid sources, but make sure we have at least the stub
if(MEMORY_SOURCES)
    set(COMMON_SOURCES ${MEMORY_SOURCES})
    message(STATUS "Found memory sources: ${MEMORY_SOURCES}")
else()
    set(COMMON_SOURCES ${STUB_SOURCES})
    message(STATUS "No memory sources found, using stub: ${STUB_SOURCES}")
endif()

# Create the static library
add_library(roblox_execution STATIC ${COMMON_SOURCES})

# Link with Lua
target_link_libraries(roblox_execution
    lua_bundled
)

# Print what we're building
message(STATUS "Building with CI stub implementations")
