
#include "../ios_compat.h"
#include "MemoryAccess.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <dlfcn.h>
#include <mach/mach_error.h>
#include <mach-o/dyld_images.h>
#include <sys/sysctl.h>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <algorithm>

namespace iOS {
    // Initialize static members
    mach_port_t MemoryAccess::m_targetTask = MACH_PORT_NULL;
    std::atomic<bool> MemoryAccess::m_initialized{false};
    std::mutex MemoryAccess::m_accessMutex;
    std::mutex MemoryAccess::m_cacheMutex;
    
    // Add a cache for memory regions to avoid redundant scans
    std::unordered_map<std::string, mach_vm_address_t> MemoryAccess::m_patternCache;
    std::unordered_map<std::string, mach_vm_address_t> MemoryAccess::m_moduleBaseCache;
    std::unordered_map<mach_vm_address_t, size_t> MemoryAccess::m_moduleSizeCache;
    
    // Cache memory regions for faster scanning
    std::vector<std::pair<mach_vm_address_t, mach_vm_address_t>> MemoryAccess::m_cachedReadableRegions;
    uint64_t MemoryAccess::m_regionsLastUpdated = 0;
    
    bool MemoryAccess::Initialize() {
        // Double-checked locking pattern
        if (!m_initialized) {
            std::lock_guard<std::mutex> lock(m_accessMutex);
            if (!m_initialized) {
                // Get the task port for our own process
                kern_return_t kr = task_self_trap();
                if (kr == KERN_SUCCESS) {
                    m_targetTask = kr;
                    // Warm up the region cache
                    RefreshMemoryRegions();
                    m_initialized = true;
                    return true;
                }
                
                std::cerr << "Failed to get task port: " << mach_error_string(kr) << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    bool MemoryAccess::ReadMemory(mach_vm_address_t address, void* buffer, size_t size) {
        if (!m_initialized) {
            if (!Initialize()) {
                return false;
            }
        }
        
        // Validate address
        if (!IsAddressValid(address, size)) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(m_accessMutex);
        
        vm_size_t bytesRead;
        kern_return_t kr = vm_read_overwrite(m_targetTask, address, size, 
                                           (vm_address_t)buffer, &bytesRead);
        
        if (kr != KERN_SUCCESS) {
            // Only log serious errors
            if (kr != KERN_INVALID_ADDRESS) {
                std::cerr << "ReadMemory failed at 0x" << std::hex << address << ": " 
                          << mach_error_string(kr) << std::endl;
            }
            return false;
        }
        
        return bytesRead == size;
    }
    
    bool MemoryAccess::WriteMemory(mach_vm_address_t address, const void* buffer, size_t size) {
        if (!m_initialized) {
            if (!Initialize()) {
                return false;
            }
        }
        
        // Validate address
        if (!IsAddressValid(address, size)) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(m_accessMutex);
        
        // Get current protection
        vm_region_basic_info_data_64_t info;
        mach_msg_type_number_t infoCount = VM_REGION_BASIC_INFO_COUNT_64;
        vm_size_t vmSize;
        mach_port_t objectName = MACH_PORT_NULL;
        kern_return_t kr = vm_region_64(m_targetTask, (vm_address_t*)&address, &vmSize,
                                      VM_REGION_BASIC_INFO_64, (vm_region_info_t)&info,
                                      &infoCount, &objectName);
                                      
        if (kr != KERN_SUCCESS) {
            std::cerr << "Failed to get region info: " << mach_error_string(kr) << std::endl;
            return false;
        }
        
        // If memory is not writable, make it writable temporarily
        vm_prot_t oldProtection = info.protection;
        bool needToRestore = !(oldProtection & VM_PROT_WRITE);
        
        if (needToRestore) {
            // Try to make memory writable
            kr = vm_protect(m_targetTask, address, size, FALSE, 
                          oldProtection | VM_PROT_WRITE);
            
            if (kr != KERN_SUCCESS) {
                std::cerr << "Failed to change memory protection: " << mach_error_string(kr) << std::endl;
                return false;
            }
        }
        
        // Write the memory
        kr = vm_write(m_targetTask, address, (vm_offset_t)buffer, size);
        
        // Restore original protection if needed
        if (needToRestore) {
            vm_protect(m_targetTask, address, size, FALSE, oldProtection);
        }
        
        if (kr != KERN_SUCCESS) {
            std::cerr << "WriteMemory failed at 0x" << std::hex << address << ": " 
                      << mach_error_string(kr) << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool MemoryAccess::IsAddressValid(mach_vm_address_t address, size_t size) {
        // Quick validation of address range
        if (address == 0 || address + size < address) {
            return false;
        }
        
        // Ensure we have region information
        if (m_cachedReadableRegions.empty()) {
            RefreshMemoryRegions();
        }
        
        // Check if address is in a readable region
        for (const auto& region : m_cachedReadableRegions) {
            mach_vm_address_t start = region.first;
            mach_vm_address_t end = region.second;
            
            if (address >= start && address + size <= end) {
                return true;
            }
        }
        
        return false;
    }
    
    bool MemoryAccess::ProtectMemory(mach_vm_address_t address, size_t size, vm_prot_t protection) {
        if (!m_initialized) {
            if (!Initialize()) {
                return false;
            }
        }
        
        std::lock_guard<std::mutex> lock(m_accessMutex);
        
        kern_return_t kr = vm_protect(m_targetTask, address, size, FALSE, protection);
        
        if (kr != KERN_SUCCESS) {
            std::cerr << "ProtectMemory failed at 0x" << std::hex << address << ": " 
                      << mach_error_string(kr) << std::endl;
            return false;
        }
        
        return true;
    }
    
    void MemoryAccess::RefreshMemoryRegions() {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        
        // Clear existing regions
        m_cachedReadableRegions.clear();
        
        // Variables for memory region iteration
        vm_address_t address = 0;
        vm_size_t size = 0;
        vm_region_basic_info_data_64_t info;
        mach_msg_type_number_t infoCount = VM_REGION_BASIC_INFO_COUNT_64;
        mach_port_t objectName = MACH_PORT_NULL;
        kern_return_t kr = KERN_SUCCESS;
        
        // Iterate through all memory regions
        while (true) {
            kr = vm_region_64(m_targetTask, &address, &size,
                            VM_REGION_BASIC_INFO_64, (vm_region_info_t)&info,
                            &infoCount, &objectName);
            
            if (kr != KERN_SUCCESS) {
                break;
            }
            
            // Store readable regions
            if (info.protection & VM_PROT_READ) {
                m_cachedReadableRegions.emplace_back(address, address + size);
            }
            
            // Move to next region
            address += size;
        }
        
        // Sort regions by address for faster lookup
        std::sort(m_cachedReadableRegions.begin(), m_cachedReadableRegions.end(),
                [](const auto& a, const auto& b) { return a.first < b.first; });
        
        // Update timestamp
        m_regionsLastUpdated = GetCurrentTimestamp();
    }
    
    uint64_t MemoryAccess::GetCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    bool MemoryAccess::GetMemoryRegions(std::vector<vm_region_basic_info_data_64_t>& regions) {
        if (!m_initialized && !Initialize()) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(m_accessMutex);
        
        regions.clear();
        
        // Variables for memory region iteration
        vm_address_t vm_address = 0;
        vm_size_t vm_size = 0;
        vm_region_basic_info_data_64_t info;
        mach_msg_type_number_t infoCount = VM_REGION_BASIC_INFO_COUNT_64;
        mach_port_t objectName = MACH_PORT_NULL;
        kern_return_t kr = KERN_SUCCESS;
        
        while (true) {
            kr = vm_region_64(
                m_targetTask,
                &vm_address,
                &vm_size,
                VM_REGION_BASIC_INFO_64,
                (vm_region_info_t)&info, 
                &infoCount, 
                &objectName);
            
            if (kr != KERN_SUCCESS) {
                if (kr != KERN_INVALID_ADDRESS) {
                    std::cerr << "GetMemoryRegions failed: " << mach_error_string(kr) << std::endl;
                }
                break;
            }
            
            // Store region size in the upper bits of the protection field so we can access it later
            info.protection |= ((uint64_t)vm_size & 0xFFFFFFFF) << 32;
            
            regions.push_back(info);
            vm_address += vm_size;
        }
        
        // Update the cached regions while we're at it
        if (m_regionsLastUpdated == 0 || GetCurrentTimestamp() - m_regionsLastUpdated > 30000) { // 30 seconds
            RefreshMemoryRegions();
        }
        
        return !regions.empty();
    }
    
    mach_vm_address_t MemoryAccess::GetModuleBase(const std::string& moduleName) {
        // Check cache first
        {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            auto it = m_moduleBaseCache.find(moduleName);
            if (it != m_moduleBaseCache.end()) {
                return it->second;
            }
        }
        
        // Not in cache, look it up
        mach_vm_address_t baseAddress = 0;
        
        // Get the image count
        const uint32_t imageCount = _dyld_image_count();
        
        // Iterate through all loaded modules
        for (uint32_t i = 0; i < imageCount; i++) {
            const char* imageName = _dyld_get_image_name(i);
            if (imageName && strstr(imageName, moduleName.c_str())) {
                baseAddress = _dyld_get_image_vmaddr_slide(i) + (mach_vm_address_t)_dyld_get_image_header(i);
                break;
            }
        }
        
        // Add to cache
        if (baseAddress != 0) {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            m_moduleBaseCache[moduleName] = baseAddress;
        }
        
        return baseAddress;
    }
    
    size_t MemoryAccess::GetModuleSize(mach_vm_address_t moduleBase) {
        if (moduleBase == 0) {
            return 0;
        }
        
        // Check cache first
        {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            auto it = m_moduleSizeCache.find(moduleBase);
            if (it != m_moduleSizeCache.end()) {
                return it->second;
            }
        }
        
        // Not in cache, compute it
        size_t totalSize = 0;
        
        // Read the Mach-O header
        struct mach_header_64 header;
        if (!ReadMemory(moduleBase, &header, sizeof(header))) {
            return 0;
        }
        
        // Ensure it's a valid 64-bit Mach-O
        if (header.magic != MH_MAGIC_64) {
            return 0;
        }
        
        // Calculate the total size from Mach-O segments
        mach_vm_address_t currentOffset = moduleBase + sizeof(header);
        
        // Skip command headers and calculate size
        for (uint32_t i = 0; i < header.ncmds; i++) {
            struct load_command cmd;
            if (!ReadMemory(currentOffset, &cmd, sizeof(cmd))) {
                break;
            }
            
            if (cmd.cmd == LC_SEGMENT_64) {
                struct segment_command_64 segCmd;
                if (ReadMemory(currentOffset, &segCmd, sizeof(segCmd))) {
                    totalSize += segCmd.vmsize;
                }
            }
            
            currentOffset += cmd.cmdsize;
        }
        
        // Add to cache
        if (totalSize > 0) {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            m_moduleSizeCache[moduleBase] = totalSize;
        }
        
        return totalSize;
    }
    
    mach_vm_address_t MemoryAccess::FindPattern(mach_vm_address_t rangeStart, size_t rangeSize, 
                                              const std::string& pattern, const std::string& mask) {
        // Validate inputs
        if (rangeStart == 0 || rangeSize == 0 || pattern.empty() || mask.empty() || pattern.size() != mask.size()) {
            return 0;
        }
        
        // Convert pattern string to bytes before reading memory
        std::vector<uint8_t> patternBytes;
        std::istringstream patternStream(pattern);
        std::string byteStr;
        
        while (std::getline(patternStream, byteStr, ' ')) {
            if (byteStr.length() == 2) {
                patternBytes.push_back(static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16)));
            } else {
                patternBytes.push_back(0);
            }
        }
        
        // Allocate buffer for the memory region
        std::vector<uint8_t> buffer(rangeSize);
        
        // Read the memory region
        if (!ReadMemory(rangeStart, buffer.data(), rangeSize)) {
            return 0;
        }
        
        // Use Boyer-Moore algorithm for faster searching
        size_t patternLen = patternBytes.size();
        
        // Create bad character table for Boyer-Moore
        int badChar[256];
        for (int i = 0; i < 256; i++) {
            badChar[i] = patternLen;
        }
        
        for (size_t i = 0; i < patternLen - 1; i++) {
            badChar[patternBytes[i]] = patternLen - i - 1;
        }
        
        // Start the search
        size_t offset = 0;
        while (offset <= buffer.size() - patternLen) {
            size_t j = patternLen - 1;
            
            // Match from right to left
            while (j < patternLen && (mask[j] == '?' || buffer[offset + j] == patternBytes[j])) {
                j--;
            }
            
            if (j >= patternLen) {
                // Match found
                return rangeStart + offset;
            }
            
            // Shift by bad character rule if we have a mismatch
            offset += (mask[j] == '?') ? 1 : std::max(1, static_cast<int>(badChar[buffer[offset + j]] - (patternLen - 1 - j)));
        }
        
        return 0;
    }
    
    mach_vm_address_t MemoryAccess::ScanForPattern(const std::string& pattern, const std::string& mask) {
        // Create a unique key for this pattern
        std::string cacheKey = pattern + ":" + mask;
        
        // Check cache first
        {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            auto it = m_patternCache.find(cacheKey);
            if (it != m_patternCache.end()) {
                return it->second;
            }
        }
        
        // Not in cache, so scan for it
        if (!m_initialized && !Initialize()) {
            return 0;
        }
        
        // Ensure we have region information
        if (m_cachedReadableRegions.empty() || 
            GetCurrentTimestamp() - m_regionsLastUpdated > 30000) { // 30 seconds
            RefreshMemoryRegions();
        }
        
        // Scan each readable region
        mach_vm_address_t result = 0;
        
        for (const auto& region : m_cachedReadableRegions) {
            mach_vm_address_t start = region.first;
            mach_vm_address_t end = region.second;
            size_t size = end - start;
            
            // Use a maximum chunk size to avoid excessive memory usage
            const size_t MAX_CHUNK_SIZE = 4 * 1024 * 1024; // 4MB
            
            // Scan the region in smaller chunks if it's large
            if (size > MAX_CHUNK_SIZE) {
                for (mach_vm_address_t chunkStart = start; chunkStart < end; chunkStart += MAX_CHUNK_SIZE) {
                    size_t chunkSize = std::min(MAX_CHUNK_SIZE, static_cast<size_t>(end - chunkStart));
                    result = FindPattern(chunkStart, chunkSize, pattern, mask);
                    if (result != 0) {
                        break;
                    }
                }
            } else {
                result = FindPattern(start, size, pattern, mask);
            }
            
            if (result != 0) {
                break;
            }
        }
        
        // Add to cache if found
        if (result != 0) {
            std::lock_guard<std::mutex> lock(m_cacheMutex);
            m_patternCache[cacheKey] = result;
        }
        
        return result;
    }
    
    void MemoryAccess::ClearCache() {
        std::lock_guard<std::mutex> lock(m_cacheMutex);
        m_patternCache.clear();
        m_moduleBaseCache.clear();
        m_moduleSizeCache.clear();
        m_cachedReadableRegions.clear();
        m_regionsLastUpdated = 0;
    }
    
    void MemoryAccess::Cleanup() {
        std::lock_guard<std::mutex> lock(m_accessMutex);
        if (m_initialized && m_targetTask != MACH_PORT_NULL) {
            m_targetTask = MACH_PORT_NULL;
            m_initialized = false;
            ClearCache();
        }
    }
}
