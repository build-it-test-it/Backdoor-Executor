# Shortcut Signing with GitHub Actions

This document explains how to use the GitHub Actions workflow to automatically sign your iOS Shortcuts and make them available for download.

## How It Works

The GitHub Actions workflow automatically:

1. Signs the JIT Enabler Shortcut when changes are pushed to the main branch
2. Creates a GitHub Release with the signed shortcut
3. Generates a QR code for easy installation
4. Updates a GitHub Pages site with download links

## Triggering the Workflow

The workflow can be triggered in two ways:

1. **Automatically**: When changes are pushed to the `JIT_Enabler_Shortcut.json` file in the main branch
2. **Manually**: By running the workflow from the GitHub Actions tab with custom parameters

### Manual Trigger

To manually trigger the workflow:

1. Go to the GitHub repository
2. Click on the "Actions" tab
3. Select the "Sign iOS Shortcut" workflow
4. Click "Run workflow"
5. Enter your backend URL (e.g., `https://your-jit-backend.onrender.com`)
6. Click "Run workflow"

## Customizing the Backend URL

When manually triggering the workflow, you can specify the backend URL to use in the shortcut. This is useful for testing with different backend environments.

## Installation Options

The workflow creates multiple ways for users to install the shortcut:

1. **Direct Download**: Users can download the signed shortcut file directly from the GitHub Release
2. **QR Code**: Users can scan a QR code with their iOS device to install the shortcut
3. **Web Interface**: Users can visit the GitHub Pages site for a user-friendly installation experience

## Shortcut Signing Process

The signing process involves:

1. Preparing the shortcut by updating the backend URL
2. Converting the shortcut JSON to a binary plist format
3. Signing the shortcut (note: this is a mock implementation)
4. Creating a QR code for the shortcut download URL

## Limitations

The current implementation uses a mock signing process since we don't have access to Apple's signing keys. In a production environment, you would need to use a proper signing service or implement a more sophisticated signing mechanism.

## Troubleshooting

If you encounter issues with the workflow:

1. Check the workflow logs in the GitHub Actions tab
2. Verify that the `JIT_Enabler_Shortcut.json` file is valid
3. Ensure that the repository has the necessary permissions for GitHub Actions and GitHub Pages

## Security Considerations

- The workflow uses GitHub's built-in secrets management to handle sensitive information
- The signed shortcut is distributed via HTTPS to ensure secure downloads
- Users should always verify that they trust the source before installing shortcuts

## Future Improvements

- Implement a more robust signing mechanism
- Add support for signing multiple shortcuts
- Improve the installation experience with progressive web app features
- Add analytics to track shortcut installations (with user consent)