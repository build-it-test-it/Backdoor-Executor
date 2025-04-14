#pragma once

// Define CI_BUILD for CI environments
#define CI_BUILD

#include <string>
#include <vector>
#include <cstdint>
#include <atomic>
#include <iostream>

// Include mach_compat.h to get mach_vm_address_t
#include "mach_compat.h"

namespace iOS {
    // Forward declarations of classes we'll use as stubs
    class PatternScanner {
    public:
        // Simple enum for scan modes
        enum class ScanMode {
            Normal, Fast, LowMemory, Stealth
        };
        
        // Simple enum for confidence levels
        enum class MatchConfidence {
            Exact, High, Medium, Low
        };
        
        // Simple result structure
        struct ScanResult {
            mach_vm_address_t m_address;
            std::string m_moduleName;
            size_t m_offset;
            
            ScanResult() : m_address(0), m_moduleName(""), m_offset(0) {}
            
            ScanResult(mach_vm_address_t address, const std::string& moduleName, size_t offset) 
                : m_address(address), m_moduleName(moduleName), m_offset(offset) {}
            
            bool IsValid() const { return m_address != 0; }
        };
        
        // Constructor
        PatternScanner() {
            std::cout << "PatternScanner::Constructor - CI stub" << std::endl;
        }
        
        // Required instance methods
        ScanResult FindPattern(const std::string& pattern) {
            std::cout << "PatternScanner::FindPattern - CI stub" << std::endl;
            return ScanResult(0x1000, "CIStub", 0);
        }
        
        uintptr_t GetModuleBase(const std::string& moduleName) {
            std::cout << "PatternScanner::GetModuleBase - CI stub" << std::endl;
            return 0x10000000;
        }
        
        // Static methods
        static bool Initialize() {
            std::cout << "PatternScanner::Initialize - CI stub" << std::endl;
            return true;
        }
        
        static void SetScanMode(ScanMode mode) {
            std::cout << "PatternScanner::SetScanMode - CI stub" << std::endl;
        }
        
        static ScanMode GetScanMode() {
            return ScanMode::Normal;
        }
        
        static bool StringToPattern(const std::string& patternStr, 
                                   std::vector<uint8_t>& outBytes, 
                                   std::string& outMask) {
            outBytes = {0x90, 0x90, 0x90}; // NOP, NOP, NOP as stub
            outMask = "xxx";
            return true;
        }
        
        static ScanResult FindPatternInModule(
            const std::string& moduleName, 
            const std::string& patternStr,
            MatchConfidence minConfidence = MatchConfidence::Exact) {
            
            std::cout << "FindPatternInModule - CI stub for " << moduleName << std::endl;
            return ScanResult(0x10001000, moduleName, 0x1000);
        }
    };
}
