// FileSystem compatibility header
// This redirects to the renamed IOSFileSystem to avoid conflicts with std::filesystem
#pragma once

#include "IOSFileSystem.h"

// FileSystem is now a type alias for IOSFileSystem in the iOS namespace
// This is defined in IOSFileSystem.h: using FileSystem = IOSFileSystem;
