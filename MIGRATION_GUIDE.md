# Migration Guide: CodeMagic to GitHub Actions

This guide explains how to migrate from CodeMagic to GitHub Actions for building and deploying the JIT Enabler iOS app.

## Why Migrate?

If you're experiencing issues with CodeMagic builds, such as the "Invalid encryption key" error, migrating to GitHub Actions can provide:
- Better integration with your GitHub repository
- More control over the build process
- Potentially lower costs
- Elimination of team-specific encryption key issues

## Migration Steps

### 1. Set Up GitHub Secrets

First, you need to set up the necessary secrets in your GitHub repository:

#### From CodeMagic to GitHub Secrets

| CodeMagic Variable | GitHub Secret |
|-------------------|---------------|
| APP_STORE_CONNECT_ISSUER_ID | APP_STORE_CONNECT_ISSUER_ID |
| APP_STORE_CONNECT_KEY_IDENTIFIER | APP_STORE_CONNECT_KEY_IDENTIFIER |
| APP_STORE_CONNECT_PRIVATE_KEY | APP_STORE_CONNECT_PRIVATE_KEY |
| DROPBOX_APP_KEY | DROPBOX_APP_KEY |
| DROPBOX_APP_SECRET | DROPBOX_APP_SECRET |
| DROPBOX_REFRESH_TOKEN | DROPBOX_REFRESH_TOKEN |

#### Additional Required Secrets

You'll also need to add these secrets that were handled differently in CodeMagic:

- `IOS_DISTRIBUTION_P12`: Base64-encoded P12 certificate file
- `IOS_DISTRIBUTION_P12_PASSWORD`: Password for the P12 certificate
- `IOS_DISTRIBUTION_CERTIFICATE_NAME`: Name of the distribution certificate
- `IOS_PROVISIONING_PROFILE`: Base64-encoded mobileprovision file
- `IOS_PROVISIONING_PROFILE_SPECIFIER`: Provisioning profile specifier name
- `APPLE_TEAM_ID`: Your Apple Developer Team ID

### 2. Prepare Your Certificates and Profiles

1. Export your distribution certificate as a P12 file:
   - Open Keychain Access
   - Find your iOS Distribution certificate
   - Right-click and select "Export"
   - Save as a P12 file and set a password

2. Base64 encode your P12 file:
   ```bash
   base64 -i YourCertificate.p12 | pbcopy
   ```

3. Get your provisioning profile:
   - Download it from Apple Developer Portal
   - Or find it in `~/Library/MobileDevice/Provisioning Profiles/`

4. Base64 encode your provisioning profile:
   ```bash
   base64 -i YourProfile.mobileprovision | pbcopy
   ```

### 3. Add the GitHub Workflow File

The GitHub Actions workflow file (`.github/workflows/ios-app-build.yml`) has been created for you. It includes:

- Setting up the macOS build environment
- Installing your certificates and provisioning profiles
- Building the app
- Creating an IPA file
- Uploading to TestFlight

### 4. Remove CodeMagic Configuration

You can now remove or disable the CodeMagic configuration:

1. Either delete the `codemagic.yaml` file
2. Or keep it for reference but disable the workflow in the CodeMagic dashboard

### 5. Trigger Your First Build

1. Push your changes to GitHub
2. Go to the Actions tab in your repository
3. You should see the "Build and Deploy iOS App" workflow running
4. Check the logs for any issues

## Troubleshooting

### Certificate and Provisioning Profile Issues

If you encounter code signing issues:
- Verify that your P12 file is correctly exported and encoded
- Check that your provisioning profile is valid and matches your bundle identifier
- Ensure your team ID is correct

### TestFlight Upload Issues

If the app builds but fails to upload to TestFlight:
- Verify your App Store Connect API credentials
- Check that your app's bundle ID is registered in App Store Connect
- Ensure your provisioning profile has the correct entitlements

### Build Failures

If the build fails:
- Check the GitHub Actions logs for specific error messages
- Verify that your Xcode project builds locally without issues
- Ensure all dependencies are correctly specified

## Need Help?

If you encounter issues during migration:
1. Check the GitHub Actions logs for detailed error messages
2. Review Apple's documentation on code signing and TestFlight uploads
3. Consider consulting with an iOS CI/CD specialist if problems persist