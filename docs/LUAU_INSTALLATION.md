# Using Pre-installed Luau Libraries

This document explains how to use pre-installed Luau (Roblox Lua) libraries with this project instead of building them during the CMake process.

## Why Use Pre-installed Luau?

1. **Faster Builds**: No need to build Luau during each CMake run
2. **More Control**: Use specific versions of Luau 
3. **Better Reliability**: Avoid build-time issues with Luau compilation
4. **Custom Modifications**: Use your custom-built Luau version

## Installation Steps

### 1. Build and Install Luau

First, build Luau from source:

```bash
# Clone the repository
git clone https://github.com/Roblox/luau.git
cd luau

# Create build directory
mkdir build && cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release -DLUAU_BUILD_TESTS=OFF
cmake --build . --target Luau.VM Luau.Compiler
```

### 2. Organize Luau Files

Organize the Luau files in the expected directory structure:

```
external/luau/
├── VM/
│   └── include/
│       ├── lua.h
│       ├── luaconf.h
│       ├── lualib.h
│       └── lauxlib.h
└── build/
    ├── libLuau.VM.a (or Luau.VM.a)
    └── libLuau.Compiler.a (or Luau.Compiler.a)
```

You can create this structure by:

```bash
# Create directories
mkdir -p external/luau/VM/include
mkdir -p external/luau/build

# Copy headers
cp luau/VM/include/*.h external/luau/VM/include/

# Copy libraries
cp luau/build/libLuau.VM.a external/luau/build/
cp luau/build/libLuau.Compiler.a external/luau/build/
```

### 3. Configure CMake to Use Pre-installed Luau

When configuring your project with CMake, set the following options:

```bash
cmake -DUSE_LUAU=ON -DUSE_PREINSTALLED_LUAU=ON ..
```

If you've placed Luau in a different location, you can specify the paths:

```bash
cmake -DUSE_LUAU=ON -DUSE_PREINSTALLED_LUAU=ON \
      -DLUAU_ROOT=/path/to/luau \
      -DLUAU_INCLUDE_DIR=/path/to/luau/VM/include \
      -DLUAU_VM_LIBRARY=/path/to/luau/build/libLuau.VM.a \
      -DLUAU_COMPILER_LIBRARY=/path/to/luau/build/libLuau.Compiler.a \
      ..
```

## Troubleshooting

### Missing Headers

If CMake can't find Luau headers, verify that:
- The header files (lua.h, lualib.h, etc.) exist in your include directory
- The path to `LUAU_INCLUDE_DIR` is correctly set

### Library Not Found

If CMake can't find Luau libraries, verify that:
- The library files (libLuau.VM.a, etc.) exist in your build directory
- The library files have the correct names (may be `Luau.VM.a` instead of `libLuau.VM.a`)
- The paths to `LUAU_VM_LIBRARY` and `LUAU_COMPILER_LIBRARY` are correctly set

### Linking Errors

If you get linking errors:
- Make sure the Luau libraries were compiled for your target architecture
- Check that you're using the correct version of Luau compatible with your code
