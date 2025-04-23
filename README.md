# JIT Enabler iOS App

This repository contains the JIT Enabler iOS application and related components.

## Building the App

The iOS app is built using GitHub Actions. The workflow automatically:

1. Sets up the iOS build environment
2. Builds the app for iOS simulator (no code signing required)
3. Creates a zip file of the app (as an IPA)
4. Uploads the build as a GitHub Actions artifact

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

### Manual Build Trigger

To manually trigger a build:
1. Go to the Actions tab in your GitHub repository
2. Select the "Build iOS App" workflow
3. Click "Run workflow"
4. Select the branch to build from
5. Click "Run workflow"