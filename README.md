# JIT Enabler for iOS

This repository contains the JIT Enabler iOS app and backend server for enabling Just-In-Time (JIT) compilation on iOS devices.

## Components

### iOS App

The iOS app is located in the root directory and consists of:

- `JITEnabler/` - The Swift source code for the iOS app
- `JITEnabler.xcodeproj/` - The Xcode project file

### Backend Server

The backend server is located in the `JIT Backend/` directory and is built with Flask.

## Getting Started

### Running the Backend Server

1. Navigate to the JIT Backend directory:
   ```
   cd "JIT Backend"
   ```

2. Install the required dependencies:
   ```
   pip install -r requirements.txt
   ```

3. Run the server:
   ```
   python app.py
   ```

## Building the iOS App

The iOS app is built using GitHub Actions with a robust build process that:

1. Sets up the iOS build environment
2. Builds the app for iOS simulator (no code signing required)
3. Creates a properly structured IPA file
4. Verifies the IPA file structure
5. Uploads the build as a GitHub Actions artifact

### Build Tools

The repository includes multiple build methods to ensure reliability:

1. **Makefile**: `Makefile.ios` provides a structured build process with various targets
2. **Build Script**: `build-ios.sh` offers a simplified way to run the Makefile
3. **GitHub Actions Workflow**: Orchestrates the build process in CI/CD

### GitHub Actions Workflow

The build process is defined in `.github/workflows/ios-app-build.yml`. The workflow is triggered on:
- Push to the main branch
- Pull requests to the main branch
- Manual workflow dispatch

### No Secrets Required

The workflow has been simplified to not require any GitHub secrets. All necessary values are hardcoded in the workflow file.

### Downloading the Build

After the workflow completes:
1. Go to the Actions tab in your GitHub repository
2. Click on the completed workflow run
3. Scroll down to the "Artifacts" section
4. Download the "ios-app-build" artifact which contains the IPA file

### Manual Building

You can build the app locally using the following methods:

#### Using the Build Script (Recommended):
```bash
./build-ios.sh
```

#### Using Makefile Directly:
```bash
make -f Makefile.ios clean setup build-ipa
```

The IPA file will be created in the `build/ios/ipa/` directory.

#### Using Xcode Directly:
1. Open the Xcode project:
   ```
   open JITEnabler.xcodeproj
   ```
2. Configure the backend URL in the app settings
3. Build and run the app on your iOS device or simulator

### Build Options

The Makefile provides several targets for different build scenarios:

- `clean`: Removes all build artifacts
- `setup`: Sets up the build environment
- `build-simulator`: Builds the app for iOS simulator
- `create-simulator-ipa`: Creates an IPA from the simulator build
- `build-ipa`: Creates the final IPA file
- `info`: Shows information about the build configuration

### IPA File Structure

The generated IPA file is a standard iOS app package with the following structure:

```
Payload/
  JITEnabler.app/
    Info.plist
    ...app contents...
```

## How It Works

The JIT Enabler system works by:

1. Registering your device with the secure JIT backend
2. Requesting JIT enablement for your selected app
3. Applying the necessary memory permission changes to enable JIT
4. All communication is encrypted and secure

The app uses different techniques based on your iOS version to ensure compatibility with iOS 15, 16, and 17+.

## Supported Apps

The JIT Enabler works with many apps, including:

- **Emulators:** Delta, PPSSPP, UTM, iNDS, Provenance
- **JavaScript Apps:** JavaScriptCore-based apps
- **Development Tools:** iSH, a-Shell, Pythonista
- **Custom Apps:** Any app that could benefit from JIT

## License

This project is for educational and personal use only.
