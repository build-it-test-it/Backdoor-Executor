// include_guard.h - Central include file for all platform-specific headers
#pragma once

// Include the objc_isolation header with the correct path based on the build system
// This allows all files to include "include_guard.h" instead of having relative paths
#include "objc_isolation.h"
#include "ios_compat.h"
