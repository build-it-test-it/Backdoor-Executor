#!/bin/bash
# Clean, targeted fixes for FileSystem.mm

# Make a backup
cp source/cpp/ios/FileSystem.mm source/cpp/ios/FileSystem.mm.bak_clean

# 1. Fix all CombinePaths references to JoinPaths
sed -i 's/CombinePaths/JoinPaths/g' source/cpp/ios/FileSystem.mm

# 2. Fix all FileSystem:: scope references
sed -i 's/FileSystem::FileInfo/FileInfo/g' source/cpp/ios/FileSystem.mm
sed -i 's/FileSystem::FileType/FileType/g' source/cpp/ios/FileSystem.mm

# 3. Fix FileType enum values
sed -i 's/FileType::Regular/FileType::File/g' source/cpp/ios/FileSystem.mm
sed -i 's/FileType::Symlink/FileType::File/g' source/cpp/ios/FileSystem.mm

# 4. Fix Delete to DeleteFile
sed -i 's/bool FileSystem::Delete(/bool FileSystem::DeleteFile(/g' source/cpp/ios/FileSystem.mm

# 5. Fix Rename to RenameFile
sed -i 's/bool FileSystem::Rename(/bool FileSystem::RenameFile(/g' source/cpp/ios/FileSystem.mm

# 6. Fix GetFileType reference
sed -i 's/if (GetFileType(path) == FileType::Directory)/if (GetFileInfo(path).m_type == FileType::Directory)/g' source/cpp/ios/FileSystem.mm

# 7. Fix the WriteFile function properly - it got messed up by our previous sed
grep -n "bool FileSystem::WriteFile" source/cpp/ios/FileSystem.mm

# Let's use a special approach for the WriteFile function
# First, find the line numbers of the function
START_LINE=$(grep -n "
^
    bool FileSystem::WriteFile" source/cpp/ios/FileSystem.mm | cut -d: -f1)
if [ -z "$START_LINE" ]; then
  echo "Error: Could not find WriteFile function start"
  exit 1
fi

# Find the end of the function (the next occurrence of '}'
END_LINE=$(tail -n +$START_LINE source/cpp/ios/FileSystem.mm | grep -n "
^
    }" | head -1 | cut -d: -f1)
END_LINE=$((START_LINE + END_LINE - 1))

echo "WriteFile function found from line $START_LINE to $END_LINE"

# Create a fixed WriteFile function - just adjust the signature
cat > fixed_write_file.tmp << 'EOL'
    bool FileSystem::WriteFile(const std::string& path, const std::string& content) {
EOL

# Replace the first line of the function
sed -i "${START_LINE}c\\    bool FileSystem::WriteFile(const std::string& path, const std::string& content) {" source/cpp/ios/FileSystem.mm

# 8. Fix CreateFile to WriteFile
sed -i 's/bool FileSystem::CreateFile(/bool FileSystem::WriteFile(/g' source/cpp/ios/FileSystem.mm

# 9. Fix vector<FileSystem::FileInfo> to vector<FileInfo>
sed -i 's/std::vector<FileSystem::FileInfo>/std::vector<FileInfo>/g' source/cpp/ios/FileSystem.mm

# 10. Fix all occurrences of FileInfo constructor with extra params
# This requires a more delicate approach. Let's find them first.
grep -n "return FileInfo" source/cpp/ios/FileSystem.mm

# Let's specifically fix line 235 which has 7 parameters
if grep -q "return FileInfo.*name" source/cpp/ios/FileSystem.mm; then
  LINE=$(grep -n "return FileInfo.*name" source/cpp/ios/FileSystem.mm | cut -d: -f1)
  if [ ! -z "$LINE" ]; then
    # Replace with a 6-parameter version
    sed -i "${LINE}c\\        return FileInfo(safePath, type, size, modTime, isReadable, isWritable);" source/cpp/ios/FileSystem.mm
  fi
fi

echo "FileSystem.mm fixes applied"
