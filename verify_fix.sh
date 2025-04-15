#!/bin/bash
# Verify the CMakeLists.txt file is properly fixed

# Check the syntax of the CMakeLists.txt file
echo "Checking CMake syntax..."
cd /tmp
cmake -S /workspace/dylib-Finally_executor- --check-system-vars > /dev/null 2>&1
status=$?

if [ $status -eq 0 ]; then
  echo "✅ CMake syntax is valid!"
else
  echo "❌ CMake syntax is still invalid!"
  echo "Details:"
  cmake -S /workspace/dylib-Finally_executor- --check-system-vars
fi
