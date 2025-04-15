// Special compatibility file to prevent namespace conflicts with std::filesystem
#pragma once

// Ensure we don't include std::filesystem directly
#ifndef IOS_AVOID_STD_FILESYSTEM
#define IOS_AVOID_STD_FILESYSTEM
#endif

// Include what we need
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <ctime>
