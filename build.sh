#!/bin/bash
set -e

# Define directories
BUILD_DIR="build"
OUTPUT_DIR="output"
LIB_NAME="libmylibrary.dylib"
IOS_SDK="/Applications/Xcode_16.2.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.2.sdk"

# Create build directories
mkdir -p $BUILD_DIR/VM $BUILD_DIR/CPP $OUTPUT_DIR

# Clean previous build artifacts
echo "Cleaning previous build artifacts..."
rm -rf $BUILD_DIR/VM/*.o $BUILD_DIR/CPP/*.o $BUILD_DIR/*.a $BUILD_DIR/$LIB_NAME $OUTPUT_DIR/$LIB_NAME

# Compiler flags
CXXFLAGS="-std=c++17 -fPIC -O3 -Wall -Wextra -fvisibility=hidden -Wno-unused-parameter -Wno-unused-variable -ferror-limit=0 -fno-limit-debug-info"
PLATFORM_FLAGS="-isysroot $IOS_SDK -arch arm64 -mios-version-min=15.0"
DEFS="-DPRODUCTION_BUILD=1 -DUSE_DOBBY=1 -DENABLE_AI_FEATURES=0 -DENABLE_ADVANCED_BYPASS=1 -DLUAU_PLATFORM_IOS=1 -DLUAU_TARGET_IOS=1 -DIOS_VERSION=15.0"
INCLUDES="-I. -I/usr/local/include -I$IOS_SDK/usr/include -IVM/include -IVM/src -Isource -Iinclude"

# Create a dummy main.cpp file for linking
echo "Creating dummy main.cpp for linking..."
cat > $BUILD_DIR/main.cpp << EOF
extern "C" int main(int argc, char** argv) {
    return 0;
}
EOF

# Compile the dummy main.cpp
clang++ $CXXFLAGS $PLATFORM_FLAGS $DEFS $INCLUDES -c -o $BUILD_DIR/main.o $BUILD_DIR/main.cpp

# First compile VM sources to object files
echo "Compiling VM sources..."
VM_SOURCES=$(find VM/src -name "*.cpp" | sort)
VM_OBJECTS=""

for src in $VM_SOURCES; do
    base=$(basename "$src" .cpp)
    obj="$BUILD_DIR/VM/$base.o"
    VM_OBJECTS="$VM_OBJECTS $obj"
    echo "Compiling $src to $obj"
    clang++ $CXXFLAGS $PLATFORM_FLAGS $DEFS $INCLUDES -c -o "$obj" "$src"
    if [ $? -ne 0 ]; then
        echo "Error compiling $src"
        exit 1
    fi
done

# Create VM static library
echo "Creating VM static library..."
ar rcs $BUILD_DIR/libvm.a $VM_OBJECTS

# Compile CPP sources
echo "Compiling CPP sources..."
CPP_SOURCES=$(find source/cpp -name "*.cpp" | sort)
CPP_OBJECTS=""

for src in $CPP_SOURCES; do
    dir=$(dirname "$src" | sed 's|source/cpp|'$BUILD_DIR'/CPP|')
    base=$(basename "$src" .cpp)
    mkdir -p "$dir"
    obj="$dir/$base.o"
    CPP_OBJECTS="$CPP_OBJECTS $obj"
    echo "Compiling $src to $obj"
    clang++ $CXXFLAGS $PLATFORM_FLAGS $DEFS $INCLUDES -c -o "$obj" "$src"
    if [ $? -ne 0 ]; then
        echo "Error compiling $src"
        exit 1
    fi
done

# Compile MM sources
echo "Compiling MM sources..."
MM_SOURCES=$(find source/cpp -name "*.mm" | sort)
MM_OBJECTS=""

for src in $MM_SOURCES; do
    dir=$(dirname "$src" | sed 's|source/cpp|'$BUILD_DIR'/CPP|')
    base=$(basename "$src" .mm)
    mkdir -p "$dir"
    obj="$dir/$base.o"
    MM_OBJECTS="$MM_OBJECTS $obj"
    echo "Compiling $src to $obj"
    clang++ $CXXFLAGS $PLATFORM_FLAGS $DEFS $INCLUDES -c -o "$obj" "$src" -fobjc-arc
    if [ $? -ne 0 ]; then
        echo "Error compiling $src"
        exit 1
    fi
done

# Create CPP static library
echo "Creating CPP static library..."
ar rcs $BUILD_DIR/libcpp.a $CPP_OBJECTS $MM_OBJECTS

# Link all libraries into the final dylib
echo "Linking $LIB_NAME..."
clang++ $CXXFLAGS $PLATFORM_FLAGS -dynamiclib -o $BUILD_DIR/$LIB_NAME $BUILD_DIR/main.o $VM_OBJECTS $CPP_OBJECTS $MM_OBJECTS \
    -framework Foundation -framework UIKit -framework CoreGraphics -framework CoreFoundation \
    -framework Security -framework CoreML -framework Vision -framework Metal -framework MetalKit

# Copy to output directory
cp $BUILD_DIR/$LIB_NAME $OUTPUT_DIR/

echo "Build complete: $OUTPUT_DIR/$LIB_NAME"