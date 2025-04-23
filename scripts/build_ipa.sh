#!/bin/bash
# Script to build and package iOS app into IPA

set -e

# Configuration
XCODE_PROJECT="JITEnabler.xcodeproj"
XCODE_SCHEME="JITEnabler"
BUILD_DIR="build"
IPA_DIR="${BUILD_DIR}/ios/ipa"
DERIVED_DATA_PATH="${BUILD_DIR}/DerivedData"

# Create directories
mkdir -p "${BUILD_DIR}"
mkdir -p "${IPA_DIR}"
mkdir -p "${DERIVED_DATA_PATH}"

echo "=== Setting up environment ==="
echo "DROPBOX_APP_KEY=2bi422xpd3xd962" > .env
echo "DROPBOX_APP_SECRET=j3yx0b41qdvfu86" >> .env
echo "DROPBOX_REFRESH_TOKEN=RvyL03RE5qAAAAAAAAAAAVMVebvE7jDx8Okd0ploMzr85c6txvCRXpJAt30mxrKF" >> .env

echo "=== Building for iOS simulator ==="
xcodebuild clean build \
    -project "${XCODE_PROJECT}" \
    -scheme "${XCODE_SCHEME}" \
    -configuration Debug \
    -sdk iphonesimulator \
    -derivedDataPath "${DERIVED_DATA_PATH}" \
    -destination 'platform=iOS Simulator,name=iPhone 14,OS=latest' \
    CODE_SIGN_IDENTITY="" \
    CODE_SIGNING_REQUIRED=NO \
    CODE_SIGNING_ALLOWED=NO

# Find the app file
APP_PATH=$(find "${DERIVED_DATA_PATH}/Build/Products" -name "*.app" -type d | head -1)

if [ -z "${APP_PATH}" ]; then
    echo "Error: Could not find .app file in build products"
    exit 1
fi

echo "Found app at: ${APP_PATH}"

# Create Payload directory and copy app
echo "=== Creating IPA package ==="
mkdir -p "${IPA_DIR}/Payload"
cp -R "${APP_PATH}" "${IPA_DIR}/Payload/"

# Add Info.plist with required metadata
cat > "${IPA_DIR}/Payload/$(basename "${APP_PATH}")/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleIdentifier</key>
    <string>com.jitenabler.app</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>MinimumOSVersion</key>
    <string>15.0</string>
</dict>
</plist>
EOF

# Create the IPA file
cd "${IPA_DIR}" && zip -r JITEnabler.ipa Payload && rm -rf Payload

echo "=== Build completed successfully ==="
echo "IPA file created at: ${IPA_DIR}/JITEnabler.ipa"

# Verify the IPA file
if [ -f "${IPA_DIR}/JITEnabler.ipa" ]; then
    echo "=== IPA file details ==="
    ls -la "${IPA_DIR}/JITEnabler.ipa"
    echo "Size: $(du -h "${IPA_DIR}/JITEnabler.ipa" | cut -f1)"
else
    echo "Error: IPA file was not created"
    exit 1
fi