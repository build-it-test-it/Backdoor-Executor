#!/bin/bash
# Apply final fixes to make the project build

# 1. Make sure any .cpp files that were using Objective-C are renamed to .mm
find source/cpp/ios -name "*.cpp" | while read file; do
  echo "Converting $file to .mm..."
  if [ -f "$file" ]; then
    mv "$file" "${file%.cpp}.mm"
    
    # Update CMake if necessary
    if grep -q "$(basename "$file")" CMakeLists.txt; then
      sed -i "s|$(basename "$file")|$(basename "${file%.cpp}.mm")|g" CMakeLists.txt
    fi
  fi
done

# 2. Add a definition for DOBBY_UNHOOK_DEFINED if needed
if grep -q "DobbyUnHook" external/dobby/include/dobby.h; then
  echo "Adding DOBBY_UNHOOK_DEFINED..."
  echo "#define DOBBY_UNHOOK_DEFINED 1" > source/cpp/dobby_defs.h
  
  # Add include to dobby_wrapper.cpp
  sed -i '1i#include "dobby_defs.h"' source/cpp/dobby_wrapper.cpp
fi

echo "Final fixes applied!"
