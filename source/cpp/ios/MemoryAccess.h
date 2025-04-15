// Memory access utilities for iOS
#pragma once

#include "../objc_isolation.h"
#include <cstdint>
#include <vector>

// Include platform-specific headers
#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/mach_interface.h>
#include <mach/task.h>

// We'll use the system-defined mach_vm types instead of defining our own

#else
// Include mach_compat.h for non-Apple platforms
#include "mach_compat.h"
#endif // __APPLE__

namespace iOS {
    class MemoryAccess {
    public:
        // Read memory from a process
        static bool ReadMemory(void* address, void* buffer, size_t size);
        
        // Write memory to a process
        static bool WriteMemory(void* address, const void* buffer, size_t size);
        
        // Change memory protection
        static bool SetMemoryProtection(void* address, size_t size, int protection);
        
        // Allocate memory
        static void* AllocateMemory(size_t size);
        
        // Free memory
        static bool FreeMemory(void* address, size_t size);
        
        // Find memory region
        static void* FindMemoryRegion(const char* pattern, size_t size, void* startAddress = nullptr, void* endAddress = nullptr);
        
        // Initialize memory subsystem
        static bool Initialize();
        
        // Template methods for convenience
        template<typename T>
        static bool ReadValue(void* address, T& value) {
            return ReadMemory(address, &value, sizeof(T));
        }
        
        template<typename T>
        static bool WriteValue(void* address, const T& value) {
            return WriteMemory(address, &value, sizeof(T));
        }
    };
    
    // Helper functions for type safety
    namespace MemoryHelper {
        // Convert between void* and mach_vm_address_t
        inline void* AddressToPtr(mach_vm_address_t addr) {
            return reinterpret_cast<void*>(static_cast<uintptr_t>(addr));
        }
        
        inline mach_vm_address_t PtrToAddress(void* ptr) {
            return static_cast<mach_vm_address_t>(reinterpret_cast<uintptr_t>(ptr));
        }
    }
}
