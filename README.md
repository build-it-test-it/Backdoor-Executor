# JIT Enabler iOS App

This repository contains the JIT Enabler iOS application and related components.

## Building the App

The iOS app is built using GitHub Actions. The workflow automatically:

1. Sets up the iOS build environment
2. Configures code signing
3. Builds the app
4. Uploads the build to TestFlight (for non-PR builds)

### GitHub Actions Workflow

The build process is defined in `.github/workflows/ios-app-build.yml`. The workflow is triggered on:
- Push to the main branch
- Pull requests to the main branch
- Manual workflow dispatch

### Required Secrets

To use the GitHub Actions workflow, you need to set up the following secrets in your GitHub repository:

#### Code Signing
- `IOS_DISTRIBUTION_P12`: Base64-encoded P12 certificate file
- `IOS_DISTRIBUTION_P12_PASSWORD`: Password for the P12 certificate
- `IOS_DISTRIBUTION_CERTIFICATE_NAME`: Name of the distribution certificate
- `IOS_PROVISIONING_PROFILE`: Base64-encoded mobileprovision file
- `IOS_PROVISIONING_PROFILE_SPECIFIER`: Provisioning profile specifier name
- `APPLE_TEAM_ID`: Your Apple Developer Team ID

#### App Store Connect
- `APP_STORE_CONNECT_ISSUER_ID`: App Store Connect API Issuer ID
- `APP_STORE_CONNECT_KEY_IDENTIFIER`: App Store Connect API Key ID
- `APP_STORE_CONNECT_PRIVATE_KEY`: App Store Connect API Private Key

#### Dropbox Integration
- `DROPBOX_APP_KEY`: Dropbox App Key
- `DROPBOX_APP_SECRET`: Dropbox App Secret
- `DROPBOX_REFRESH_TOKEN`: Dropbox Refresh Token

## Setting Up Secrets

1. Go to your GitHub repository
2. Navigate to Settings > Secrets and variables > Actions
3. Click "New repository secret"
4. Add each of the required secrets listed above

## Manual Build

To manually trigger a build:
1. Go to the Actions tab in your GitHub repository
2. Select the "Build and Deploy iOS App" workflow
3. Click "Run workflow"
4. Select the branch to build from
5. Click "Run workflow"