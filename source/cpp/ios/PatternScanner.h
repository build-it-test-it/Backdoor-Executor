#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <atomic>
// Include MemoryAccess.h first as it contains the mach_vm typedefs and compatibility wrappers
#include "MemoryAccess.h"

// MemoryAccess.h should already have defined all necessary typedefs
// No additional typedefs needed here

namespace iOS {
    /**
     * @class PatternScanner
     * @brief Specialized pattern scanner for ARM64 architecture on iOS
     * 
     * This class provides pattern scanning functionality specifically optimized
     * for ARM64 instruction patterns and iOS memory layout. It works with the
     * iOS::MemoryAccess class to perform memory operations.
     * 
     * Features:
     * - Thread-safe implementation with caching for better performance
     * - Optimized Boyer-Moore-Horspool algorithm for faster pattern matching
     * - Multi-threaded scanning for large memory regions
     * - Chunk-based scanning to reduce memory usage
     * - Comprehensive ARM64 instruction parsing
     */
    class PatternScanner {
    private:
        // Member variables
        static const size_t ARM64_INSTRUCTION_SIZE = 4; // ARM64 instructions are 4 bytes
        
    public:
        /**
         * @struct ScanResult
         * @brief Contains the result of a pattern scan with additional metadata
         */
        struct ScanResult {
            mach_vm_address_t m_address;  // The address where the pattern was found
            std::string m_moduleName;     // The module name containing the pattern
            size_t m_offset;              // Offset from module base address
            
            ScanResult() : m_address(0), m_moduleName(""), m_offset(0) {}
            
            ScanResult(mach_vm_address_t address, const std::string& moduleName, size_t offset) 
                : m_address(address), m_moduleName(moduleName), m_offset(offset) {}
            
            bool IsValid() const { return m_address != 0; }
        };
        
        /**
         * @brief Convert a pattern string to byte pattern and mask
         * @param patternStr Pattern string with wildcards (e.g., "48 8B ? ? 90")
         * @param outBytes Output vector to store the byte pattern
         * @param outMask Output string to store the mask ('x' for match, '?' for wildcard)
         * @return True if conversion was successful, false otherwise
         */
        static bool StringToPattern(const std::string& patternStr, 
                                   std::vector<uint8_t>& outBytes, 
                                   std::string& outMask);
        
        /**
         * @brief Find a pattern in memory within a specific module
         * @param moduleName Name of the module to scan
         * @param patternStr Pattern string with wildcards (e.g., "48 8B ? ? 90")
         * @return ScanResult containing the found address and metadata, or invalid result if not found
         * 
         * Enhanced with multi-threaded scanning for large modules and result caching
         */
        static ScanResult FindPatternInModule(const std::string& moduleName, 
                                             const std::string& patternStr);
        
        /**
         * @brief Find a pattern in memory within Roblox's main module
         * @param patternStr Pattern string with wildcards (e.g., "48 8B ? ? 90")
         * @return ScanResult containing the found address and metadata, or invalid result if not found
         */
        static ScanResult FindPatternInRoblox(const std::string& patternStr);
        
        /**
         * @brief Find all occurrences of a pattern in a specific module
         * @param moduleName Name of the module to scan
         * @param patternStr Pattern string with wildcards (e.g., "48 8B ? ? 90")
         * @return Vector of ScanResults for all occurrences
         * 
         * Enhanced with chunk-based scanning for large modules and result caching
         */
        static std::vector<ScanResult> FindAllPatternsInModule(const std::string& moduleName, 
                                                              const std::string& patternStr);
        
        /**
         * @brief Resolve an ARM64 branch instruction's target address
         * @param instructionAddress Address of the branch instruction
         * @return Target address the instruction branches to, or 0 if invalid
         * 
         * Supports B, BL, CBZ, and CBNZ instruction types
         */
        static mach_vm_address_t ResolveBranchTarget(mach_vm_address_t instructionAddress);
        
        /**
         * @brief Resolve an ARM64 ADRP+ADD/LDR sequence to get the target address
         * @param adrpInstructionAddress Address of the ADRP instruction
         * @param nextInstructionOffset Offset to the next instruction (ADD or LDR)
         * @return Target address calculated from the instruction sequence, or 0 if invalid
         * 
         * Enhanced to support 64-bit, 32-bit, and byte load instructions
         */
        static mach_vm_address_t ResolveAdrpSequence(mach_vm_address_t adrpInstructionAddress, 
                                                   size_t nextInstructionOffset = ARM64_INSTRUCTION_SIZE);
        
        /**
         * @brief Find a reference to a string in the module
         * @param moduleName Name of the module to scan
         * @param str String to find references to
         * @return ScanResult containing the address of a reference to the string
         * 
         * Enhanced with chunk-based scanning for large modules and result caching
         */
        static ScanResult FindStringReference(const std::string& moduleName, const std::string& str);
        
        /**
         * @brief Enable or disable parallel scanning
         * @param enable Whether to enable parallel scanning
         * 
         * Parallel scanning uses multiple threads to scan large memory regions,
         * which can significantly improve performance on multi-core devices.
         */
        static void SetUseParallelScanning(bool enable);
        
        /**
         * @brief Check if parallel scanning is enabled
         * @return True if parallel scanning is enabled, false otherwise
         */
        static bool GetUseParallelScanning();
        
        /**
         * @brief Clear the pattern and string cache
         * 
         * This is useful when memory has been modified and cached results may be invalid,
         * or to free up memory.
         */
        static void ClearCache();
    };
    
    /**
     * @brief Helper function to scan memory using Boyer-Moore-Horspool algorithm
     * @param haystack Buffer to scan
     * @param haystackSize Size of the buffer
     * @param needle Pattern to find
     * @param mask Mask for the pattern ('x' for match, '?' for wildcard)
     * @return Offset where the pattern was found, or 0 if not found
     */
    mach_vm_address_t ScanWithBoyerMooreHorspool(
        const uint8_t* haystack, size_t haystackSize,
        const std::vector<uint8_t>& needle, const std::string& mask);
    
    /**
     * @brief Enhanced pattern scanner that can use multithreading for large scans
     * @param startAddress Base address of the memory region
     * @param buffer Buffer containing the memory data
     * @param bufferSize Size of the buffer
     * @param pattern Pattern to find
     * @param mask Mask for the pattern ('x' for match, '?' for wildcard)
     * @return Address where the pattern was found, or 0 if not found
     */
    mach_vm_address_t ScanMemoryRegionParallel(
        mach_vm_address_t startAddress, const uint8_t* buffer, size_t bufferSize,
        const std::vector<uint8_t>& pattern, const std::string& mask);
}
