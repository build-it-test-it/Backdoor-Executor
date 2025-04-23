# JIT Enabler for iOS

This repository contains the JIT Enabler iOS app and backend server for enabling Just-In-Time (JIT) compilation on iOS devices.

## Components

### iOS App

The iOS app is located in the root directory and consists of:

- `JITEnabler/` - The Swift source code for the iOS app
- `JITEnabler.xcodeproj/` - The Xcode project file
- `codemagic.yaml` - Configuration for CI/CD with Codemagic

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

### Building the iOS App

1. Open the Xcode project:
   ```
   open JITEnabler.xcodeproj
   ```

2. Configure the backend URL in the app settings
3. Build and run the app on your iOS device

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