#!/bin/bash
# Build script for the Roblox Executor iOS Dynamic Library
# This script automatically downloads and builds Dobby if needed,
# then builds the executor dylib with real implementations.

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

# Create necessary directories
mkdir -p build external/dobby/include external/dobby/lib output/Resources/AIData

# Remove any CI_BUILD definitions from source files
echo -e "${YELLOW}Removing CI_BUILD definitions from source files...${NC}"
find source -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.cpp" -o -name "*.mm" \) | xargs sed -i -e "s/#define CI_BUILD//g" 2>/dev/null || true

# Check if Dobby is already built
if [ ! -f "external/dobby/lib/libdobby.a" ]; then
    echo -e "${YELLOW}Dobby not found, downloading and building...${NC}"
    
    # Clone Dobby repository
    mkdir -p build/dobby-src
    git clone --depth=1 https://github.com/jmpews/Dobby.git build/dobby-src || {
        echo -e "${RED}Failed to clone Dobby repository${NC}"
        exit 1
    }
    
    # Build Dobby
    cd build/dobby-src
    mkdir -p build
    cd build
    
    echo -e "${YELLOW}Configuring Dobby...${NC}"
    cmake .. -DCMAKE_BUILD_TYPE=Release -DDOBBY_BUILD_SHARED_LIBRARY=OFF -DDOBBY_BUILD_STATIC_LIBRARY=ON || {
        echo -e "${RED}Failed to configure Dobby${NC}"
        cd ../../../
        exit 1
    }
    
    echo -e "${YELLOW}Building Dobby...${NC}"
    cmake --build . --config Release || {
        echo -e "${RED}Failed to build Dobby${NC}"
        cd ../../../
        exit 1
    }
    
    # Copy the Dobby files
    echo -e "${YELLOW}Installing Dobby...${NC}"
    cp -r ../include/* ../../../external/dobby/include/
    cp libdobby.a ../../../external/dobby/lib/
    
    cd ../../../
    echo -e "${GREEN}Dobby successfully built and installed${NC}"
else
    echo -e "${GREEN}Found existing Dobby installation${NC}"
fi

# Create a basic AIData config if it doesn't exist
if [ ! -f "output/Resources/AIData/config.json" ]; then
    echo -e "${YELLOW}Creating default config.json...${NC}"
    echo '{
        "version": "1.0.0",
        "led_effects": true,
        "ai_features": true,
        "memory_optimization": true
    }' > output/Resources/AIData/config.json
fi

# Configure and build the project
echo -e "${YELLOW}Configuring project...${NC}"
cmake -S . -B build \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="15.0" \
    -DCMAKE_BUILD_TYPE=Release || {
    echo -e "${RED}Failed to configure the project${NC}"
    exit 1
}

echo -e "${YELLOW}Building project...${NC}"
cmake --build build --config Release -j4 || {
    echo -e "${RED}Failed to build the project${NC}"
    exit 1
}

# Copy output files
echo -e "${YELLOW}Copying output files...${NC}"
mkdir -p output

# Copy the dylib to the output directory
if [ -f "build/lib/libmylibrary.dylib" ]; then
    cp build/lib/libmylibrary.dylib output/
    echo -e "${GREEN}Successfully built and copied libmylibrary.dylib to output directory${NC}"
else
    echo -e "${RED}Failed to find the built library${NC}"
    exit 1
fi

# Copy Resources if they exist
if [ -d "Resources" ]; then
    cp -r Resources/* output/Resources/
fi

echo -e "${GREEN}=====================================================${NC}"
echo -e "${GREEN}    Build completed successfully!                     ${NC}"
echo -e "${GREEN}=====================================================${NC}"
echo -e "The dynamic library is available at: ${BLUE}output/libmylibrary.dylib${NC}"
echo -e "You can now use this library with your iOS application."
