#include "PatternScanner.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace iOS {
    bool PatternScanner::StringToPattern(const std::string& patternStr, 
                                       std::vector<uint8_t>& outBytes, 
                                       std::string& outMask) {
        outBytes.clear();
        outMask.clear();
        
        std::istringstream iss(patternStr);
        std::string byteStr;
        
        while (iss >> byteStr) {
            if (byteStr == "?" || byteStr == "??") {
                // Wildcard byte
                outBytes.push_back(0);
                outMask.push_back('?');
            } else {
                // Convert hex string to byte
                try {
                    uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
                    outBytes.push_back(byte);
                    outMask.push_back('x');
                } catch (const std::exception& e) {
                    // Invalid hex string
                    return false;
                }
            }
        }
        
        return !outBytes.empty() && outBytes.size() == outMask.size();
    }
    
    PatternScanner::ScanResult PatternScanner::FindPatternInModule(const std::string& moduleName, 
                                                               const std::string& patternStr) {
        // Convert pattern string to bytes and mask
        std::vector<uint8_t> patternBytes;
        std::string mask;
        if (!StringToPattern(patternStr, patternBytes, mask)) {
            return ScanResult(); // Invalid pattern
        }
        
        // Get module base and size
        mach_vm_address_t moduleBase = MemoryAccess::GetModuleBase(moduleName);
        if (moduleBase == 0) {
            return ScanResult(); // Module not found
        }
        
        size_t moduleSize = MemoryAccess::GetModuleSize(moduleBase);
        if (moduleSize == 0) {
            return ScanResult(); // Failed to get module size
        }
        
        // Allocate buffer for module memory
        std::vector<uint8_t> moduleBuffer(moduleSize);
        if (!MemoryAccess::ReadMemory(moduleBase, moduleBuffer.data(), moduleSize)) {
            return ScanResult(); // Failed to read module memory
        }
        
        // Scan for the pattern
        for (size_t i = 0; i <= moduleBuffer.size() - patternBytes.size(); i++) {
            bool found = true;
            
            for (size_t j = 0; j < patternBytes.size(); j++) {
                if (mask[j] == 'x' && moduleBuffer[i + j] != patternBytes[j]) {
                    found = false;
                    break;
                }
            }
            
            if (found) {
                // Pattern found, create result
                return ScanResult(moduleBase + i, moduleName, i);
            }
        }
        
        return ScanResult(); // Pattern not found
    }
    
    PatternScanner::ScanResult PatternScanner::FindPatternInRoblox(const std::string& patternStr) {
        // Roblox iOS module name
        const std::string robloxModuleName = "RobloxPlayer";
        return FindPatternInModule(robloxModuleName, patternStr);
    }
    
    std::vector<PatternScanner::ScanResult> PatternScanner::FindAllPatternsInModule(
        const std::string& moduleName, const std::string& patternStr) {
        // Results vector
        std::vector<ScanResult> results;
        
        // Convert pattern string to bytes and mask
        std::vector<uint8_t> patternBytes;
        std::string mask;
        if (!StringToPattern(patternStr, patternBytes, mask)) {
            return results; // Invalid pattern
        }
        
        // Get module base and size
        mach_vm_address_t moduleBase = MemoryAccess::GetModuleBase(moduleName);
        if (moduleBase == 0) {
            return results; // Module not found
        }
        
        size_t moduleSize = MemoryAccess::GetModuleSize(moduleBase);
        if (moduleSize == 0) {
            return results; // Failed to get module size
        }
        
        // Allocate buffer for module memory
        std::vector<uint8_t> moduleBuffer(moduleSize);
        if (!MemoryAccess::ReadMemory(moduleBase, moduleBuffer.data(), moduleSize)) {
            return results; // Failed to read module memory
        }
        
        // Scan for all occurrences of the pattern
        for (size_t i = 0; i <= moduleBuffer.size() - patternBytes.size(); i++) {
            bool found = true;
            
            for (size_t j = 0; j < patternBytes.size(); j++) {
                if (mask[j] == 'x' && moduleBuffer[i + j] != patternBytes[j]) {
                    found = false;
                    break;
                }
            }
            
            if (found) {
                // Pattern found, add to results
                results.push_back(ScanResult(moduleBase + i, moduleName, i));
            }
        }
        
        return results;
    }
    
    mach_vm_address_t PatternScanner::ResolveBranchTarget(mach_vm_address_t instructionAddress) {
        // Read the instruction
        uint32_t instruction;
        if (!MemoryAccess::ReadMemory(instructionAddress, &instruction, sizeof(instruction))) {
            return 0;
        }
        
        // Check if it's a B or BL instruction (ARM64)
        // B: 0x14000000 - 0x17FFFFFF
        // BL: 0x94000000 - 0x97FFFFFF
        bool isB = (instruction & 0xFC000000) == 0x14000000;
        bool isBL = (instruction & 0xFC000000) == 0x94000000;
        
        if (!isB && !isBL) {
            return 0; // Not a branch instruction
        }
        
        // Extract the signed 26-bit immediate from the instruction
        int32_t offset = instruction & 0x03FFFFFF;
        
        // Sign-extend if necessary (bit 25 is set)
        if (offset & 0x02000000) {
            offset |= 0xFC000000; // Sign extend to 32 bits
        }
        
        // Multiply by 4 to get byte offset (each ARM64 instruction is 4 bytes)
        offset *= 4;
        
        // Calculate target address
        return instructionAddress + offset;
    }
    
    mach_vm_address_t PatternScanner::ResolveAdrpSequence(mach_vm_address_t adrpInstructionAddress, 
                                                     size_t nextInstructionOffset) {
        // Read ADRP instruction
        uint32_t adrpInstruction;
        if (!MemoryAccess::ReadMemory(adrpInstructionAddress, &adrpInstruction, sizeof(adrpInstruction))) {
            return 0;
        }
        
        // Check if it's an ADRP instruction (ARM64)
        // ADRP: 0x90000000 - 0x9FFFFFFF
        if ((adrpInstruction & 0x9F000000) != 0x90000000) {
            return 0; // Not an ADRP instruction
        }
        
        // Read the next instruction (ADD or LDR)
        uint32_t nextInstruction;
        if (!MemoryAccess::ReadMemory(adrpInstructionAddress + nextInstructionOffset, 
                                    &nextInstruction, sizeof(nextInstruction))) {
            return 0;
        }
        
        // Extract destination register from ADRP (bits 0-4)
        uint32_t destReg = adrpInstruction & 0x1F;
        
        // Calculate the base address from ADRP
        // Extract the immhi and immlo fields
        uint32_t immhi = (adrpInstruction >> 5) & 0x7FFFF;
        uint32_t immlo = (adrpInstruction >> 29) & 0x3;
        
        // Combine and sign-extend the immediate
        int64_t imm = (immhi << 2) | immlo;
        if (imm & 0x100000) {
            imm |= 0xFFFFFFFFFFF00000; // Sign extend to 64 bits
        }
        
        // Calculate the page address (4KB pages in ARM64)
        mach_vm_address_t pageAddr = (adrpInstructionAddress & ~0xFFF) + (imm << 12);
        
        // Check if the next instruction is ADD or LDR and uses the same destination register
        // ADD: 0x91000000 - 0x91FFFFFF (immediate add)
        // LDR: 0xF9400000 - 0xF9FFFFFF (load register)
        bool isAdd = (nextInstruction & 0xFF000000) == 0x91000000;
        bool isLdr = (nextInstruction & 0xFF000000) == 0xF9400000;
        
        if (isAdd) {
            // Extract source register from ADD (should match dest reg from ADRP)
            uint32_t srcReg = (nextInstruction >> 5) & 0x1F;
            if (srcReg != destReg) {
                return 0; // Register mismatch
            }
            
            // Extract the 12-bit immediate from ADD
            uint32_t addImm = (nextInstruction >> 10) & 0xFFF;
            
            // Calculate final address
            return pageAddr + addImm;
        } 
        else if (isLdr) {
            // Extract base register from LDR (should match dest reg from ADRP)
            uint32_t baseReg = (nextInstruction >> 5) & 0x1F;
            if (baseReg != destReg) {
                return 0; // Register mismatch
            }
            
            // Extract the scaled 12-bit immediate from LDR
            uint32_t ldrImm = (nextInstruction >> 10) & 0xFFF;
            
            // Scale the immediate based on the size of the load (typically * 8 for 64-bit loads)
            ldrImm *= 8; // Assuming 64-bit load
            
            // Calculate the address being loaded from
            mach_vm_address_t loadAddr = pageAddr + ldrImm;
            
            // Optionally, read the value at this address
            mach_vm_address_t targetAddr;
            if (MemoryAccess::ReadMemory(loadAddr, &targetAddr, sizeof(targetAddr))) {
                return targetAddr;
            }
            
            return loadAddr;
        }
        
        // Just return the page address if we couldn't fully resolve the sequence
        return pageAddr;
    }
    
    PatternScanner::ScanResult PatternScanner::FindStringReference(const std::string& moduleName, 
                                                               const std::string& str) {
        // Get module base and size
        mach_vm_address_t moduleBase = MemoryAccess::GetModuleBase(moduleName);
        if (moduleBase == 0) {
            return ScanResult(); // Module not found
        }
        
        size_t moduleSize = MemoryAccess::GetModuleSize(moduleBase);
        if (moduleSize == 0) {
            return ScanResult(); // Failed to get module size
        }
        
        // First, find the string itself in memory
        std::vector<uint8_t> strBytes(str.begin(), str.end());
        strBytes.push_back(0); // Null terminator
        
        // Allocate buffer for module memory
        std::vector<uint8_t> moduleBuffer(moduleSize);
        if (!MemoryAccess::ReadMemory(moduleBase, moduleBuffer.data(), moduleSize)) {
            return ScanResult(); // Failed to read module memory
        }
        
        mach_vm_address_t stringAddr = 0;
        
        // Search for the string
        for (size_t i = 0; i <= moduleBuffer.size() - strBytes.size(); i++) {
            bool found = true;
            
            for (size_t j = 0; j < strBytes.size(); j++) {
                if (moduleBuffer[i + j] != strBytes[j]) {
                    found = false;
                    break;
                }
            }
            
            if (found) {
                stringAddr = moduleBase + i;
                break;
            }
        }
        
        if (stringAddr == 0) {
            return ScanResult(); // String not found
        }
        
        // Now find references to this string
        // In ARM64, look for ADRP/ADD sequences that would load the string address
        
        // Scan the module for potential references
        for (size_t i = 0; i <= moduleSize - 8; i += 4) { // ARM64 instructions are 4 bytes
            mach_vm_address_t instrAddr = moduleBase + i;
            
            // Try to resolve as an ADRP sequence
            mach_vm_address_t targetAddr = ResolveAdrpSequence(instrAddr);
            
            if (targetAddr == stringAddr) {
                return ScanResult(instrAddr, moduleName, i);
            }
        }
        
        return ScanResult(); // No reference found
    }
}
