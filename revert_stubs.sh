#!/bin/bash

# Remove all the stubs we created earlier
rm -f source/cpp/stubs/empty_stub.cpp

# Restore original files from backups if they exist
for file in source/cpp/ios/UIController.cpp.backup source/cpp/ios/UIController.h.backup; do
  if [ -f "$file" ]; then
    echo "Restoring $file to ${file%.backup}"
    cp "$file" "${file%.backup}"
  fi
done

# Make sure we're using the proper iOS compatibility system
for file in source/cpp/ios/*.cpp source/cpp/ios/*.mm source/cpp/ios/*/*.cpp source/cpp/ios/*/*.mm; do
  if [ -f "$file" ]; then
    # Add the iOS compatibility header at the top if it's not already there
    if ! grep -q "#include \"ios_compat.h\"" "$file"; then
      sed -i '1i#include "../ios_compat.h"' "$file"
    fi
    
    # Remove any direct iOS imports
    sed -i '/#import <UIKit\/UIKit.h>/d' "$file"
    sed -i '/#import <Foundation\/Foundation.h>/d' "$file"
    sed -i '/#import <objc\/runtime.h>/d' "$file"
  fi
done

# Make sure our CMake is properly set up for CI
if ! grep -q "add_definitions(-DCI_BUILD)" CMakeLists.txt; then
  sed -i '/cmake_minimum_required/a\
# Enable CI build detection\
if(DEFINED ENV{CI} OR DEFINED BUILD_CI)\
  set(CI_BUILD TRUE)\
  add_definitions(-DCI_BUILD)\
  message(STATUS "CI Build detected - using conditional compilation")\
else()\
  set(CI_BUILD FALSE)\
  message(STATUS "Normal build detected")\
endif()' CMakeLists.txt
fi
