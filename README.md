# Roblox Executor iOS

An advanced iOS executor for Roblox with integrated AI functionality for script generation and vulnerability detection.

## Features

- Lua script execution
- Local AI-powered script generation
- Game vulnerability detection
- Advanced anti-detection capabilities
- Fully offline operation

## Building

To build the project, use the provided CMake configuration:

```bash
mkdir -p build
cmake -S . -B build -DCMAKE_OSX_ARCHITECTURES="arm64" -DCMAKE_OSX_DEPLOYMENT_TARGET="15.0" -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=iOS
cmake --build build --config Release -j4
```

## Dependencies

- Lua
- Dobby (optional, for hooking functionality)
- LuaFileSystem

## AI Features

The executor includes a completely local AI system for:
- Generating scripts based on natural language descriptions
- Detecting vulnerabilities in Roblox games
- Analyzing and improving scripts automatically

