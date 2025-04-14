#pragma once

#include <mach/mach.h>
// mach_vm.h is not supported on iOS, use alternative headers
#if !defined(IOS_TARGET) && !defined(__APPLE__)
#include <mach/mach_vm.h>
#else
// Add additional headers needed for iOS compatibility
#include <mach/vm_types.h>
#include <mach/vm_prot.h>
#include <mach/vm_map.h>
#include <mach/vm_region.h>
#endif

#include <mach/vm_map.h>
#include <mach-o/dyld.h>
#include <vector>
#include <string>
#include <cstdint>
#include <sys/types.h>
#include <mutex>
#include <atomic>
#include <unordered_map>

// Define iOS-compatible replacements for mach_vm functions
#if defined(IOS_TARGET) || defined(__APPLE__)
// Use vm_read/write instead of mach_vm functions on iOS
inline kern_return_t ios_vm_read(vm_map_t target_task, vm_address_t address, vm_size_t size, vm_offset_t *data, mach_msg_type_number_t *dataCnt) {
    return vm_read(target_task, address, size, data, dataCnt);
}

inline kern_return_t ios_vm_write(vm_map_t target_task, vm_address_t address, vm_offset_t data, mach_msg_type_number_t dataCnt) {
    return vm_write(target_task, address, data, dataCnt);
}

inline kern_return_t ios_vm_protect(vm_map_t target_task, vm_address_t address, vm_size_t size, boolean_t set_maximum, vm_prot_t new_protection) {
    return vm_protect(target_task, address, size, set_maximum, new_protection);
}

// Define compatibility macros to replace mach_vm functions
#define mach_vm_read ios_vm_read
#define mach_vm_write ios_vm_write
#define mach_vm_protect ios_vm_protect
#endif

namespace iOS {
    /**
     * @class MemoryAccess
     * @brief Provides platform-specific memory access utilities for iOS
     * 
     * This class handles all memory-related operations for iOS, including reading/writing
     * process memory, finding modules, and scanning for patterns. It uses Mach kernel APIs
     * for all operations to ensure compatibility with iOS devices.
     * 
     * Thread-safe implementation with caching for improved performance.
     */
    class MemoryAccess {
    private:
        // Private member variables with consistent m_ prefix
        static mach_port_t m_targetTask;
        static std::atomic<bool> m_initialized;
        static std::mutex m_accessMutex;   // Mutex for memory operations
        static std::mutex m_cacheMutex;    // Mutex for cache access
        
        // Caches for improved performance
        static std::unordered_map<std::string, mach_vm_address_t> m_patternCache;
        static std::unordered_map<std::string, mach_vm_address_t> m_moduleBaseCache;
        static std::unordered_map<mach_vm_address_t, size_t> m_moduleSizeCache;
        
        // Cached memory regions for faster scanning
        static std::vector<std::pair<mach_vm_address_t, mach_vm_address_t>> m_cachedReadableRegions;
        static uint64_t m_regionsLastUpdated;
        
        /**
         * @brief Refresh the cached memory regions
         */
        static void RefreshMemoryRegions();
        
        /**
         * @brief Get current timestamp in milliseconds
         * @return Current timestamp
         */
        static uint64_t GetCurrentTimestamp();
        
        /**
         * @brief Check if address is valid and readable
         * @param address Address to check
         * @param size Size of memory region to validate
         * @return True if address is valid, false otherwise
         */
        static bool IsAddressValid(mach_vm_address_t address, size_t size);
        
    public:
        /**
         * @brief Initialize memory access to the target process
         * @return True if initialization succeeded, false otherwise
         */
        static bool Initialize();
        
        /**
         * @brief Read memory from target process
         * @param address Memory address to read from
         * @param buffer Buffer to store read data
         * @param size Number of bytes to read
         * @return True if read succeeded, false otherwise
         */
        static bool ReadMemory(mach_vm_address_t address, void* buffer, size_t size);
        
        /**
         * @brief Write memory to target process
         * @param address Memory address to write to
         * @param buffer Data buffer to write
         * @param size Number of bytes to write
         * @return True if write succeeded, false otherwise
         */
        static bool WriteMemory(mach_vm_address_t address, const void* buffer, size_t size);
        
        /**
         * @brief Protect memory region with specified protection
         * @param address Start address of region
         * @param size Size of region
         * @param protection New protection flags
         * @return True if protection change succeeded, false otherwise
         */
        static bool ProtectMemory(mach_vm_address_t address, size_t size, vm_prot_t protection);
        
        /**
         * @brief Get information about memory regions in the process
         * @param regions Vector to store region information
         * @return True if retrieval succeeded, false otherwise
         */
        static bool GetMemoryRegions(std::vector<vm_region_basic_info_data_64_t>& regions);
        
        /**
         * @brief Find module base address by name
         * @param moduleName Name of the module to find
         * @return Base address of the module, 0 if not found
         */
        static mach_vm_address_t GetModuleBase(const std::string& moduleName);
        
        /**
         * @brief Get size of a module
         * @param moduleBase Base address of the module
         * @return Size of the module in bytes, 0 if not found
         */
        static size_t GetModuleSize(mach_vm_address_t moduleBase);
        
        /**
         * @brief Find a pattern in memory within a specified range
         * @param rangeStart Start address of the search range
         * @param rangeSize Size of the search range
         * @param pattern Byte pattern to search for
         * @param mask Mask for the pattern (? for wildcards)
         * @return Address where pattern was found, 0 if not found
         */
        static mach_vm_address_t FindPattern(mach_vm_address_t rangeStart, size_t rangeSize, 
                                           const std::string& pattern, const std::string& mask);
        
        /**
         * @brief Scan all memory regions for a pattern
         * @param pattern Byte pattern to search for
         * @param mask Mask for the pattern (? for wildcards)
         * @return Address where pattern was found, 0 if not found
         */
        static mach_vm_address_t ScanForPattern(const std::string& pattern, const std::string& mask);
        
        /**
         * @brief Clear all memory caches
         */
        static void ClearCache();
        
        /**
         * @brief Clean up resources used by memory access
         */
        static void Cleanup();
        
        /**
         * @brief Read a value of type T from memory
         * @tparam T Type of value to read
         * @param address Address to read from
         * @return Value read from memory, default-constructed T if read fails
         */
        template<typename T>
        static T ReadValue(mach_vm_address_t address) {
            T value = T();
            ReadMemory(address, &value, sizeof(T));
            return value;
        }
        
        /**
         * @brief Write a value of type T to memory
         * @tparam T Type of value to write
         * @param address Address to write to
         * @param value Value to write
         * @return True if write succeeded, false otherwise
         */
        template<typename T>
        static bool WriteValue(mach_vm_address_t address, const T& value) {
            return WriteMemory(address, &value, sizeof(T));
        }
        
        /**
         * @brief Force a refresh of the memory region cache
         */
        static void ForceRegionRefresh() {
            RefreshMemoryRegions();
        }
        
        /**
         * @brief Check if an address is part of a specified memory region with certain protection
         * @param address Address to check
         * @param requiredProtection Protection flags to check for
         * @return True if address is in a region with the specified protection, false otherwise
         */
        static bool IsAddressInRegionWithProtection(mach_vm_address_t address, vm_prot_t requiredProtection);
    };
}
