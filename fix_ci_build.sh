#!/bin/bash

# Find all files with #define CI_BUILD
grep -l -r "#define CI_BUILD" --include="*.h" --include="*.hpp" source/ | while read file; do
  echo "Fixing $file..."
  # Use sed to remove the #define CI_BUILD line (macOS and Linux compatible)
  sed -i.bak 's/#define CI_BUILD//g' "$file"
  # Remove backup files
  rm -f "$file.bak"
done
