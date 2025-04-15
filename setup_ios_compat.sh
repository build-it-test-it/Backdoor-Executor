#!/bin/bash

# Script to set up iOS compatibility headers and macros for CI builds

echo "Setting up iOS compatibility for CI build..."

# Create iOS compatibility directory and copy headers
mkdir -p build/ios_compat
cp build/ios_compat.h source/cpp/ios_compat.h
cp -r build/ios_compat/* build/ios_compat/

# Fix include paths in iOS files to use compatibility headers
for file in $(find source/cpp/ios -name "*.cpp" -o -name "*.h" -o -name "*.mm"); do
  if [ -f "$file" ]; then
    # Add ios_compat.h include if not already there
    if ! grep -q "#include \".*ios_compat.h\"" "$file"; then
      sed -i '1i#include "../ios_compat.h"' "$file"
    fi
    
    # Remove direct iOS imports
    sed -i '/#import <UIKit\/UIKit.h>/d' "$file"
    sed -i '/#import <Foundation\/Foundation.h>/d' "$file"
    sed -i '/#import <objc\/runtime.h>/d' "$file"
    
    # Add CI_BUILD define if not already there
    if ! grep -q "#define CI_BUILD" "$file"; then
      sed -i '1i#define CI_BUILD' "$file"
    fi
  fi
done

# Apply our fixes
cp CMakeLists.txt.ios_fix CMakeLists.txt
cp source/cpp/CMakeLists.txt.ios_fix source/cpp/CMakeLists.txt

echo "iOS compatibility setup complete"
