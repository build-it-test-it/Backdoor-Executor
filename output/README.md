# iOS Roblox Executor Library

This directory contains the compiled dynamic library (.dylib) for the iOS Roblox executor.

## Directory Structure

- `libmylibrary.dylib` - The main dynamic library for iOS
- `Resources/` - Resources needed by the executor
  - `AIData/` - AI-related configuration and data
    - `LocalModels/` - Local AI models for offline use
    - `Vulnerabilities/` - Vulnerability definitions and patterns
    - `config.json` - AI configuration file

## Usage

This dylib should be loaded into the Roblox iOS app process using appropriate injection methods.
Refer to the main documentation for installation instructions.
