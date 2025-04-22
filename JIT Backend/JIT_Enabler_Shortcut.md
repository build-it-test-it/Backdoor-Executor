# JIT Enabler iOS Shortcut

This document provides instructions for installing and using the JIT Enabler iOS Shortcut, which allows you to enable Just-In-Time (JIT) compilation for your iOS apps without needing to create a custom app.

## Prerequisites

- iOS device running iOS 15 or later
- Shortcuts app installed (comes pre-installed on iOS)
- The app you want to enable JIT for must be installed on your device

## Installation Instructions

1. **Download the Shortcut**:
   - Open this link on your iOS device: [JIT Enabler Shortcut](https://www.icloud.com/shortcuts/YOUR_SHORTCUT_LINK)
   - If you can't access the link, scan the QR code below:
   
   [QR CODE PLACEHOLDER]

2. **Add the Shortcut**:
   - Tap "Add Shortcut" when prompted
   - If you see a security warning, tap "Add Untrusted Shortcut"

3. **Configure the Shortcut**:
   - When you first run the shortcut, you'll be asked to enter the JIT Backend URL
   - Enter: `https://your-jit-backend-url.onrender.com` (replace with your actual backend URL)
   - This only needs to be done once

## Using the Shortcut

1. **Run the Shortcut**:
   - Open the Shortcuts app
   - Tap on the "JIT Enabler" shortcut
   - Alternatively, you can add it to your home screen for easier access

2. **Device Registration**:
   - The first time you run the shortcut, it will register your device with the JIT backend
   - This only needs to be done once

3. **Select an App**:
   - Choose the app you want to enable JIT for from the list of installed apps
   - The shortcut will show apps that might benefit from JIT (like emulators, interpreters, etc.)

4. **Enable JIT**:
   - The shortcut will communicate with the JIT backend to enable JIT for the selected app
   - Follow any on-screen instructions
   - Once complete, you'll see a success message

5. **Launch the App**:
   - Launch your app immediately after enabling JIT
   - JIT compilation should now be working

## Troubleshooting

If you encounter issues:

1. **JIT Not Working**:
   - Make sure you're running the shortcut while the target app is already open in the background
   - Try restarting the app after enabling JIT
   - Check that you're using the correct backend URL

2. **Connection Errors**:
   - Verify your internet connection
   - Ensure the backend URL is correct
   - The backend server might be down - try again later

3. **Permission Errors**:
   - Some iOS versions have additional restrictions
   - Make sure you're allowing all permissions requested by the shortcut

## How It Works

The JIT Enabler Shortcut works by:

1. Getting a unique identifier for your device
2. Registering with our secure JIT backend
3. Requesting JIT enablement for your selected app
4. Applying the necessary memory permission changes to enable JIT
5. All communication is encrypted and secure

## Privacy and Security

- The shortcut only collects the minimum information needed to enable JIT
- All communication with the backend is encrypted using HTTPS
- Your device information is only used for JIT enablement and is not shared with third parties
- The shortcut does not have access to your personal data

## Shortcut Details

Here's what the shortcut does step by step:

```
1. Get Device Information
   - Device Name
   - iOS Version
   - Device Model
   - Generate a device identifier

2. Register Device (if first run)
   - Send device info to backend
   - Store authentication token securely

3. Show App Selection
   - Display list of installed apps
   - User selects target app

4. Request JIT Enablement
   - Send app bundle ID to backend
   - Receive JIT enablement instructions

5. Apply JIT Settings
   - Execute the instructions from the backend
   - Toggle memory permissions as needed

6. Show Result
   - Display success or error message
   - Provide next steps
```

## Creating the Shortcut Manually

If you prefer to create the shortcut manually, follow these steps:

1. Open the Shortcuts app
2. Tap the "+" button to create a new shortcut
3. Add the following actions in sequence:
   - Text: [JIT Backend URL]
   - Set Variable: "backendURL"
   - Get Device Details
   - Set Variable: "deviceInfo"
   - If: Shortcut Input is empty
   - Show List (installed apps)
   - Set Variable: "selectedApp"
   - Get Contents of URL: [Register/Enable JIT endpoint]
   - If: Result contains "success"
   - Show Result
   - Otherwise
   - Show Error
   - End If
   - End If

## Support

If you need help with the JIT Enabler Shortcut, please contact us at:
- Email: support@example.com
- Twitter: @JITEnablerSupport