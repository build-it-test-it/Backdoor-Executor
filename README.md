# iOS Roblox Executor

This repository contains the iOS Roblox Executor, a dynamic library for enhancing Roblox on iOS devices.

## Features

- Memory reading and writing
- Method hooking for Roblox iOS app
- Script execution within Roblox 
- UI injection for executor interface
- AI-powered script generation and optimization

## Building

To build the project:

```bash
mkdir build && cd build
cmake ..
make
```

This will generate `libmylibrary.dylib` in the build directory.

## Directory Structure

- `source/` - Source code
  - `source/cpp/` - C++ implementation
  - `source/cpp/ios/` - iOS-specific code
  - `source/cpp/memory/` - Memory manipulation utilities
- `output/` - Build output directory
  - `output/Resources/` - Resource files
- `cmake/` - CMake modules and utilities

## AI Features

The executor includes AI-powered features for script generation and optimization. 
Configuration files for these features are located in `output/Resources/AIData/`.
