#!/bin/bash
# Script to remove standalone CI_BUILD definitions from header files
# This ensures CI_BUILD is only defined in CMakeLists.txt

# Find all .h, .hpp files that define CI_BUILD without checking
find source -name "*.h" -o -name "*.hpp" | xargs grep -l "
^
#define CI_BUILD" | while read file; do
  echo "Removing standalone CI_BUILD from $file"
  # Use sed to remove the #define CI_BUILD line that appears at the beginning of a line
  sed -i 's/
^
#define CI_BUILD//g' "$file"
done
