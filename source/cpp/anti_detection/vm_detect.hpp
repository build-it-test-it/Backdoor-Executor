#pragma once

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <sys/utsname.h>
#else
#include <sys/utsname.h>
#endif

namespace AntiDetection {
    
    class VMDetection {
    private:
        // Check if a file exists - platform independent
        static bool FileExists(const char* filename) {
            return access(filename, F_OK) != -1;
        }
        
        // Read a file into a string - platform independent
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
#ifdef __APPLE__
            return CheckIOSVM();
#else
            return CheckVMFiles() || CheckCPUInfo() || CheckDMI() || CheckHypervisorPresence();
#endif
        }

#ifdef __APPLE__
        // iOS-specific VM detection
        static bool CheckIOSVM() {
            // Check for simulator
            bool isSimulator = false;
            
            // Check for simulator-specific files
            if (FileExists("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform") ||
                FileExists("/Library/Developer/CoreSimulator")) {
                return true;
            }
            
            // Check system version
            struct utsname systemInfo;
            if (uname(&systemInfo) == 0) {
                if (strstr(systemInfo.machine, "x86_64") || 
                    strstr(systemInfo.machine, "i386")) {
                    // iOS devices don't use x86 architecture
                    return true;
                }
            }
            
            // Check sysctl values
            int mib[2] = { CTL_HW, HW_MODEL };
            char model[256];
            size_t len = sizeof(model);
            if (sysctl(mib, 2, model, &len, NULL, 0) == 0) {
                if (strstr(model, "Simulator")) {
                    return true;
                }
            }
            
            // Check environment variables specific to simulators
            if (getenv("SIMULATOR_DEVICE_NAME") || 
                getenv("SIMULATOR_UDID") ||
                getenv("SIMULATOR_ROOT")) {
                return true;
            }
            
            return isSimulator;
        }
#else
        // Linux/Android specific VM file checks
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
        
        // Linux/Android CPU info check
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
        
        // Linux/Android DMI check
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
        
        // Linux/Android hypervisor check via uname
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
#endif
        
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
                std::cerr << "VM environment detected, enabling countermeasures" << std::endl;
            }
        }
    };
}
