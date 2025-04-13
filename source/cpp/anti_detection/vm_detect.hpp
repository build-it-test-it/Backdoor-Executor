#pragma once

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/utsname.h>

namespace AntiDetection {
    
    class VMDetection {
    private:
        // Check if a file exists
        static bool FileExists(const char* filename) {
            return access(filename, F_OK) != -1;
        }
        
        // Read a file into a string
        static std::string ReadFile(const char* filename) {
            FILE* file = fopen(filename, "r");
            if (!file) return "";
            
            fseek(file, 0, SEEK_END);
            long size = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            std::string result;
            result.resize(size);
            fread(&result[0], 1, size, file);
            fclose(file);
            
            return result;
        }
        
    public:
        // Main VM detection function that combines multiple techniques
        static bool DetectVM() {
            return CheckVMFiles() || CheckCPUInfo() || CheckDMI() || CheckHypervisorPresence();
        }
        
        // Check for VM-specific files
        static bool CheckVMFiles() {
            const char* vmFiles[] = {
                "/sys/class/dmi/id/product_name",  // Often contains "Virtual" or "VMware" for VMs
                "/sys/hypervisor/uuid",            // Only exists in VMs
                "/proc/scsi/scsi",                 // May contain VM-specific strings
                "/proc/ide/hd*/model"              // May show VM disks like "VBOX HARDDISK"
            };
            
            for (const char* file : vmFiles) {
                if (FileExists(file)) {
                    std::string content = ReadFile(file);
                    if (content.find("VMware") != std::string::npos ||
                        content.find("VBOX") != std::string::npos ||
                        content.find("Virtual") != std::string::npos ||
                        content.find("QEMU") != std::string::npos) {
                        return true;
                    }
                }
            }
            
            return false;
        }
        
        // Check CPU info for VM indicators
        static bool CheckCPUInfo() {
            std::string cpuInfo = ReadFile("/proc/cpuinfo");
            
            if (cpuInfo.empty()) return false;
            
            // Check for hypervisor flag which is present in VMs
            if (cpuInfo.find("hypervisor") != std::string::npos) {
                return true;
            }
            
            // Check for VM-specific CPU model names
            if (cpuInfo.find("QEMU") != std::string::npos ||
                cpuInfo.find("KVM") != std::string::npos ||
                cpuInfo.find("VMware") != std::string::npos) {
                return true;
            }
            
            return false;
        }
        
        // Check DMI (Desktop Management Interface) for VM indicators
        static bool CheckDMI() {
            const char* dmiFiles[] = {
                "/sys/class/dmi/id/sys_vendor",
                "/sys/class/dmi/id/board_vendor",
                "/sys/class/dmi/id/bios_vendor"
            };
            
            for (const char* file : dmiFiles) {
                if (FileExists(file)) {
                    std::string content = ReadFile(file);
                    if (content.find("VMware") != std::string::npos ||
                        content.find("QEMU") != std::string::npos ||
                        content.find("VirtualBox") != std::string::npos ||
                        content.find("innotek") != std::string::npos) {  // VirtualBox
                        return true;
                    }
                }
            }
            
            return false;
        }
        
        // Check for hypervisor presence
        static bool CheckHypervisorPresence() {
            struct utsname systemInfo;
            if (uname(&systemInfo) != 0) {
                return false;
            }
            
            // Check for VM-specific strings in system info
            if (strstr(systemInfo.version, "hypervisor") ||
                strstr(systemInfo.version, "vbox") ||
                strstr(systemInfo.version, "vmware")) {
                return true;
            }
            
            return false;
        }
        
        // Takes appropriate actions based on VM detection
        static void HandleVMDetection() {
            if (DetectVM()) {
                // In a real implementation, you might:
                // 1. Subtly alter program behavior to confuse analysis
                // 2. Inject false positives in key functionality
                // 3. Introduce non-deterministic delays
                // 4. Gradually degrade performance
                
                // Instead of outright crashing or refusing to run, which would
                // make your countermeasures obvious to analysts, subtly alter behavior
                
                // For demonstration, we'll just log the detection
                fprintf(stderr, "VM environment detected, enabling countermeasures\n");
            }
        }
    };
}
