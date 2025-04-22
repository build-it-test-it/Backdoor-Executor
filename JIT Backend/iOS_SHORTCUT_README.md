# JIT Enabler iOS Shortcut

This document explains how to use the JIT Enabler iOS Shortcut to enable Just-In-Time (JIT) compilation for your iOS apps without needing to create a custom app.

## How It Works

The JIT Enabler solution consists of two parts:

1. **JIT Backend**: A Flask server that handles device registration, authentication, and JIT enablement instructions
2. **iOS Shortcut**: A user-friendly iOS Shortcut that communicates with the backend to enable JIT for selected apps

When a user runs the shortcut on their iOS device, it:

1. Gets device information (name, model, iOS version)
2. Generates a unique device identifier
3. Registers with the JIT backend
4. Allows the user to select which app needs JIT
5. Requests JIT enablement from the backend
6. Applies the necessary memory permission changes
7. Launches the app with JIT enabled

## Setting Up the Backend

1. Deploy the JIT Backend to Render.com (see DEPLOYMENT.md for instructions)
2. Make note of your backend URL (e.g., `https://your-jit-backend.onrender.com`)

## Distributing the Shortcut

There are two ways to distribute the shortcut:

### Option 1: Web Interface

1. Users visit your backend URL in Safari on their iOS device
2. The web interface provides instructions and a download button
3. Users tap the download button to get the shortcut
4. They add the shortcut to their device and configure it with your backend URL

### Option 2: Direct iCloud Link

1. Download the shortcut JSON file from your backend
2. Import it into the Shortcuts app on your own iOS device
3. Share the shortcut to iCloud
4. Share the iCloud link with users

## Using the Shortcut

Users can enable JIT for their apps by:

1. Running the JIT Enabler shortcut
2. Selecting the app they want to enable JIT for
3. Waiting for the shortcut to communicate with the backend
4. Launching their app when prompted

## Technical Details

### How JIT Enablement Works

The iOS Shortcut approach leverages the same backend logic as a custom app would, but packages it in a more accessible format. The key steps are:

1. **Device Registration**: The shortcut registers the device with the backend to get an authentication token
2. **JIT Request**: When the user selects an app, the shortcut sends a request to the backend with the app's bundle ID
3. **JIT Instructions**: The backend returns instructions specific to the iOS version
4. **Memory Permission Toggle**: The shortcut applies these instructions to toggle memory permissions and enable JIT

### iOS Version Compatibility

The backend provides different JIT enablement strategies based on the iOS version:

- **iOS 15**: Uses legacy methods for enabling JIT
- **iOS 16**: Uses the CS_DEBUGGED flag approach
- **iOS 17+**: Uses memory permission toggling to comply with W^X security policy

### Security Considerations

- All communication between the shortcut and backend is encrypted using HTTPS
- The backend uses JWT tokens for authentication
- The shortcut stores the authentication token securely
- No sensitive data is collected or stored

## Limitations

While the iOS Shortcut approach is user-friendly, it has some limitations:

1. **Permissions**: The shortcut may not have all the permissions that a custom app would have
2. **Persistence**: JIT enablement may need to be reapplied when the app is restarted
3. **Compatibility**: Some apps may require additional steps beyond what the shortcut can provide

## Troubleshooting

If users encounter issues:

1. **JIT Not Working**: Make sure they're running the shortcut while the target app is already open in the background
2. **Connection Errors**: Verify their internet connection and the backend URL
3. **Permission Errors**: Some iOS versions have additional restrictions that may require a different approach

## Support

Provide users with a way to contact you for support, such as:

- Email address
- Twitter handle
- GitHub issues
- Discord server

## Legal Considerations

Make sure to include appropriate disclaimers about:

1. The educational nature of the tool
2. Compliance with Apple's terms of service
3. Privacy policy regarding data collection
4. Any limitations of liability

## Future Improvements

Consider these enhancements for future versions:

1. More detailed error reporting
2. Support for additional iOS versions
3. Enhanced app detection
4. Improved persistence of JIT enablement
5. Better user interface and instructions