#define CI_BUILD
#include "../ios_compat.h"
#include "PatternScanner.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <future>
#include <chrono>

namespace iOS {
    // Static cache and mutex for thread safety
    static std::mutex s_patternCacheMutex;
    static std::unordered_map<std::string, std::vector<PatternScanner::ScanResult>> s_patternResultCache;
    static std::unordered_map<std::string, mach_vm_address_t> s_stringCache;
    static std::atomic<bool> s_useParallelScanning{true}; // Enable parallel scanning by default
    
    // Helper function to create cache key
    static std::string CreateCacheKey(const std::string& moduleName, const std::string& pattern) {
        return moduleName + ":" + pattern;
    }
    
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
    
    // Improved scanning algorithm using Boyer-Moore-Horspool
    mach_vm_address_t ScanWithBoyerMooreHorspool(
        const uint8_t* haystack, size_t haystackSize,
        const std::vector<uint8_t>& needle, const std::string& mask) {
        
        if (needle.empty() || haystackSize < needle.size()) {
            return 0;
        }
        
        // Create bad character table
        size_t badCharTable[256];
        const size_t needleSize = needle.size();
        
        for (size_t i = 0; i < 256; i++) {
            badCharTable[i] = needleSize;
        }
        
        // Fill the table with the last positions of each character
        for (size_t i = 0; i < needleSize - 1; i++) {
            if (mask[i] == 'x') { // Only use non-wildcard characters
                badCharTable[needle[i]] = needleSize - 1 - i;
            }
        }
        
        // Start searching
        size_t offset = 0;
        while (offset <= haystackSize - needleSize) {
            size_t j = needleSize - 1;
            
            // Compare from right to left
            while (true) {
                if (mask[j] == '?' || haystack[offset + j] == needle[j]) {
                    if (j == 0) {
                        return offset; // Match found
                    }
                    j--;
                } else {
                    break;
                }
            }
            
            // Shift based on bad character rule or by 1 if character is wildcard
            uint8_t badChar = haystack[offset + needleSize - 1];
            offset += std::max(size_t(1), badCharTable[badChar]);
        }
        
        return 0; // Not found
    }
    
    // Enhanced pattern scanner that can use multithreading for large scans
    mach_vm_address_t ScanMemoryRegionParallel(
        mach_vm_address_t startAddress, const uint8_t* buffer, size_t bufferSize,
        const std::vector<uint8_t>& pattern, const std::string& mask) {
        
        // For small buffers, don't bother with multithreading
        if (bufferSize < 1024 * 1024) { // 1MB threshold
            mach_vm_address_t result = ScanWithBoyerMooreHorspool(buffer, bufferSize, pattern, mask);
            return result ? (startAddress + result) : 0;
        }
        
        // For larger buffers, use parallel scanning if enabled
        if (!s_useParallelScanning) {
            mach_vm_address_t result = ScanWithBoyerMooreHorspool(buffer, bufferSize, pattern, mask);
            return result ? (startAddress + result) : 0;
        }
        
        // Determine thread count based on available cores (max 4 threads for memory scans)
        unsigned int threadCount = std::min(4u, std::thread::hardware_concurrency());
        if (threadCount <= 1) {
            // Single thread fallback
            mach_vm_address_t result = ScanWithBoyerMooreHorspool(buffer, bufferSize, pattern, mask);
            return result ? (startAddress + result) : 0;
        }
        
        // Divide the buffer into chunks for parallel scanning
        size_t chunkSize = bufferSize / threadCount;
        size_t overlap = pattern.size() - 1; // Overlap to avoid missing patterns at chunk boundaries
        
        std::vector<std::future<mach_vm_address_t>> futures;
        std::atomic<bool> patternFound{false};
        std::atomic<mach_vm_address_t> foundAddress{0};
        
        // Launch worker threads
        for (unsigned int i = 0; i < threadCount; i++) {
            size_t startOffset = i * chunkSize;
            size_t scanSize = (i == threadCount - 1) ? (bufferSize - startOffset) : (chunkSize + overlap);
            
            // Ensure we don't go past the buffer end
            if (startOffset + scanSize > bufferSize) {
                scanSize = bufferSize - startOffset;
            }
            
            // Create future for this chunk
            futures.push_back(std::async(std::launch::async, [=, &patternFound, &foundAddress]() {
                // Check if pattern already found by another thread
                if (patternFound) return mach_vm_address_t(0);
                
                // Scan this chunk
                mach_vm_address_t result = ScanWithBoyerMooreHorspool(
                    buffer + startOffset, scanSize, pattern, mask);
                
                if (result) {
                    patternFound = true;
                    return startAddress + startOffset + result;
                }
                
                return mach_vm_address_t(0);
            }));
        }
        
        // Wait for results and return the first match
        for (auto& future : futures) {
            mach_vm_address_t result = future.get();
            if (result) {
                foundAddress = result;
                break;
            }
        }
        
        return foundAddress;
    }
    
    PatternScanner::ScanResult PatternScanner::FindPatternInModule(const std::string& moduleName, 
                                                               const std::string& patternStr) {
        // Check the cache first
        std::string cacheKey = CreateCacheKey(moduleName, patternStr);
        {
            std::lock_guard<std::mutex> lock(s_patternCacheMutex);
            auto it = s_patternResultCache.find(cacheKey);
            if (it != s_patternResultCache.end() && !it->second.empty()) {
                return it->second[0]; // Return the first cached result
            }
        }
        
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
        
        // Allocate buffer for module memory - only allocate what we actually need
        // For very large modules, read in manageable chunks
        constexpr size_t MAX_CHUNK_SIZE = 16 * 1024 * 1024; // 16MB max chunk
        
        if (moduleSize <= MAX_CHUNK_SIZE) {
            // Read the entire module at once for small modules
            std::vector<uint8_t> moduleBuffer(moduleSize);
            if (!MemoryAccess::ReadMemory(moduleBase, moduleBuffer.data(), moduleSize)) {
                return ScanResult(); // Failed to read module memory
            }
            
            // Scan for the pattern
            mach_vm_address_t matchAddress = ScanMemoryRegionParallel(
                moduleBase, moduleBuffer.data(), moduleSize, patternBytes, mask);
            
            if (matchAddress) {
                // Pattern found, create result
                ScanResult result(matchAddress, moduleName, matchAddress - moduleBase);
                
                // Cache the result
                std::lock_guard<std::mutex> lock(s_patternCacheMutex);
                s_patternResultCache[cacheKey] = {result};
                
                return result;
            }
        } else {
            // For large modules, scan in chunks
            size_t chunkSize = MAX_CHUNK_SIZE;
            std::vector<uint8_t> chunkBuffer(chunkSize);
            
            for (size_t offset = 0; offset < moduleSize; offset += chunkSize) {
                // Adjust chunk size for the final chunk
                size_t currentChunkSize = std::min(chunkSize, moduleSize - offset);
                
                // Read this chunk
                if (!MemoryAccess::ReadMemory(moduleBase + offset, chunkBuffer.data(), currentChunkSize)) {
                    continue; // Skip this chunk if read fails
                }
                
                // Scan for the pattern in this chunk
                mach_vm_address_t matchAddress = ScanMemoryRegionParallel(
                    moduleBase + offset, chunkBuffer.data(), currentChunkSize, patternBytes, mask);
                
                if (matchAddress) {
                    // Pattern found, create result
                    ScanResult result(matchAddress, moduleName, matchAddress - moduleBase);
                    
                    // Cache the result
                    std::lock_guard<std::mutex> lock(s_patternCacheMutex);
                    s_patternResultCache[cacheKey] = {result};
                    
                    return result;
                }
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
        // Check the cache first
        std::string cacheKey = CreateCacheKey(moduleName, patternStr);
        {
            std::lock_guard<std::mutex> lock(s_patternCacheMutex);
            auto it = s_patternResultCache.find(cacheKey);
            if (it != s_patternResultCache.end()) {
                return it->second; // Return cached results
            }
        }
        
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
        
        // Process in manageable chunks for large modules
        constexpr size_t MAX_CHUNK_SIZE = 16 * 1024 * 1024; // 16MB max chunk
        
        // Helper function to process a memory buffer
        auto processBuffer = [&](const uint8_t* buffer, size_t bufferSize, mach_vm_address_t baseAddress) {
            size_t offset = 0;
            while (offset <= bufferSize - patternBytes.size()) {
                bool found = true;
                
                // Check pattern match
                for (size_t j = 0; j < patternBytes.size(); j++) {
                    if (mask[j] == 'x' && buffer[offset + j] != patternBytes[j]) {
                        found = false;
                        break;
                    }
                }
                
                if (found) {
                    // Pattern found, add to results
                    results.push_back(ScanResult(baseAddress + offset, moduleName, 
                                              baseAddress + offset - moduleBase));
                    
                    // Skip to after this match
                    offset += patternBytes.size();
                } else {
                    // Move to next position
                    offset++;
                }
            }
        };
        
        if (moduleSize <= MAX_CHUNK_SIZE) {
            // Read the entire module at once for small modules
            std::vector<uint8_t> moduleBuffer(moduleSize);
            if (MemoryAccess::ReadMemory(moduleBase, moduleBuffer.data(), moduleSize)) {
                processBuffer(moduleBuffer.data(), moduleSize, moduleBase);
            }
        } else {
            // For large modules, scan in chunks
            size_t chunkSize = MAX_CHUNK_SIZE;
            std::vector<uint8_t> chunkBuffer(chunkSize);
            
            for (size_t offset = 0; offset < moduleSize; offset += chunkSize) {
                // Adjust chunk size for the final chunk
                size_t currentChunkSize = std::min(chunkSize, moduleSize - offset);
                
                // Read this chunk
                if (MemoryAccess::ReadMemory(moduleBase + offset, chunkBuffer.data(), currentChunkSize)) {
                    processBuffer(chunkBuffer.data(), currentChunkSize, moduleBase + offset);
                }
            }
        }
        
        // Cache the results
        if (!results.empty()) {
            std::lock_guard<std::mutex> lock(s_patternCacheMutex);
            s_patternResultCache[cacheKey] = results;
        }
        
        return results;
    }
    
    // Helper to resolve branch targets in ARM64 instructions
    mach_vm_address_t PatternScanner::ResolveBranchTarget(mach_vm_address_t instructionAddress) {
        // Read the instruction
        uint32_t instruction;
        if (!MemoryAccess::ReadMemory(instructionAddress, &instruction, sizeof(instruction))) {
            return 0;
        }
        
        // Check if it's a B or BL instruction (ARM64)
        // B:   0x14000000 - 0x17FFFFFF
        // BL:  0x94000000 - 0x97FFFFFF
        // CBZ: 0xB4000000 - 0xBFFFFFFF (Conditional branch if zero)
        // CBNZ: 0xB5000000 - 0xBFFFFFFF (Conditional branch if not zero)
        
        bool isB = (instruction & 0xFC000000) == 0x14000000;
        bool isBL = (instruction & 0xFC000000) == 0x94000000;
        bool isCBZ = (instruction & 0x7F000000) == 0x34000000;
        bool isCBNZ = (instruction & 0x7F000000) == 0x35000000;
        
        if (!isB && !isBL && !isCBZ && !isCBNZ) {
            return 0; // Not a recognized branch instruction
        }
        
        // For B/BL: Extract the signed 26-bit immediate
        // For CBZ/CBNZ: Extract the signed 19-bit immediate
        int32_t offset;
        
        if (isB || isBL) {
            offset = instruction & 0x03FFFFFF;
            
            // Sign-extend if necessary (bit 25 is set)
            if (offset & 0x02000000) {
                offset |= 0xFC000000; // Sign extend to 32 bits
            }
        } else { // CBZ/CBNZ
            offset = (instruction >> 5) & 0x7FFFF;
            
            // Sign-extend if necessary (bit 18 is set)
            if (offset & 0x40000) {
                offset |= 0xFFF80000; // Sign extend to 32 bits
            }
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
        // Format: ADRP Xd, imm{21:0}
        bool isAdrp = ((adrpInstruction >> 24) & 0x9F) == 0x90;
        
        if (!isAdrp) {
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
        // Extract the immhi (bits 5-23) and immlo (bits 29-30) fields
        uint32_t immhi = (adrpInstruction >> 5) & 0x7FFFF;
        uint32_t immlo = (adrpInstruction >> 29) & 0x3;
        
        // Combine to form the 21-bit signed immediate value
        int64_t imm = (immhi << 2) | immlo;
        
        // Sign-extend the 21-bit immediate to 64 bits
        if (imm & 0x100000) {
            imm |= 0xFFFFFFFFFFF00000;
        }
        
        // Calculate the page address (4KB pages in ARM64)
        mach_vm_address_t pageAddr = (adrpInstructionAddress & ~0xFFF) + (imm << 12);
        
        // Determine type of next instruction
        // ADD immediate: 0x91000000 - 0x91FFFFFF (format: ADD Xd, Xn, #imm)
        // LDR (64-bit): 0xF9400000 - 0xF9FFFFFF (format: LDR Xt, [Xn, #imm])
        // LDR (32-bit): 0xB9400000 - 0xB9FFFFFF (format: LDR Wt, [Xn, #imm])
        bool isAdd = ((nextInstruction >> 22) & 0x3FF) == 0x244;   // ADD Xd, Xn, #imm
        bool isLdr64 = ((nextInstruction >> 22) & 0x3FF) == 0x3D5; // LDR Xt, [Xn, #imm]
        bool isLdr32 = ((nextInstruction >> 22) & 0x3FF) == 0x2E5; // LDR Wt, [Xn, #imm]
        bool isLdrb = ((nextInstruction >> 22) & 0x3FF) == 0x285;  // LDRB Wt, [Xn, #imm]
        
        if (isAdd) {
            // Extract source register from ADD (bits 5-9)
            uint32_t srcReg = (nextInstruction >> 5) & 0x1F;
            if (srcReg != destReg) {
                return 0; // Register mismatch
            }
            
            // Extract the 12-bit immediate (bits 10-21)
            uint32_t addImm = (nextInstruction >> 10) & 0xFFF;
            
            // Calculate final address
            return pageAddr + addImm;
        } 
        else if (isLdr64 || isLdr32 || isLdrb) {
            // Extract base register from LDR (bits 5-9)
            uint32_t baseReg = (nextInstruction >> 5) & 0x1F;
            if (baseReg != destReg) {
                return 0; // Register mismatch
            }
            
            // Extract the 12-bit immediate (bits 10-21)
            uint32_t ldrImm = (nextInstruction >> 10) & 0xFFF;
            
            // Scale the immediate based on the size of the load
            if (isLdr64) {
                ldrImm *= 8; // 64-bit load scales by 8
            } else if (isLdr32) {
                ldrImm *= 4; // 32-bit load scales by 4
            } else if (isLdrb) {
                // LDRB doesn't scale (byte access)
            }
            
            // Calculate the address being loaded from
            mach_vm_address_t loadAddr = pageAddr + ldrImm;
            
            // For LDR, optionally try to read the final target
            if (isLdr64 || isLdr32) {
                // Determine value size
                size_t valueSize = isLdr64 ? 8 : 4;
                
                // Try to read the actual value at the load address
                if (valueSize == 8) {
                    uint64_t value = 0;
                    if (MemoryAccess::ReadMemory(loadAddr, &value, valueSize)) {
                        return value;
                    }
                } else {
                    uint32_t value = 0;
                    if (MemoryAccess::ReadMemory(loadAddr, &value, valueSize)) {
                        return value;
                    }
                }
            }
            
            // If we couldn't read the value, just return the load address
            return loadAddr;
        }
        
        // Just return the page address if we couldn't fully resolve the sequence
        return pageAddr;
    }
    
    PatternScanner::ScanResult PatternScanner::FindStringReference(const std::string& moduleName, 
                                                               const std::string& str) {
        // Check cache first
        {
            std::lock_guard<std::mutex> lock(s_patternCacheMutex);
            std::string cacheKey = moduleName + ":string:" + str;
            auto it = s_stringCache.find(cacheKey);
            if (it != s_stringCache.end()) {
                mach_vm_address_t stringAddr = it->second;
                return ScanResult(stringAddr, moduleName, stringAddr - MemoryAccess::GetModuleBase(moduleName));
            }
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
        
        // First, find the string itself in memory
        std::vector<uint8_t> strBytes(str.begin(), str.end());
        strBytes.push_back(0); // Null terminator
        
        // Process in manageable chunks for large modules
        constexpr size_t MAX_CHUNK_SIZE = 16 * 1024 * 1024; // 16MB max chunk
        mach_vm_address_t stringAddr = 0;
        
        if (moduleSize <= MAX_CHUNK_SIZE) {
            // Read the entire module at once for small modules
            std::vector<uint8_t> moduleBuffer(moduleSize);
            if (!MemoryAccess::ReadMemory(moduleBase, moduleBuffer.data(), moduleSize)) {
                return ScanResult(); // Failed to read module memory
            }
            
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
        } else {
            // For large modules, scan in chunks
            size_t chunkSize = MAX_CHUNK_SIZE;
            std::vector<uint8_t> chunkBuffer(chunkSize);
            
            for (size_t offset = 0; offset < moduleSize && stringAddr == 0; offset += chunkSize) {
                // Adjust chunk size for the final chunk
                size_t currentChunkSize = std::min(chunkSize, moduleSize - offset);
                
                // Read this chunk
                if (!MemoryAccess::ReadMemory(moduleBase + offset, chunkBuffer.data(), currentChunkSize)) {
                    continue; // Skip this chunk if read fails
                }
                
                // Search for the string in this chunk
                for (size_t i = 0; i <= currentChunkSize - strBytes.size(); i++) {
                    bool found = true;
                    
                    for (size_t j = 0; j < strBytes.size(); j++) {
                        if (chunkBuffer[i + j] != strBytes[j]) {
                            found = false;
                            break;
                        }
                    }
                    
                    if (found) {
                        stringAddr = moduleBase + offset + i;
                        break;
                    }
                }
            }
        }
        
        if (stringAddr == 0) {
            return ScanResult(); // String not found
        }
        
        // Cache the string address
        {
            std::lock_guard<std::mutex> lock(s_patternCacheMutex);
            std::string cacheKey = moduleName + ":string:" + str;
            s_stringCache[cacheKey] = stringAddr;
        }
        
        // Now find references to this string
        // In ARM64, look for ADRP/ADD sequences that would load the string address
        
        // Process in chunks to avoid excessive memory usage
        mach_vm_address_t refAddr = 0;
        
        // Scan the module for potential references
        for (size_t offset = 0; offset < moduleSize && refAddr == 0; offset += MAX_CHUNK_SIZE) {
            size_t chunkSize = std::min(MAX_CHUNK_SIZE, moduleSize - offset);
            
            for (size_t i = 0; i < chunkSize; i += 4) { // ARM64 instructions are 4 bytes
                mach_vm_address_t instrAddr = moduleBase + offset + i;
                
                // Try to resolve as an ADRP sequence
                mach_vm_address_t targetAddr = ResolveAdrpSequence(instrAddr);
                
                if (targetAddr == stringAddr) {
                    refAddr = instrAddr;
                    break;
                }
            }
        }
        
        if (refAddr != 0) {
            return ScanResult(refAddr, moduleName, refAddr - moduleBase);
        }
        
        // If we couldn't find a reference, at least return the string address
        return ScanResult(stringAddr, moduleName, stringAddr - moduleBase);
    }
    
    void PatternScanner::SetUseParallelScanning(bool enable) {
        s_useParallelScanning = enable;
    }
    
    bool PatternScanner::GetUseParallelScanning() {
        return s_useParallelScanning;
    }
    
    void PatternScanner::ClearCache() {
        std::lock_guard<std::mutex> lock(s_patternCacheMutex);
        s_patternResultCache.clear();
        s_stringCache.clear();
    }
}
