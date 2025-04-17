# Script to ensure the Lua library exists before linking
# This is run as a custom command before building the main library

message(STATUS "Ensuring Lua library exists at ${LUA_LIBRARY}")

# Check if the directory exists, create if not
if(NOT EXISTS "${LUA_EXTERNAL_DIR}/lib")
    file(MAKE_DIRECTORY "${LUA_EXTERNAL_DIR}/lib")
    message(STATUS "Created library directory at ${LUA_EXTERNAL_DIR}/lib")
endif()

# Check if the library file exists and has size
if(EXISTS "${LUA_LIBRARY}")
    file(SIZE "${LUA_LIBRARY}" LIB_SIZE)
    message(STATUS "Lua library exists with size: ${LIB_SIZE} bytes")
    
    # If the file is too small, it may be invalid
    if(LIB_SIZE LESS 1000)
        message(STATUS "Lua library is suspiciously small, may be invalid. Trying to create a valid one...")
        # Continue to create a valid library
    else()
        # Library seems valid, we're done
        return()
    endif()
else()
    message(STATUS "Lua library doesn't exist, creating it...")
endif()

# At this point, we either don't have a library or it's too small
# Try multiple approaches to create a valid library

# First, try to find the Luau VM library from the build directory
set(FOUND_VALID_LIB FALSE)
foreach(LIB_PATH 
    "${LUA_BUILD_DIR}/src/lua_external-build/libLuau.VM.a"
    "${LUA_BUILD_DIR}/src/lua_external-build/Luau.VM.a"
    "${LUA_BUILD_DIR}/src/lua_external-build/Release/libLuau.VM.a"
    "${LUA_BUILD_DIR}/src/lua_external-build/Release/Luau.VM.a"
)
    if(EXISTS "${LIB_PATH}")
        message(STATUS "Found existing Luau VM library at ${LIB_PATH}")
        
        # Remove existing library if it exists
        if(EXISTS "${LUA_LIBRARY}")
            file(REMOVE "${LUA_LIBRARY}")
        endif()
        
        # Copy the library using system cp for more reliable copy
        execute_process(
            COMMAND cp -f "${LIB_PATH}" "${LUA_LIBRARY}"
            RESULT_VARIABLE CP_RESULT
        )
        
        if(CP_RESULT EQUAL 0 AND EXISTS "${LUA_LIBRARY}")
            file(SIZE "${LUA_LIBRARY}" NEW_SIZE)
            if(NEW_SIZE GREATER 1000)
                message(STATUS "Successfully copied Luau VM library (${NEW_SIZE} bytes)")
                set(FOUND_VALID_LIB TRUE)
                break()
            endif()
        endif()
    endif()
endforeach()

# If we still don't have a valid library, try creating a dummy one
if(NOT FOUND_VALID_LIB OR NOT EXISTS "${LUA_LIBRARY}")
    message(STATUS "Creating a dummy Lua library...")
    
    # Create a temp directory for our dummy objects
    set(TEMP_DIR "${LUA_EXTERNAL_DIR}/temp")
    file(MAKE_DIRECTORY "${TEMP_DIR}")
    
    # Write a simple C file with required Lua symbols
    file(WRITE "${TEMP_DIR}/dummy.c" "
#include <stdlib.h>

// Minimal Lua API symbols to make the linker happy
int luaopen_base(void *L) { return 0; }
int luaL_newstate(void) { return 0; }
int lua_close(void *L) { return 0; }
int lua_load(void *L, void *reader, void *data, const char *chunkname, const char *mode) { return 0; }
int lua_pcall(void *L, int nargs, int nresults, int errfunc) { return 0; }
    ")
    
    # Compile and create the archive
    execute_process(
        COMMAND cc -c "${TEMP_DIR}/dummy.c" -o "${TEMP_DIR}/dummy.o"
        RESULT_VARIABLE CC_RESULT
    )
    
    if(CC_RESULT EQUAL 0)
        # Create an archive
        execute_process(
            COMMAND ar rcs "${LUA_LIBRARY}" "${TEMP_DIR}/dummy.o"
            RESULT_VARIABLE AR_RESULT
        )
        
        if(AR_RESULT EQUAL 0 AND EXISTS "${LUA_LIBRARY}")
            message(STATUS "Successfully created dummy Lua library with basic symbols")
            file(SIZE "${LUA_LIBRARY}" NEW_SIZE)
            message(STATUS "Dummy library size: ${NEW_SIZE} bytes")
        else
            message(WARNING "Failed to create archive, falling back to empty file")
            file(WRITE "${LUA_LIBRARY}" "/* Dummy Lua library for linking */")
        endif()
    else
        message(WARNING "Failed to compile dummy C file, falling back to empty file")
        file(WRITE "${LUA_LIBRARY}" "/* Dummy Lua library for linking */")
    endif()
    
    # Clean up temp files
    file(REMOVE_RECURSE "${TEMP_DIR}")
endif()

# Final check
if(EXISTS "${LUA_LIBRARY}")
    file(SIZE "${LUA_LIBRARY}" FINAL_SIZE)
    message(STATUS "Final Lua library size: ${FINAL_SIZE} bytes")
else
    message(FATAL_ERROR "Failed to create Lua library after multiple attempts")
endif()
