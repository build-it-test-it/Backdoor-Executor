#pragma once

#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include <iostream>

// Platform-specific includes
#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <mach/mach.h>
#endif

// Type definitions
typedef unsigned long DWORD;

namespace Memory {
    // Platform-specific utility functions
    
    // Get base address of a library
    static uintptr_t GetLibraryBase(const char* libraryName) {
#ifdef __APPLE__
        // iOS implementation using dyld APIs
        uint32_t imageCount = _dyld_image_count();
        for (uint32_t i = 0; i < imageCount; i++) {
            const char* imageName = _dyld_get_image_name(i);
            if (strstr(imageName, libraryName)) {
                return (uintptr_t)_dyld_get_image_header(i);
            }
        }
        return 0;
#else
        // Android/Linux implementation using /proc/self/maps
        char filename[255] = {0};
        char buffer[1024] = {0};
        FILE* fp = NULL;
        uintptr_t address = 0;

        snprintf(filename, sizeof(filename), "/proc/self/maps");

        fp = fopen(filename, "rt");
        if (fp == NULL) {
            perror("fopen");
            return 0;
        }

        while (fgets(buffer, sizeof(buffer), fp)) {
            if (strstr(buffer, libraryName)) {
                address = (uintptr_t)strtoul(buffer, NULL, 16);
                break;
            }
        }

        if (fp) {
            fclose(fp);
        }

        return address;
#endif
    }

    // Get address of a function within a library
    static uintptr_t GetAddress(const char* libraryName, uintptr_t relativeAddr) {
        uintptr_t libBase = GetLibraryBase(libraryName);
        if (libBase == 0) {
            return 0;
        }
        return libBase + relativeAddr;
    }

    // Get address with default library
    static uintptr_t GetAddress(uintptr_t relativeAddr) {
#ifdef __APPLE__
        return GetAddress("libroblox.dylib", relativeAddr);
#else
        return GetAddress("libroblox.so", relativeAddr);
#endif
    }

    // Check if a library is loaded
    static bool IsLibraryLoaded(const char* libraryName) {
#ifdef __APPLE__
        // iOS implementation using dyld APIs
        uint32_t imageCount = _dyld_image_count();
        for (uint32_t i = 0; i < imageCount; i++) {
            const char* imageName = _dyld_get_image_name(i);
            if (strstr(imageName, libraryName)) {
                return true;
            }
        }
        return false;
#else
        // Android/Linux implementation using /proc/self/maps
        char line[512] = {0};
        FILE* fp = fopen("/proc/self/maps", "rt");
        if (fp != NULL) {
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, libraryName)) {
                    fclose(fp);
                    return true;
                }
            }
            fclose(fp);
        }
        return false;
#endif
    }

    // Check if the game library is loaded
    static bool IsGameLibLoaded() {
#ifdef __APPLE__
        return IsLibraryLoaded("libroblox.dylib");
#else
        return IsLibraryLoaded("libroblox.so");
#endif
    }

    // Memory read/write functions
    template<typename T>
    static T Read(uintptr_t address) {
        if (address == 0) {
            return T();
        }
        return *reinterpret_cast<T*>(address);
    }

    template<typename T>
    static void Write(uintptr_t address, T value) {
        if (address == 0) {
            return;
        }
        *reinterpret_cast<T*>(address) = value;
    }

    // Memory protection manipulation
    static bool ProtectMemory(void* address, size_t size, int protection) {
#ifdef __APPLE__
        // iOS memory protection
        vm_prot_t prot = 0;
        if (protection & 1) prot |= VM_PROT_READ;
        if (protection & 2) prot |= VM_PROT_WRITE;
        if (protection & 4) prot |= VM_PROT_EXECUTE;
        
        kern_return_t result = vm_protect(mach_task_self(), (vm_address_t)address, size, FALSE, prot);
        return result == KERN_SUCCESS;
#else
        // Implement for other platforms as needed
        return false;
#endif
    }

    // Initialize memory system
    static void Initialize() {
        std::cout << "Initializing memory system" << std::endl;
    }
}
