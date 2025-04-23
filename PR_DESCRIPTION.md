# Fix iOS App Launch Issue

## Problem
The iOS app was failing to launch after being built and packaged into an IPA file. This was happening because the build process was overwriting the original Info.plist file with a simplified version that was missing critical keys required for the app to function properly.

## Changes Made

1. **Fixed build_ipa.sh script**:
   - Prevented the script from completely overwriting the Info.plist file
   - Modified it to only update specific keys (bundle ID, version) while preserving all other entries
   - Added proper handling of the entitlements file

2. **Updated Makefile.ios**:
   - Fixed the IPA creation process to preserve the original Info.plist structure
   - Added proper inclusion of the entitlements file
   - Improved the build configuration to properly handle app requirements

3. **Updated Info.plist**:
   - Changed UIRequiredDeviceCapabilities from "armv7" to "arm64" to match modern iOS devices

## Testing
After these changes, the app should successfully launch when installed from the built IPA file. The changes ensure that all necessary app configuration is preserved during the build process.

## Technical Details
The root cause was that the build script was replacing the entire Info.plist file with a minimal version that only contained a few keys. This stripped out critical configuration that iOS requires for app launching. The fix ensures we preserve the original Info.plist structure while only updating specific values that need to be customized during the build.