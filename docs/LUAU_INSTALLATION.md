# Luau Installation Guide

This project uses Luau (Roblox's Lua implementation) libraries for script execution. 

## Required Files

You need to have the following Luau files installed:

1. **Header Files**:
   - `lua.h`
   - `luaconf.h`
   - `lualib.h`
   - `lauxlib.h`

2. **Library Files**:
   - `libLuau.VM.a` or `Luau.VM.a` (required)
   - `libLuau.Compiler.a` or `Luau.Compiler.a` (optional)

## Default Location

By default, the build system looks for Luau files in these locations:

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

## Installing Luau

If you don't already have Luau installed, you can build it from source:

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

Then place the files in the expected locations:

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

## Custom Locations

If your Luau files are in a different location, you can specify the paths when running CMake:

```bash
cmake \
  -DLUAU_ROOT=/path/to/luau \
  -DLUAU_INCLUDE_DIR=/path/to/luau/VM/include \
  -DLUAU_VM_LIBRARY=/path/to/luau/build/libLuau.VM.a \
  ..
```

## Troubleshooting

### Missing Headers Error

If you get an error like:
```
Luau headers not found at /path/to/include
```

Make sure:
1. The header files exist in the specified directory
2. Set the correct path with `-DLUAU_INCLUDE_DIR=/correct/path/to/headers`

### Missing Library Error

If you get an error like:
```
Luau VM library not found at /path/to/library
```

Make sure:
1. The library file exists in the specified directory
2. Check if the library has a different name (e.g., `Luau.VM.a` instead of `libLuau.VM.a`)
3. Set the correct path with `-DLUAU_VM_LIBRARY=/correct/path/to/library`
