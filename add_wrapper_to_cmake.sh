#!/bin/bash
# Find the CMakeLists.txt file that contains the sources definition
echo "Finding CMakeLists.txt with sources definition..."
CMAKE_FILE=$(grep -l "SOURCES" --include="CMakeLists.txt" -r .)

if [ -n "$CMAKE_FILE" ]; then
  echo "Found CMakeLists.txt file: $CMAKE_FILE"
  
  # Add lua_wrapper.c to the sources
  echo "Adding lua_wrapper.c to sources list..."
  awk '{
    print $0;
    if ($0 ~ /CORE_SOURCES|add_library.*roblox_executor/) {
      print "    source/lua_wrapper.c";
    }
  }' "$CMAKE_FILE" > "$CMAKE_FILE.new"
  
  mv "$CMAKE_FILE.new" "$CMAKE_FILE"
  
  # Add source directory to include paths
  echo "Adding source directory to include paths..."
  awk '{
    print $0;
    if ($0 ~ /include_directories/) {
      print "    ${CMAKE_SOURCE_DIR}/source";
    }
  }' "$CMAKE_FILE" > "$CMAKE_FILE.new"
  
  mv "$CMAKE_FILE.new" "$CMAKE_FILE"
  
  echo "Done!"
else
  echo "ERROR: Could not find CMakeLists.txt with sources definition!"
fi
