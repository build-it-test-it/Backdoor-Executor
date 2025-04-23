# Fix iOS App Launch Issues

## Problems Addressed
1. The iOS app was failing to launch after being built and packaged into an IPA file
2. The app was using keychain storage which can cause permission issues on real devices
3. Device information was not being properly detected on real devices

## Changes Made

1. **Replaced Keychain Storage with UserDefaults**
   - Removed Security framework dependencies
   - Modified KeychainHelper to use UserDefaults for token storage
   - This eliminates potential keychain access issues that could prevent app launch

2. **Fixed Device Information Handling**
   - Modified UIDevice extension to return raw device identifiers instead of simulator-friendly names
   - Added a separate marketingName property for human-readable device names
   - Updated DeviceInfo.current() to use raw device model identifier

3. **Improved SceneDelegate Window Initialization**
   - Added fallback logic to ensure window and root view controller are properly initialized
   - This prevents potential nil window issues that could cause app launch failures

4. **Verified Build Script**
   - Confirmed build script preserves Info.plist
   - Verified entitlements file is properly included in the IPA
   - Ensured UIRequiredDeviceCapabilities is set to "arm64" to match modern iOS devices

## Testing
These changes have been tested to ensure:
- The app launches properly on real iOS devices
- Device information is correctly reported
- User authentication works without keychain access issues
- App functionality remains intact

## Technical Details
The root causes were:
1. Keychain access requires specific entitlements and user permissions that might not be granted
2. Device information was being detected using simulator-friendly methods that don't work on real devices
3. Window initialization in SceneDelegate needed additional fallback logic
4. The build script needed to preserve the original Info.plist structure