# JIT Enabler Shortcut (Jellycuts Version)

This document explains how to use the Jellycuts version of the JIT Enabler shortcut to enable Just-In-Time (JIT) compilation for iOS apps.

## What is Jellycuts?

Jellycuts is a programming language and development environment that allows you to create iOS Shortcuts using code instead of the visual editor. This makes it easier to share, modify, and understand complex shortcuts.

## Why Use Jellycuts for JIT Enabler?

1. **No Signing Required**: The Jellycuts version doesn't require signing, making it easier to distribute
2. **Easier to Customize**: Users can modify the shortcut to fit their specific needs
3. **Better Readability**: The code-based format is easier to understand than the JSON format
4. **Version Control**: Changes can be tracked more effectively

## Installation Instructions

### Step 1: Install Jellycuts

1. Download Jellycuts from the App Store: [Jellycuts on the App Store](https://apps.apple.com/us/app/jellycuts/id1522625245)
2. Open the Jellycuts app and complete the initial setup

### Step 2: Import the JIT Enabler Shortcut

1. Download the `JIT_Enabler.jellycuts` file from our website
2. Open the Jellycuts app
3. Tap the "+" button to create a new shortcut
4. Select "Import" and choose the downloaded file
5. Review the code (you can make any customizations if needed)
6. Tap "Build" to compile the shortcut

### Step 3: Configure the Shortcut

When building the shortcut, you'll be prompted to enter:
- The URL of your JIT Backend (e.g., `https://your-jit-backend.onrender.com`)

### Step 4: Add to Your Shortcuts Library

After building, Jellycuts will prompt you to add the shortcut to your Shortcuts library. Tap "Add Shortcut" to complete the installation.

## Using the JIT Enabler Shortcut

1. Run the JIT Enabler shortcut from the Shortcuts app
2. The first time you run it, the shortcut will register your device with the JIT backend
3. Select the app category (Emulators, JavaScript Apps, or Other Apps)
4. Choose a specific app or enter a bundle ID
5. The shortcut will communicate with the backend to enable JIT
6. Once JIT is enabled, the app will launch automatically

## Troubleshooting

### Common Issues

1. **Connection Error**: If you see "Network error", check your internet connection and make sure the backend URL is correct
2. **Authentication Error**: If you see "Device not registered", try deleting the shortcut and reinstalling it
3. **JIT Enablement Failure**: If JIT enablement fails, check that you're using the correct bundle ID and that your iOS version is supported

### Resetting the Shortcut

If you need to reset the shortcut:
1. Open the Shortcuts app
2. Long-press on the JIT Enabler shortcut
3. Select "Delete Shortcut"
4. Reinstall using the steps above

## Advanced Customization

The Jellycuts format makes it easy to customize the shortcut. Here are some common customizations:

### Adding More Apps

To add more predefined apps, find the appropriate section in the code and add your app:

```javascript
// Handle Your App
If({
    input: GetVariable("Chosen Item"),
    condition: "is",
    value: "Your App Name",
    then: [
        Text("com.example.yourapp"),
        SetVariable("bundleID")
    ]
})
```

### Changing Backend URL

If you need to change the backend URL after installation:
1. Open the Shortcuts app
2. Edit the JIT Enabler shortcut
3. Find the "Text" action at the beginning
4. Change the URL to your new backend

## Security Considerations

- The shortcut generates a pseudo-UDID based on your device characteristics
- All communication with the backend is encrypted using HTTPS
- Your device token is stored securely on your device
- No sensitive information is shared with third parties

## Technical Details

The Jellycuts shortcut performs the following operations:

1. **Device Registration**:
   - Collects device information (name, model, iOS version)
   - Generates a pseudo-UDID using a hash function
   - Registers with the backend server
   - Stores the authentication token

2. **App Selection**:
   - Provides categorized lists of common apps
   - Allows manual entry of bundle IDs
   - Maps friendly names to bundle IDs

3. **JIT Enablement**:
   - Sends the bundle ID to the backend
   - Receives JIT enablement instructions
   - Processes the response
   - Launches the target app

4. **Error Handling**:
   - Detects and reports network errors
   - Handles authentication failures
   - Provides user-friendly error messages

## Compatibility

- **iOS Versions**: Compatible with iOS 15, 16, and 17
- **Device Types**: Works on iPhone and iPad
- **App Types**: Compatible with emulators, JavaScript-based apps, and other apps that use JIT compilation

## Support

If you encounter any issues with the JIT Enabler shortcut, please:
1. Check the troubleshooting section above
2. Visit our website for updated documentation
3. Contact support with details about your device and the specific error