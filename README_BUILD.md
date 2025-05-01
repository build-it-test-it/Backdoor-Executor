# iOS Roblox Executor Build System

This repository contains a comprehensive build system for the iOS Roblox Executor. It includes scripts for building, testing, and deploying the executor, as well as utilities for checking compatibility with new Roblox versions.

## Key Components

1. **Main Build Script (`build_executor.sh`)**
   - Builds the dynamic library with all necessary components
   - Sets up the build environment and dependencies (e.g., Dobby)
   - Verifies the build output
   - Usage: `./build_executor.sh [options]`

2. **Version Update Script (`version_updater.sh`)**
   - Checks for new Roblox versions
   - Updates signatures and offsets for compatibility
   - Generates update reports
   - Usage: `./version_updater.sh [options]`

3. **Integration Script (`integration.sh`)**
   - Master script that combines all other components
   - Handles version checking, building, testing, and deployment
   - Usage: `./integration.sh [options]`

4. **Diagnostic System (`source/cpp/Diagnostic.hpp` & `.cpp`)**
   - Runtime diagnostic capabilities
   - Tests core functionality (Lua VM, memory access, hooks, etc.)
   - Generates HTML and JSON reports

5. **Lua Test Script (`test_script.lua`)**
   - Verifies executor functionality from within Roblox
   - Tests core features (memory manipulation, function hooking, etc.)
   - Provides detailed test reports

## Getting Started

### Prerequisites

- macOS with Xcode (iOS SDK installed)
- Command line tools (`clang`, `make`, etc.)
- Git

### Basic Usage

1. **Build the dylib:**
   ```bash
   chmod +x build_executor.sh
   ./build_executor.sh
   ```

2. **Check for Roblox updates:**
   ```bash
   chmod +x version_updater.sh
   ./version_updater.sh
   ```

3. **Complete workflow (build, test, deploy):**
   ```bash
   chmod +x integration.sh
   ./integration.sh --install --device=192.168.1.x
   ```

### Advanced Options

#### Build Script Options
```
--debug                Build with debugging enabled
--release              Build optimized release version (default)
--enable-ai            Enable AI features
--disable-ai           Disable AI features (default)
--enable-bypass        Enable advanced bypass (default)
--disable-bypass       Disable advanced bypass
--use-dobby            Use Dobby for hooking (default)
--no-dobby             Don't use Dobby
--clean                Clean before building
--verbose              Enable verbose output
--install=DIR          Set installation directory (default: ./output)
--help                 Show this help message
```

#### Integration Script Options
```
--build-only          Only build the dylib, don't run other steps
--skip-version-check  Skip Roblox version check
--skip-diagnostics    Skip diagnostic tests after build
--install             Install to iOS device after build
--device=IP           Specify iOS device IP for installation
--password=PWD        Specify iOS device SSH password
--debug               Build debug version
--enable-ai           Enable AI features
--no-clean            Skip cleaning before build
--help                Show this help message
```

## Project Structure

The build system is designed to work with the following project structure:

```
.
├── VM/                    # Lua virtual machine files
│   ├── include/           # VM header files
│   └── src/               # VM source files
├── include/               # System headers and compatibility files
├── source/                # Main source code
│   ├── cpp/               # C++ implementation files
│   │   ├── ios/           # iOS-specific code
│   │   ├── memory/        # Memory manipulation
│   │   ├── security/      # Anti-tamper & protection
│   │   ├── hooks/         # Function hooking
│   │   └── ...            # Other subsystems
│   └── ...
├── build/                 # Build artifacts (created during build)
├── output/                # Output directory for final dylib
├── build_executor.sh      # Main build script
├── version_updater.sh     # Version checker script
├── integration.sh         # Integration script
└── test_script.lua        # Lua test script
```

## Notes on Include Folder

The `include` directory contains system header files that are necessary for compilation. These files include:

1. **iOS/macOS Compatibility Headers:**
   - `Availability.h` & `AvailabilityMacros.h` - Define API availability macros for iOS versions
   - `TargetConditionals.h` - Platform detection (iOS vs macOS)

2. **Objective-C Runtime & Foundation:**
   - `objc/NSObjCRuntime.h` - Core Objective-C runtime definitions
   - Other Objective-C headers for message passing and class structure

3. **System-level Headers:**
   - `mach-o/dyld.h` - Dynamic linker functions for hooking and symbol resolution
   - `sys/mman.h` - Memory mapping and protection used by `ProtectMemory` function

These headers are referenced throughout the codebase for iOS compatibility, particularly in:
- `source/cpp/ios_compat.h` - Platform compatibility layer
- Memory and hooking functionality 
- Security and anti-tamper features

## Building for Production

For a production build:

```bash
./integration.sh --enable-ai --install --device=YOUR_DEVICE_IP
```

This will:
1. Check for Roblox updates
2. Build the dylib with AI features enabled
3. Run diagnostics
4. Install to your iOS device

## License

See the LICENSE file in the root directory for licensing information.
