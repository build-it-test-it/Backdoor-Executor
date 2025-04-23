# iOS JIT Enabler

A native iOS application that enables Just-In-Time (JIT) compilation for iOS apps without modifying their code. This app communicates with a secure backend to enable JIT functionality for compatible apps.

## Features

- Enable JIT compilation for compatible iOS apps
- Support for iOS 15, 16, and 17+
- Secure communication with backend server
- Session history tracking
- Device registration and management
- User-friendly interface

## Requirements

- iOS 15.0+
- Xcode 14.0+
- Swift 5.0+

## Installation

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/iOS-JIT-Enabler.git
   cd iOS-JIT-Enabler
   ```

2. Open the project in Xcode:
   ```bash
   open iOS-JIT-Enabler.xcodeproj
   ```

3. Configure signing:
   - Select the project in the Project Navigator
   - Select the "iOS-JIT-Enabler" target
   - Go to the "Signing & Capabilities" tab
   - Select your team and configure signing

4. Build and run the app on your device

### Using Codemagic CI/CD

This project includes a `codemagic.yaml` file for building with [Codemagic](https://codemagic.io/):

1. Set up a Codemagic account and connect your repository
2. Configure the environment variables in the Codemagic dashboard
3. Start a build using the `ios-native-workflow` workflow

## Usage

1. Launch the app on your iOS device
2. The app will register your device with the backend server
3. Select "Enable JIT for App" to see a list of compatible apps
4. Select an app to enable JIT compilation
5. The app will communicate with the backend to enable JIT
6. Once JIT is enabled, the target app will be launched automatically

## How It Works

The iOS JIT Enabler app uses different techniques based on your iOS version:

- **iOS 15**: Uses a combination of debugging entitlements and memory page permission toggling
- **iOS 16**: Leverages specific memory management techniques to enable JIT
- **iOS 17+**: Uses the latest methods to comply with iOS's W^X (Write XOR Execute) security policy

All communication with the backend is secured using HTTPS and token-based authentication.

## Backend Configuration

The app requires a backend server to function. By default, it will use the server URL configured during first launch. You can change this in the Settings screen.

## Security

- All communication with the backend is encrypted using HTTPS
- Authentication tokens are securely stored in the device keychain
- No sensitive data is logged or stored in plain text
- The app does not require jailbreaking or other system modifications

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgements

- Thanks to the JIT research community for their work on iOS JIT enablement techniques
- Special thanks to contributors who have helped improve this project