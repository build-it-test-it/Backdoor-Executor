#!/bin/bash
# Fix duplicate WriteFile methods in FileSystem.mm

# Make a backup
cp source/cpp/ios/FileSystem.mm source/cpp/ios/FileSystem.mm.bak

# Find the start lines of the WriteFile methods
FIRST_LINE=$(grep -n "bool FileSystem::WriteFile" source/cpp/ios/FileSystem.mm | head -1 | cut -d: -f1)
SECOND_LINE=$(grep -n "bool FileSystem::WriteFile" source/cpp/ios/FileSystem.mm | tail -1 | cut -d: -f1)

if [ "$FIRST_LINE" != "$SECOND_LINE" ]; then
  echo "Found duplicate WriteFile methods at lines $FIRST_LINE and $SECOND_LINE"
  
  # Find the end of the second method
  END_LINE=$(tail -n +$SECOND_LINE source/cpp/ios/FileSystem.mm | grep -n "
^
    }" | head -1 | cut -d: -f1)
  END_LINE=$((SECOND_LINE + END_LINE - 1))
  
  echo "Second method ends at line $END_LINE"
  
  # Remove the second method
  sed -i "${SECOND_LINE},${END_LINE}d" source/cpp/ios/FileSystem.mm
  
  echo "Removed duplicate method"
fi

# Check if we're missing a closing brace for the namespace
OPEN_BRACES=$(grep -c "{" source/cpp/ios/FileSystem.mm)
CLOSE_BRACES=$(grep -c "}" source/cpp/ios/FileSystem.mm)

echo "FileSystem.mm has $OPEN_BRACES opening braces and $CLOSE_BRACES closing braces"

if [ $OPEN_BRACES -gt $CLOSE_BRACES ]; then
  echo "Adding closing brace at the end"
  echo "}" >> source/cpp/ios/FileSystem.mm
fi
