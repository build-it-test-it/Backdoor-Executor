#!/bin/bash
# Script to fix all paths that include "../objc_isolation.h"

# Process all files under source/cpp/ios
find source/cpp/ios -type f \( -name "*.h" -o -name "*.mm" -o -name "*.cpp" -o -name "*.m" \) -print0 | xargs -0 grep -l "../objc_isolation.h" | while read file; do
  # Get the relative depth to determine how many "../" we need
  dir=$(dirname "$file")
  depth=$(echo "$dir" | tr '/' '\n' | wc -l)
  
  # Calculate the correct number of "../" based on depth
  # source/cpp/ios is depth 3, so we need 1 "../" from there
  # source/cpp/ios/ui is depth 4, so we need 2 "../" from there
  # etc.
  
  # Depth 3 (source/cpp/ios) => "objc_isolation.h" (same directory)
  # Depth 4 (source/cpp/ios/something) => "../objc_isolation.h" (parent directory)
  # Depth 5 (source/cpp/ios/something/deeper) => "../../objc_isolation.h" (grandparent directory)
  
  rel_path=""
  if [ "$depth" -eq 3 ]; then
    # If we're in source/cpp/ios, we need to include from same directory
    rel_path="objc_isolation.h"
  elif [ "$depth" -gt 3 ]; then
    # Calculate the correct number of "../" based on depth relative to source/cpp/ios
    up_levels=$(($depth - 3))
    for ((i=0; i<$up_levels; i++)); do
      rel_path="../$rel_path"
    done
    rel_path="${rel_path}objc_isolation.h"
  fi
  
  if [ -n "$rel_path" ]; then
    echo "Fixing $file => #include \"$rel_path\""
    sed -i "s|#include \"../objc_isolation.h\"|#include \"$rel_path\"|g" "$file"
  fi
done
