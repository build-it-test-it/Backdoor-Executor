#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
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
         */
        static std::vector<ScanResult> FindAllPatternsInModule(const std::string& moduleName, 
                                                              const std::string& patternStr);
        
        /**
         * @brief Resolve an ARM64 B/BL instruction's target address
         * @param instructionAddress Address of the B/BL instruction
         * @return Target address the instruction branches to, or 0 if invalid
         */
        static mach_vm_address_t ResolveBranchTarget(mach_vm_address_t instructionAddress);
        
        /**
         * @brief Resolve an ARM64 ADRP+ADD/LDR sequence to get the target address
         * @param adrpInstructionAddress Address of the ADRP instruction
         * @param nextInstructionOffset Offset to the next instruction (ADD or LDR)
         * @return Target address calculated from the instruction sequence, or 0 if invalid
         */
        static mach_vm_address_t ResolveAdrpSequence(mach_vm_address_t adrpInstructionAddress, 
                                                   size_t nextInstructionOffset = ARM64_INSTRUCTION_SIZE);
        
        /**
         * @brief Find a reference to a string in the module
         * @param moduleName Name of the module to scan
         * @param str String to find references to
         * @return ScanResult containing the address of a reference to the string
         */
        static ScanResult FindStringReference(const std::string& moduleName, const std::string& str);
    };
}
