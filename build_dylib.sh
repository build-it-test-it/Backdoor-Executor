#!/bin/bash
# Build script for Roblox Executor dylib with real implementations

# Ensure we have the necessary directories
mkdir -p build output/Resources/AIData output/Resources/Models
mkdir -p external/dobby/include external/dobby/lib

# Clone Dobby if not present
if [ ! -d "external/dobby/dobby_source" ]; then
  echo "Downloading Dobby..."
  git clone --depth=1 https://github.com/jmpews/Dobby.git external/dobby/dobby_source
  
  # Build Dobby
  echo "Building Dobby..."
  cd external/dobby/dobby_source
  mkdir -p build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release -DDOBBY_BUILD_SHARED_LIBRARY=OFF
  make -j4
  
  # Copy the results
  cp libdobby.a ../../lib/
  cp -r ../include/* ../../include/
  cd ../../../../
fi

# Configure and build our project
echo "Configuring project..."
cmake -S . -B build \
  -DCMAKE_OSX_ARCHITECTURES="arm64" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="15.0" \
  -DCMAKE_BUILD_TYPE=Release

echo "Building dylib..."
cmake --build build --config Release

# Copy the output
cp build/lib/libmylibrary.dylib output/ 2>/dev/null || echo "Build didn't complete successfully"

echo "Build process completed."
