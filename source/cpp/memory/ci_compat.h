#pragma once

#include <cstddef> // For size_t
#include <sys/mman.h> // For mprotect on iOS

/**
 * iOS Memory Compatibility Header
 * 
 * This file provides essential memory-related utilities and platform
 * detection for iOS devices. All features are fully enabled for
 * production deployment, with no CI-specific limitations.
 */

// Platform detection - focused on iOS
#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        #define PLATFORM_IOS
    #elif TARGET_OS_MAC
        #define PLATFORM_MACOS
    #endif
#endif

// Always enable all features for production use
#define ENABLE_MEMORY_SCANNING
#define ENABLE_HOOKS
#define ENABLE_JIT

// iOS-specific constants for memory operations
#ifdef PLATFORM_IOS
    // Memory protection flags that match iOS mach-o conventions
    #define MEM_PROT_NONE  PROT_NONE
    #define MEM_PROT_READ  PROT_READ
    #define MEM_PROT_WRITE PROT_WRITE
    #define MEM_PROT_EXEC  PROT_EXEC
    #define MEM_PROT_RW    (PROT_READ | PROT_WRITE)
    #define MEM_PROT_RX    (PROT_READ | PROT_EXEC)
    #define MEM_PROT_RWX   (PROT_READ | PROT_WRITE | PROT_EXEC)
    
    // Page size constant for memory alignment
    #ifdef __arm64__
        #define MEMORY_PAGE_SIZE 16384  // 16KB for arm64
    #else
        #define MEMORY_PAGE_SIZE 4096   // 4KB for others
    #endif
#endif

// Memory protection utilities - always enabled with full functionality
#ifdef PLATFORM_IOS
    // Memory protection using mach vm_protect for iOS
    inline bool MEMORY_PROTECT(void* addr, size_t size, int prot) {
        // Need to align to page boundaries for iOS
        uintptr_t pageStart = (uintptr_t)addr & ~(MEMORY_PAGE_SIZE - 1);
        size_t pageAlignedSize = ((uintptr_t)addr + size + MEMORY_PAGE_SIZE - 1) & ~(MEMORY_PAGE_SIZE - 1);
        pageAlignedSize -= pageStart;
        
        // Use mprotect on iOS
        return mprotect((void*)pageStart, pageAlignedSize, prot) == 0;
    }
    
    // Memory unprotection to make memory writable on iOS
    inline bool MEMORY_UNPROTECT(void* addr, size_t size) {
        // Make memory RWX on iOS
        return MEMORY_PROTECT(addr, size, MEM_PROT_RWX);
    }
    
    // Function to calculate checksum for memory integrity verification
    inline uint32_t MEMORY_CHECKSUM(const void* data, size_t size) {
        if (!data || size == 0) return 0;
        
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        uint32_t checksum = 0;
        
        for (size_t i = 0; i < size; i++) {
            checksum = ((checksum << 5) + checksum) + bytes[i]; // djb2 algorithm
        }
        
        return checksum;
    }
#endif
