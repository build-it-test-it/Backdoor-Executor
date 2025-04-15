#!/bin/bash
# Standardize build workflow to use a consistent approach

echo "==== Standardizing Build Workflow ===="

# 1. Create a consistent GitHub workflow file
echo "Creating standardized GitHub workflow..."
cat > .github/workflows/build.yml << 'EOL'
name: Build Roblox Executor iOS Dynamic Library

on:
  push:
    branches:
      - main
  pull_request:
    branches:
      - main
  workflow_dispatch:

jobs:
  build:
    runs-on: macos-latest  # Use macOS for iOS compatible builds

    steps:
    - name: Checkout repository
      uses: actions/checkout@v3

    - name: Install dependencies
      run: |
        echo "Installing dependencies..."
        brew update
        brew install cmake pkg-config
        brew install luau || true
        
        # Create required directories
        mkdir -p external/dobby/include
        mkdir -p external/dobby/lib
        mkdir -p output/Resources/AIData
        mkdir -p build
        
        # Remove any CI_BUILD defines that might remain
        find source -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.cpp" -o -name "*.mm" -o -name "*.c" \) | \
          xargs sed -i '' 's/#define CI_BUILD//g' 2>/dev/null || true

    - name: Setup Xcode
      uses: maxim-lobanov/setup-xcode@v1
      with:
        xcode-version: latest-stable

    - name: Build Dobby
      id: install-dobby
      run: |
        echo "Building Dobby from source (required dependency)..."
        git clone --depth=1 https://github.com/jmpews/Dobby.git
        cd Dobby
        mkdir -p build && cd build
        
        # Configure and build Dobby
        cmake .. \
          -DCMAKE_BUILD_TYPE=Release \
          -DDOBBY_BUILD_SHARED_LIBRARY=OFF \
          -DDOBBY_BUILD_STATIC_LIBRARY=ON
        
        cmake --build . --config Release
        
        # Copy Dobby files to expected location
        mkdir -p $GITHUB_WORKSPACE/external/dobby/lib
        mkdir -p $GITHUB_WORKSPACE/external/dobby/include
        
        cp libdobby.a $GITHUB_WORKSPACE/external/dobby/lib/
        cp -r ../include/* $GITHUB_WORKSPACE/external/dobby/include/
        
        echo "Dobby successfully built and installed to external/dobby"
        cd $GITHUB_WORKSPACE

    - name: Apply Lua Compatibility Fixes
      run: |
        echo "Setting up Lua compatibility fixes..."
        ./fix_lua_includes.sh
        ./patch_lfs.sh

    - name: Build Dynamic Library
      run: |
        echo "Building the iOS dynamic library..."
        
        # Configure with Lua paths
        cmake -S . -B build \
          -DCMAKE_OSX_ARCHITECTURES="arm64" \
          -DCMAKE_OSX_DEPLOYMENT_TARGET="15.0" \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_SYSTEM_NAME=iOS \
          -DENABLE_AI_FEATURES=ON \
          -DUSE_DOBBY=ON
        
        # Build the library
        cmake --build build --config Release -j4
        
        # Check if build produced the library
        if [ -f "build/lib/libmylibrary.dylib" ]; then
          echo "✅ Successfully built libmylibrary.dylib"
          ls -la build/lib/libmylibrary.dylib
          
          # Copy to output directory
          mkdir -p output
          cp build/lib/libmylibrary.dylib output/
          
          # Copy any resources
          if [ -d "Resources" ]; then
            mkdir -p output/Resources
            cp -r Resources/* output/Resources/ 2>/dev/null || true
          fi
          
          # Create default config if it doesn't exist
          mkdir -p output/Resources/AIData
          if [ ! -f "output/Resources/AIData/config.json" ]; then
            echo '{"version":"1.0.0","led_effects":true,"ai_features":true,"memory_optimization":true}' > output/Resources/AIData/config.json
          fi
          
          echo "== Built files =="
          ls -la output/
        else
          echo "❌ Failed to build libmylibrary.dylib"
          echo "== Build directory contents =="
          find build -name "*.dylib" -o -name "*.a"
          exit 1
        fi

    - name: Verify Library
      run: |
        echo "Verifying built dylib..."
        
        if [ -f "output/libmylibrary.dylib" ]; then
          echo "✅ libmylibrary.dylib exists"
          
          # Check for exported symbols
          echo "Exported symbols:"
          nm -g output/libmylibrary.dylib | grep -E "luaopen_|ExecuteScript" || echo "No key symbols found!"
          
          # Check library type
          file output/libmylibrary.dylib
          
          # Check library dependencies
          otool -L output/libmylibrary.dylib
        else
          echo "❌ libmylibrary.dylib not found in output directory"
          exit 1
        fi

    - name: Upload Artifact
      uses: actions/upload-artifact@v3
      with:
        name: ios-dylib
        path: |
          output/libmylibrary.dylib
          output/Resources/**
EOL

# 2. Create an updated build script for local builds
echo "Creating standardized build script..."
cat > build_dylib.sh << 'EOL'
#!/bin/bash
# Build script for Roblox Executor dylib with real implementations

# Stop on any error
set -e

# Define colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=====================================================${NC}"
echo -e "${BLUE}    Roblox Executor iOS Dynamic Library Builder       ${NC}"
echo -e "${BLUE}=====================================================${NC}"

# Ensure we have the necessary directories
mkdir -p build output/Resources/AIData output/Resources/Models
mkdir -p external/dobby/include external/dobby/lib

# Fix all stub implementations
./fix_production_code.sh

# Clone Dobby if not present
if [ ! -f "external/dobby/lib/libdobby.a" ]; then
  echo -e "${YELLOW}Downloading and building Dobby...${NC}"
  git clone --depth=1 https://github.com/jmpews/Dobby.git dobby_temp
  
  # Build Dobby
  cd dobby_temp
  mkdir -p build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release -DDOBBY_BUILD_SHARED_LIBRARY=OFF -DDOBBY_BUILD_STATIC_LIBRARY=ON
  make -j4
  
  # Copy the results
  cp libdobby.a ../../external/dobby/lib/
  cp -r ../include/* ../../external/dobby/include/
  cd ../../
  
  # Clean up
  rm -rf dobby_temp
  echo -e "${GREEN}Dobby successfully built and installed${NC}"
else
  echo -e "${GREEN}Found existing Dobby installation${NC}"
fi

# Fix Lua includes
echo -e "${YELLOW}Applying Lua compatibility fixes...${NC}"
./fix_lua_includes.sh
./patch_lfs.sh

# Configure and build our project
echo -e "${YELLOW}Configuring project...${NC}"
cmake -S . -B build \
  -DCMAKE_OSX_ARCHITECTURES="arm64" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="15.0" \
  -DCMAKE_BUILD_TYPE=Release

echo -e "${YELLOW}Building dylib...${NC}"
cmake --build build --config Release -j4

# Copy the output
if [ -f "build/lib/libmylibrary.dylib" ]; then
  echo -e "${GREEN}Build successful${NC}"
  cp build/lib/libmylibrary.dylib output/
  
  # Create default config if it doesn't exist
  if [ ! -f "output/Resources/AIData/config.json" ]; then
    echo '{"version":"1.0.0","led_effects":true,"ai_features":true,"memory_optimization":true}' > output/Resources/AIData/config.json
  fi
  
  echo -e "${GREEN}=====================================================${NC}"
  echo -e "${GREEN}    Build completed successfully!                     ${NC}"
  echo -e "${GREEN}=====================================================${NC}"
  echo -e "The dynamic library is available at: ${BLUE}output/libmylibrary.dylib${NC}"
else
  echo -e "${RED}Build failed!${NC}"
  echo "Check the build logs for errors."
  exit 1
fi
EOL

chmod +x build_dylib.sh

# 3. Clean up unused workflow files
echo "Cleaning up unused build files..."
rm -f new_build_workflow.yml
mv .github/workflows/build.yml .github/workflows/build.yml.old
mv .github/workflows/fixed_build.yml .github/workflows/fixed_build.yml.old
mv .github/workflows/lua_build.yml .github/workflows/lua_build.yml.old
mv .github/workflows/o.yml .github/workflows/o.yml.old

echo "==== Build Standardization Complete ===="
