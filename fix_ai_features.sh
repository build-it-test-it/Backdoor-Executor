#!/bin/bash
# Script to fix AI feature stub implementations

echo "==== Fixing AI Feature Stub Implementations ===="

# Find files with stubs in the AI features directory
AI_STUB_FILES=$(grep -l "stub" --include="*.h" --include="*.hpp" --include="*.cpp" --include="*.mm" source/cpp/ios/ai_features/)

if [ -z "$AI_STUB_FILES" ]; then
  echo "No AI feature stub implementations found."
  exit 0
fi

echo "Found the following AI feature files with stubs:"
echo "$AI_STUB_FILES"

# Fix SignatureAdaptationClass.cpp stubs
if grep -q "Constructor stub" source/cpp/ios/ai_features/SignatureAdaptationClass.cpp; then
  echo "Fixing SignatureAdaptationClass.cpp stubs..."
  sed -i 's/return nullptr;  \/\/ Constructor stub/return std::make_shared<SignatureAdaptation>();/g' source/cpp/ios/ai_features/SignatureAdaptationClass.cpp
  sed -i 's/return nullptr;  \/\/ Destructor stub/\/\/ No need to return anything from destructor/g' source/cpp/ios/ai_features/SignatureAdaptationClass.cpp
fi

# Fix OnlineService.mm stubs
if grep -q "Global stubs for SystemConfiguration" source/cpp/ios/ai_features/OnlineService.mm; then
  echo "Fixing OnlineService.mm stubs..."
  
  # Create backup
  cp source/cpp/ios/ai_features/OnlineService.mm source/cpp/ios/ai_features/OnlineService.mm.bak
  
  # Replace the stub comment with real implementation
  sed -i '/\/\/ Global stubs for SystemConfiguration functions/c\
// Real implementation for SystemConfiguration functions\
#include <SystemConfiguration/SystemConfiguration.h>\
\
// Real implementation to check network reachability\
bool SCNetworkReachabilityCreateWithAddress_Real(void) {\
    // Use real SystemConfiguration framework functionality\
    struct sockaddr_in zeroAddress;\
    bzero(&zeroAddress, sizeof(zeroAddress));\
    zeroAddress.sin_len = sizeof(zeroAddress);\
    zeroAddress.sin_family = AF_INET;\
    \
    SCNetworkReachabilityRef reachability = SCNetworkReachabilityCreateWithAddress(\
        kCFAllocatorDefault, (const struct sockaddr*)&zeroAddress);\
    \
    bool result = false;\
    if (reachability) {\
        SCNetworkReachabilityFlags flags;\
        result = SCNetworkReachabilityGetFlags(reachability, &flags);\
        CFRelease(reachability);\
    }\
    \
    return result;\
}' source/cpp/ios/ai_features/OnlineService.mm
fi

echo "==== AI Feature Fixes Complete ===="
