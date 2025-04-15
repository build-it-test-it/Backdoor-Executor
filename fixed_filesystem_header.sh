#!/bin/bash
# Fix duplicate private sections in FileSystem.h

# Make a backup
cp source/cpp/ios/FileSystem.h source/cpp/ios/FileSystem.h.bak

# Create a new version with only one private section
awk '
  BEGIN { in_private = 0; seen_private = 0; }
  /
^
[[:space:]]*private:/ {
    if (seen_private == 0) {
      print;
      seen_private = 1;
      in_private = 1;
    } else {
      # Skip this duplicate private section
      next;
    }
  }
  /
^
[[:space:]]*};/ {
    if (in_private) {
      print;
      in_private = 0;
      next;
    } else {
      print;
    }
  }
  !/
^
[[:space:]]*private:/ {
    if (in_private && seen_private) {
      print;
    } else {
      print;
    }
  }
' source/cpp/ios/FileSystem.h > source/cpp/ios/FileSystem.h.new

# Replace the original with our fixed version
mv source/cpp/ios/FileSystem.h.new source/cpp/ios/FileSystem.h
