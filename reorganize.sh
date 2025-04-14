#!/bin/bash

# Create directories if needed
mkdir -p lib
mkdir -p output
mkdir -p Resources/AIData/LocalModels
mkdir -p Resources/AIData/Vulnerabilities
mkdir -p workspace

# Create sample configuration file 
echo '{
  "version": "1.0.0",
  "createdAt": "'$(date -u +"%Y-%m-%dT%H:%M:%SZ")'",
  "settings": {
    "enableLocalTraining": true,
    "enableVulnerabilityDetection": true,
    "operationMode": "standard"
  }
}' > Resources/AIData/config.json

# Remove unnecessary files
rm -f Resources/Models/*.mlmodel
rm -f source/cpp/ios/ai_features/AIIntegration.mm.backup
rm -f source/cpp/ios/ai_features/AIConfig_updated.h source/cpp/ios/ai_features/AIConfig_updated.mm
rm -f source/cpp/ios/ai_features/AIIntegrationManager_updated.h
rm -f source/cpp/ios/ai_features/AIIntegration_updated.mm

# Rename updated files
mv source/cpp/ios/ai_features/AIConfig_updated.h source/cpp/ios/ai_features/AIConfig.h 2>/dev/null || true
mv source/cpp/ios/ai_features/AIConfig_updated.mm source/cpp/ios/ai_features/AIConfig.mm 2>/dev/null || true
mv source/cpp/ios/ai_features/AIIntegrationManager_updated.h source/cpp/ios/ai_features/AIIntegrationManager.h 2>/dev/null || true

# Create a README file in the root directory
echo '# Roblox Executor iOS

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
' > README.md

echo "Reorganization completed"
