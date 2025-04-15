# FindDobby.cmake for iOS Roblox Executor
# This module finds the Dobby library or builds it if not found
# The following variables will be defined:
#   Dobby_FOUND        - True if Dobby was found
#   DOBBY_INCLUDE_DIR  - The Dobby include directory
#   DOBBY_LIBRARY      - The Dobby library

# Try to find dobby in standard locations
find_path(DOBBY_INCLUDE_DIR
    NAMES dobby.h
    PATHS
        ${CMAKE_SOURCE_DIR}/external/dobby/include
        ${CMAKE_SOURCE_DIR}/external/include
        ${CMAKE_SOURCE_DIR}/dobby/include
        /usr/local/include/dobby
        /usr/local/include
        /usr/include/dobby
        /usr/include
        /opt/homebrew/include/dobby
        /opt/homebrew/include
    DOC "Dobby include directory"
)

find_library(DOBBY_LIBRARY
    NAMES dobby libdobby
    PATHS
        ${CMAKE_SOURCE_DIR}/external/dobby/lib
        ${CMAKE_SOURCE_DIR}/external/lib
        ${CMAKE_SOURCE_DIR}/dobby/lib
        ${CMAKE_SOURCE_DIR}/lib
        /usr/local/lib
        /usr/lib
        /opt/homebrew/lib
    DOC "Dobby library"
)

# If Dobby wasn't found, we'll build it from source (no stubs)
if(NOT DOBBY_INCLUDE_DIR OR NOT DOBBY_LIBRARY)
    message(STATUS "Dobby not found, building from source...")
    
    # Ensure the external directory exists
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/dobby)
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/dobby)
    endif()
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/dobby/include)
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/dobby/include)
    endif()
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/dobby/lib)
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/dobby/lib)
    endif()

    # Clone and build Dobby from the repository
    include(ExternalProject)
    
    set(DOBBY_BUILD_DIR ${CMAKE_BINARY_DIR}/dobby-build)
    
    # Configure the external project
    ExternalProject_Add(
        dobby_external
        GIT_REPOSITORY https://github.com/jmpews/Dobby.git
        GIT_TAG master
        PREFIX ${DOBBY_BUILD_DIR}
        CMAKE_ARGS 
            -DCMAKE_BUILD_TYPE=Release 
            -DDOBBY_BUILD_SHARED_LIBRARY=OFF 
            -DDOBBY_BUILD_STATIC_LIBRARY=ON
            -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/external/dobby
        BUILD_ALWAYS ON
        # Custom command to copy the built library and headers
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
            <SOURCE_DIR>/include
            ${CMAKE_SOURCE_DIR}/external/dobby/include
        COMMAND ${CMAKE_COMMAND} -E copy
            <BINARY_DIR>/libdobby.a
            ${CMAKE_SOURCE_DIR}/external/dobby/lib/libdobby.a
    )
    
    # Set locations after build
    set(DOBBY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/dobby/include)
    set(DOBBY_LIBRARY ${CMAKE_SOURCE_DIR}/external/dobby/lib/libdobby.a)
    
    # Make directory for include files
    file(MAKE_DIRECTORY ${DOBBY_INCLUDE_DIR})
    
    # Set found flag after build
    set(Dobby_FOUND TRUE)
    
    # Create imported target for Dobby
    add_library(dobby_imported STATIC IMPORTED GLOBAL)
    add_dependencies(dobby_imported dobby_external)
    set_target_properties(dobby_imported PROPERTIES
        IMPORTED_LOCATION ${DOBBY_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${DOBBY_INCLUDE_DIR}
    )

    # Create an alias for the imported target
    add_library(Dobby::dobby ALIAS dobby_imported)
    
    message(STATUS "Dobby will be built from source at: ${DOBBY_BUILD_DIR}")
    message(STATUS "Dobby headers will be installed to: ${DOBBY_INCLUDE_DIR}")
    message(STATUS "Dobby library will be installed to: ${DOBBY_LIBRARY}")
else()
    # If Dobby was found, set the found flag
    set(Dobby_FOUND TRUE)
    message(STATUS "Found existing Dobby installation")
    message(STATUS "Dobby include directory: ${DOBBY_INCLUDE_DIR}")
    message(STATUS "Dobby library: ${DOBBY_LIBRARY}")
    
    # Create imported target for existing Dobby
    if(NOT TARGET Dobby::dobby)
        add_library(Dobby::dobby UNKNOWN IMPORTED GLOBAL)
        set_target_properties(Dobby::dobby PROPERTIES
            IMPORTED_LOCATION "${DOBBY_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${DOBBY_INCLUDE_DIR}"
        )
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Dobby DEFAULT_MSG DOBBY_INCLUDE_DIR DOBBY_LIBRARY)

mark_as_advanced(DOBBY_INCLUDE_DIR DOBBY_LIBRARY)
