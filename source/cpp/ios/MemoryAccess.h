#pragma once

#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach/vm_map.h>
#include <mach-o/dyld.h>
#include <vector>
#include <string>
#include <cstdint>

namespace iOS {
    /**
     * @class MemoryAccess
     * @brief Provides platform-specific memory access utilities for iOS
     * 
     * This class handles all memory-related operations for iOS, including reading/writing
     * process memory, finding modules, and scanning for patterns. It uses Mach kernel APIs
     * for all operations to ensure compatibility with iOS devices.
     */
    class MemoryAccess {
    private:
        // Private member variables with consistent m_ prefix
        static mach_port_t m_targetTask;
        static bool m_initialized;
        
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
         * @brief Clean up resources used by memory access
         */
        static void Cleanup();
    };
}
