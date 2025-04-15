#!/bin/bash
# Apply final fixes to make the project build

# 1. First, make sure all Objective-C files are .mm not .cpp
find source -name "*.cpp" | while read file; do
  # Check if this file has Objective-C code or imports
  if grep -q "#import\|@interface\|@implementation" "$file"; then
    echo "Converting $file to .mm (Objective-C++)..."
    mv "$file" "${file%.cpp}.mm"
    
    # If this file is referenced in CMakeLists.txt, update it
    if grep -q "$(basename "$file")" CMakeLists.txt; then
      sed -i "s|$(basename "$file")|$(basename "${file%.cpp}.mm")|g" CMakeLists.txt
    fi
  fi
done

# 2. Ensure ios_compat.h is included properly
echo "Checking for correct inclusion of ios_compat.h..."
find source -name "*.cpp" -o -name "*.h" | while read file; do
  # Check if this file includes the old ios_compat.h
  if grep -q "#include.*ios_compat.h" "$file" && ! grep -q "#include.*objc_isolation.h" "$file"; then
    echo "Fixing includes in $file..."
    sed -i 's|#include ".*ios_compat.h"|#include "../objc_isolation.h"|g' "$file"
    sed -i 's|#include <.*ios_compat.h>|#include "../objc_isolation.h"|g' "$file"
  fi
done

# 3. Check if any files still import Foundation or UIKit directly without __OBJC__ guard
echo "Checking for unguarded Objective-C imports..."
find source -name "*.cpp" -o -name "*.h" | while read file; do
  if grep -q "#import.*Foundation\|#import.*UIKit" "$file" && ! grep -q "#ifdef __OBJC__" "$file"; then
    echo "Adding __OBJC__ guard to $file..."
    sed -i 's|#import <Foundation/Foundation.h>|#ifdef __OBJC__\n#import <Foundation/Foundation.h>\n#endif|g' "$file"
    sed -i 's|#import <UIKit/UIKit.h>|#ifdef __OBJC__\n#import <UIKit/UIKit.h>\n#endif|g' "$file"
  fi
done

echo "Final fixes applied!"
