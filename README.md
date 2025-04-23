# JIT Enabler iOS App

This repository contains the JIT Enabler iOS application and related components.

## Building the App

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

### Manual Workflow Trigger

To manually trigger a build in GitHub Actions:
1. Go to the Actions tab in your GitHub repository
2. Select the "Build iOS App" workflow
3. Click "Run workflow"
4. Select the branch to build from
5. Click "Run workflow"