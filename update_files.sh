#!/bin/bash

# Add iOS compat header at the top of UIController.cpp
sed -i '1i#include "ios_compat.h"\n' source/cpp/ios/UIController.cpp

# Remove any existing #import statements and replace with conditional inclusion
sed -i '/#import <UIKit\/UIKit.h>/d' source/cpp/ios/UIController.cpp
sed -i '/#import <Foundation\/Foundation.h>/d' source/cpp/ios/UIController.cpp
sed -i '/#import <objc\/runtime.h>/d' source/cpp/ios/UIController.cpp

# Update the CI_BUILD guards to use our new macros
sed -i 's/#ifndef CI_BUILD/IOS_CODE(/g' source/cpp/ios/UIController.cpp
sed -i 's/#endif/)/g' source/cpp/ios/UIController.cpp

# Update the cmake file to include our compatibility headers
echo "include_directories(\${CMAKE_BINARY_DIR}/ios_compat)" >> CMakeLists.txt
