# Migration Guide: CodeMagic to GitHub Actions

This guide explains how to migrate from CodeMagic to GitHub Actions for building the JIT Enabler iOS app.

## Why Migrate?

If you're experiencing issues with CodeMagic builds, such as the "Invalid encryption key" error, migrating to GitHub Actions can provide:
- Better integration with your GitHub repository
- More control over the build process
- Potentially lower costs
- Elimination of team-specific encryption key issues

## Robust Build Approach

This migration implements a robust build process:
- Multiple build methods (Makefile and script) for reliability
- Proper IPA file structure creation
- IPA verification steps
- No code signing required (builds for simulator)
- No GitHub secrets needed (values hardcoded)
- No TestFlight deployment (artifact only)

## Migration Steps

### 1. Add the Build Tools

The migration includes several build tools:

1. **Makefile** (`Makefile.ios`): Provides a structured build process with various targets
2. **Build Script** (`scripts/build_ipa.sh`): Offers an alternative build method
3. **GitHub Actions Workflow** (`.github/workflows/ios-app-build.yml`): Orchestrates the build process

### 2. Remove CodeMagic Configuration

You can now remove or disable the CodeMagic configuration:

1. Either delete the `codemagic.yaml` file
2. Or keep it for reference but disable the workflow in the CodeMagic dashboard

### 3. Trigger Your First Build

1. Push your changes to GitHub
2. Go to the Actions tab in your repository
3. You should see the "Build iOS App" workflow running
4. Check the logs for any issues

### 4. Download the Build

After the workflow completes:
1. Go to the Actions tab in your GitHub repository
2. Click on the completed workflow run
3. Scroll down to the "Artifacts" section
4. Download the "ios-app-build" artifact which contains the IPA file

## Build Process Details

### Makefile Targets

The `Makefile.ios` includes several useful targets:

- `clean`: Removes build artifacts
- `setup`: Sets up environment variables
- `build-simulator`: Builds the app for iOS simulator
- `create-simulator-ipa`: Creates an IPA from the simulator build
- `build-ipa`: Main target that builds the IPA file
- `info`: Shows build information

### Build Script

The `scripts/build_ipa.sh` script:

1. Builds the app for iOS simulator
2. Creates a proper IPA file structure with Payload directory
3. Adds required metadata
4. Packages everything into an IPA file
5. Verifies the build

### GitHub Actions Workflow

The workflow:

1. Sets up the macOS build environment
2. Tries building with the Makefile
3. Falls back to the build script if needed
4. Verifies the IPA file structure
5. Uploads the build as a GitHub Actions artifact

## Troubleshooting

### Build Failures

If the build fails:
- Check the GitHub Actions logs for specific error messages
- Try building locally using the Makefile or script
- Verify that your Xcode project builds locally without issues
- Ensure all dependencies are correctly specified

### Simulator Build Limitations

Note that building for the simulator has some limitations:
- The app will only run on iOS simulators, not real devices
- Some device-specific features may not work in the simulator

## Future Enhancements

If you need to deploy to real devices or TestFlight in the future:
1. Set up code signing with certificates and provisioning profiles
2. Configure App Store Connect API access
3. Update the workflow to build for real devices
4. Add TestFlight deployment steps

## Need Help?

If you encounter issues during migration:
1. Check the GitHub Actions logs for detailed error messages
2. Try building locally using the provided tools
3. Review Apple's documentation on Xcode builds
4. Consider consulting with an iOS CI/CD specialist if problems persist