#!/bin/bash
# Final targeted fixes for FileSystem.mm

# 1. Remove the second WriteFile definition (line ~270)
SECOND_WRITE_LINE=$(grep -n "
^
    bool FileSystem::WriteFile.*content)" source/cpp/ios/FileSystem.mm | tail -1 | cut -d: -f1)
if [ ! -z "$SECOND_WRITE_LINE" ]; then
  echo "Found second WriteFile definition at line $SECOND_WRITE_LINE"
  
  # Find the closing brace of this function
  END_LINE=$(tail -n +$SECOND_WRITE_LINE source/cpp/ios/FileSystem.mm | grep -n "
^
    }" | head -1 | cut -d: -f1)
  END_LINE=$((SECOND_WRITE_LINE + END_LINE - 1))
  
  echo "Function ends at line $END_LINE"
  
  # Delete the entire function
  if [ ! -z "$END_LINE" ]; then
    sed -i "${SECOND_WRITE_LINE},${END_LINE}d" source/cpp/ios/FileSystem.mm
  fi
fi

# 2. Fix the WriteFile call with 3 arguments
sed -i 's/return WriteFile(safePath, content, false);/return WriteFile(safePath, content);/g' source/cpp/ios/FileSystem.mm

# 3. Fix the append variable reference
grep -n "append" source/cpp/ios/FileSystem.mm
# Let's modify the if statement with append
sed -i 's/if (append && Exists(safePath))/if (Exists(safePath))/g' source/cpp/ios/FileSystem.mm

# 4. Add declarations for missing methods to FileSystem.h
cat >> source/cpp/ios/FileSystem.h << 'EOL'
        
    private:
        // Private helper methods
        static FileType GetFileType(const std::string& path);
        static std::string GetUniqueFilePath(const std::string& basePath);
        static std::string GetSafePath(const std::string& relativePath);
        static bool HasPermission(const std::string& path, bool requireWrite = false);
EOL

echo "FileSystem.mm fixes applied"
