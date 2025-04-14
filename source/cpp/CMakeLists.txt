# CMakeLists.txt for source/cpp

# Define CI_BUILD for all files
add_definitions(-DCI_BUILD)

# Only include a minimal set of source files for CI build
file(GLOB_RECURSE COMMON_SOURCES
    "memory/*.cpp"
    "luau/*.cpp"
)

# Create the static library
add_library(roblox_execution STATIC ${COMMON_SOURCES})

# Print what we're building
message(STATUS "Building with CI stub implementations")
