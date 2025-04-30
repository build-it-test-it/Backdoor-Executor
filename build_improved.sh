#!/bin/bash
# Improved build script for iOS Roblox Executor

set -e

# Build configuration
BUILD_TYPE="Release"
ENABLE_AI_FEATURES=0
ENABLE_ADVANCED_BYPASS=1
USE_DOBBY=1

# Output directories
BUILD_DIR="build"
OUTPUT_DIR="output"
LIB_NAME="libmylibrary.dylib"

# Create directories
mkdir -p $BUILD_DIR
mkdir -p $OUTPUT_DIR

# Clean previous build artifacts
echo "Cleaning previous build artifacts..."
rm -rf $BUILD_DIR/* $OUTPUT_DIR/*

# Create dummy main.cpp for linking
echo "Creating dummy main.cpp for linking..."
mkdir -p $BUILD_DIR
cat > $BUILD_DIR/main.cpp << EOF
extern "C" int main(int argc, char** argv) { 
    return 0; 
}
EOF

# Fix unused variables in VM source files
echo "Fixing unused variables in VM source files..."
if [ -f VM/src/lfunc.cpp ]; then
    sed -i '' 's/GCObject\* o = obj2gco(uv);/(void)obj2gco(uv); \/\/ Prevent unused variable warning/g' VM/src/lfunc.cpp
    sed -i '' 's/global_State\* g = L->global;/(void)L->global; \/\/ Prevent unused variable warning/g' VM/src/lfunc.cpp
fi

if [ -f VM/src/lvmexecute.cpp ]; then
    sed -i '' 's/Instruction insn = \*pc++;/Instruction insn = \*pc++; (void)insn; \/\/ Prevent unused variable warning/g' VM/src/lvmexecute.cpp
fi

if [ -f VM/src/lvmroblox.cpp ]; then
    sed -i '' 's/VMMetrics metrics = {};/VMMetrics metrics = {0, 0, 0}; \/\/ Initialize all fields/g' VM/src/lvmroblox.cpp
fi

# Fix include paths in hooks.hpp
if [ -f source/cpp/hooks/hooks.hpp ]; then
    sed -i '' 's/#include "..\/..\/include\/objc\/runtime.h"/#include <objc\/runtime.h>/g' source/cpp/hooks/hooks.hpp
fi

# Compile VM sources
echo "Compiling VM sources..."
VM_SOURCES=$(find VM/src -name "*.cpp" 2>/dev/null)
VM_OBJECTS=""

for src in $VM_SOURCES; do
    obj="$BUILD_DIR/VM/$(basename ${src%.cpp}.o)"
    mkdir -p "$(dirname $obj)"
    echo "Compiling $src to $obj"
    clang++ -std=c++17 -fPIC -O3 -Wall -Wextra -fvisibility=hidden -ferror-limit=0 -fno-limit-debug-info \
        -isysroot /Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk \
        -arch arm64 -mios-version-min=15.0 -DIOS_VERSION=15.0 -DLUAU_PLATFORM_IOS=1 -DLUAU_TARGET_IOS=1 \
        -DPRODUCTION_BUILD=1 -DUSE_DOBBY=1 -DENABLE_AI_FEATURES=0 -DENABLE_ADVANCED_BYPASS=1 \
        -I. -I/usr/local/include -I/Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk/usr/include \
        -IVM/include -IVM/src -Iinclude -c -o $obj $src
    VM_OBJECTS="$VM_OBJECTS $obj"
done

# Compile CPP sources
echo "Compiling CPP sources..."
CPP_SOURCES=$(find source/cpp -maxdepth 1 -name "*.cpp" 2>/dev/null)
CPP_SOURCES+=" $(find source/cpp/memory -name "*.cpp" 2>/dev/null)"
CPP_SOURCES+=" $(find source/cpp/security -name "*.cpp" 2>/dev/null)"
CPP_SOURCES+=" $(find source/cpp/hooks -name "*.cpp" 2>/dev/null)"
CPP_SOURCES+=" $(find source/cpp/naming_conventions -name "*.cpp" 2>/dev/null)"
CPP_OBJECTS=""

for src in $CPP_SOURCES; do
    obj="$BUILD_DIR/$(echo $src | sed 's/\.cpp$/.o/')"
    mkdir -p "$(dirname $obj)"
    echo "Compiling $src to $obj"
    clang++ -std=c++17 -fPIC -O3 -Wall -Wextra -fvisibility=hidden -ferror-limit=0 -fno-limit-debug-info \
        -isysroot /Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk \
        -arch arm64 -mios-version-min=15.0 -DIOS_VERSION=15.0 -DLUAU_PLATFORM_IOS=1 -DLUAU_TARGET_IOS=1 \
        -DPRODUCTION_BUILD=1 -DUSE_DOBBY=1 -DENABLE_AI_FEATURES=0 -DENABLE_ADVANCED_BYPASS=1 \
        -I. -I/usr/local/include -I/Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk/usr/include \
        -IVM/include -IVM/src -Iinclude -c -o $obj $src
    CPP_OBJECTS="$CPP_OBJECTS $obj"
done

# Compile iOS CPP sources
echo "Compiling iOS CPP sources..."
iOS_CPP_SOURCES=$(find source/cpp/ios -name "*.cpp" 2>/dev/null)
iOS_CPP_OBJECTS=""

for src in $iOS_CPP_SOURCES; do
    obj="$BUILD_DIR/$(echo $src | sed 's/\.cpp$/.o/')"
    mkdir -p "$(dirname $obj)"
    echo "Compiling $src to $obj"
    clang++ -std=c++17 -fPIC -O3 -Wall -Wextra -fvisibility=hidden -ferror-limit=0 -fno-limit-debug-info \
        -isysroot /Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk \
        -arch arm64 -mios-version-min=15.0 -DIOS_VERSION=15.0 -DLUAU_PLATFORM_IOS=1 -DLUAU_TARGET_IOS=1 \
        -DPRODUCTION_BUILD=1 -DUSE_DOBBY=1 -DENABLE_AI_FEATURES=0 -DENABLE_ADVANCED_BYPASS=1 \
        -I. -I/usr/local/include -I/Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk/usr/include \
        -IVM/include -IVM/src -Iinclude -c -o $obj $src
    iOS_CPP_OBJECTS="$iOS_CPP_OBJECTS $obj"
done

# Compile iOS MM sources
echo "Compiling iOS MM sources..."
iOS_MM_SOURCES=$(find source/cpp/ios -name "*.mm" 2>/dev/null)
iOS_MM_OBJECTS=""

for src in $iOS_MM_SOURCES; do
    obj="$BUILD_DIR/$(echo $src | sed 's/\.mm$/.o/')"
    mkdir -p "$(dirname $obj)"
    echo "Compiling $src to $obj"
    clang++ -std=c++17 -fPIC -O3 -Wall -Wextra -fvisibility=hidden -ferror-limit=0 -fno-limit-debug-info \
        -isysroot /Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk \
        -arch arm64 -mios-version-min=15.0 -DIOS_VERSION=15.0 -DLUAU_PLATFORM_IOS=1 -DLUAU_TARGET_IOS=1 \
        -DPRODUCTION_BUILD=1 -DUSE_DOBBY=1 -DENABLE_AI_FEATURES=0 -DENABLE_ADVANCED_BYPASS=1 \
        -I. -I/usr/local/include -I/Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk/usr/include \
        -IVM/include -IVM/src -Iinclude -c -o $obj $src
    iOS_MM_OBJECTS="$iOS_MM_OBJECTS $obj"
done

# Create static libraries
echo "Creating VM static library..."
ar rcs $BUILD_DIR/libvm.a $VM_OBJECTS

echo "Creating CPP static library..."
ar rcs $BUILD_DIR/libcpp.a $CPP_OBJECTS $iOS_CPP_OBJECTS $iOS_MM_OBJECTS

# Compile main.cpp
echo "Compiling main.cpp..."
clang++ -std=c++17 -fPIC -O3 -Wall -Wextra -fvisibility=hidden -ferror-limit=0 -fno-limit-debug-info \
    -isysroot /Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk \
    -arch arm64 -mios-version-min=15.0 -DIOS_VERSION=15.0 -DLUAU_PLATFORM_IOS=1 -DLUAU_TARGET_IOS=1 \
    -DPRODUCTION_BUILD=1 -DUSE_DOBBY=1 -DENABLE_AI_FEATURES=0 -DENABLE_ADVANCED_BYPASS=1 \
    -I. -I/usr/local/include -I/Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk/usr/include \
    -IVM/include -IVM/src -Iinclude -c -o $BUILD_DIR/main.o $BUILD_DIR/main.cpp

# Link everything together
echo "Linking final library..."
clang++ -shared -undefined dynamic_lookup \
    -framework Foundation -framework UIKit -framework CoreGraphics -framework CoreFoundation -framework Security \
    -framework CoreML -framework Vision -framework Metal -framework MetalKit -framework AVFoundation \
    -framework CoreMedia -framework CoreVideo -framework CoreImage -framework CoreLocation \
    -framework CoreBluetooth -framework CoreMotion -framework CoreAudio -framework CoreHaptics \
    -isysroot /Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk \
    -arch arm64 -mios-version-min=15.0 \
    -o $OUTPUT_DIR/$LIB_NAME $BUILD_DIR/main.o -Wl,-force_load,$BUILD_DIR/libvm.a -Wl,-force_load,$BUILD_DIR/libcpp.a \
    -install_name @executable_path/Frameworks/$LIB_NAME

echo "✅ Build completed successfully!"
echo "Output library: $OUTPUT_DIR/$LIB_NAME"