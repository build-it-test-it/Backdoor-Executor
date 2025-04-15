#!/bin/bash
# Find and fix files that include both Lua and Objective-C

# Get a list of all source files
SOURCE_FILES=$(find source -name "*.cpp" -o -name "*.h" -o -name "*.mm")

for file in $SOURCE_FILES; do
  # Check if this file includes both Lua and Objective-C headers
  if grep -q "#include.*lua" "$file" && grep -q "#import.*Foundation\|UIKit\|#include.*ios_compat" "$file"; then
    echo "Fixing file with both Lua and Objective-C includes: $file"
    
    # Create a backup
    cp "$file" "$file.bak"
    
    # First, add include guards at the top if it's a .h file
    if [[ "$file" == *.h ]]; then
      # Check if the file already has include guards
      if ! grep -q "#pragma once\|#ifndef" "$file"; then
        # Add pragma once at the top
        sed -i '1i#pragma once\n' "$file"
      fi
    fi
    
    # Replace Objective-C imports with our safe isolation header
    sed -i 's/#import <Foundation\/Foundation.h>/#include "..\/..\/cpp\/objc_isolation.h"/g' "$file"
    sed -i 's/#import <UIKit\/UIKit.h>/#include "..\/..\/cpp\/objc_isolation.h"/g' "$file"
    sed -i 's/#include "..\/ios_compat.h"/#include "..\/..\/cpp\/objc_isolation.h"/g' "$file"
    
    # If it's a .cpp file, consider renaming to .mm if it has Objective-C code
    if [[ "$file" == *.cpp ]] && grep -q "@interface\|@implementation\|#import" "$file"; then
      echo "  This file contains Objective-C code, renaming to .mm"
      mv "$file" "${file%.cpp}.mm"
    fi
  fi
done
