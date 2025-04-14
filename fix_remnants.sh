#!/bin/bash

# Files to check
FILES=$(grep -l "grep\|sed\|\$MAIN_CMAKE" source/cpp/ios/*.h source/cpp/ios/*.cpp source/cpp/ios/ai_features/*.h source/cpp/ios/ai_features/*.cpp 2>/dev/null)

for file in $FILES; do
  echo "Checking $file..."
  
  # Create a temporary file with just the C++ content (skipping shell script parts)
  grep -v "grep\|sed\|\$MAIN_CMAKE\|then\|fi\|echo" "$file" > "$file.clean"
  
  # Add CI_BUILD definition to the file
  sed -i '1i#define CI_BUILD\n' "$file.clean"
  
  # Replace the original file
  mv "$file.clean" "$file"
  
  echo "Fixed $file"
done
