#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

namespace AntiDetection {
    /**
     * @class AntiDebug
     * @brief Advanced anti-debugging techniques to prevent analysis
     * 
     * This class implements multiple anti-debugging techniques to prevent
     * reverse engineering and analysis of the executor.
     */
    class AntiDebug {
    private:
        // Timing check state
        static std::atomic<bool> s_timingCheckActive;
        static std::mutex s_timingMutex;
        static std::chrono::high_resolution_clock::time_point s_lastCheckTime;
        
        // Random number generator
        static std::mt19937& GetRNG() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            return gen;
        }
        
        // Check if being debugged using platform-specific methods
        static bool IsBeingDebugged() {
#ifdef _WIN32
            // Windows-specific debug detection
            if (IsDebuggerPresent()) {
                return true;
            }
            
            // Check for remote debugger
            BOOL isRemoteDebuggerPresent = FALSE;
            CheckRemoteDebuggerPresent(GetCurrentProcess(), &isRemoteDebuggerPresent);
            if (isRemoteDebuggerPresent) {
                return true;
            }
            
            // Check for hardware breakpoints
            CONTEXT ctx = {};
            ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
            if (GetThreadContext(GetCurrentThread(), &ctx)) {
                if (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0) {
                    return true;
                }
            }
            
            return false;
#else
            // Unix-based debug detection using ptrace
            if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
                return true;
            }
            
            // Detach after check
            ptrace(PTRACE_DETACH, 0, 1, 0);
            
            return false;
#endif
        }
        
        // Check for timing anomalies that might indicate debugging
        static bool DetectTimingAnomalies() {
            std::lock_guard<std::mutex> lock(s_timingMutex);
            
            auto now = std::chrono::high_resolution_clock::now();
            
            if (s_lastCheckTime.time_since_epoch().count() > 0) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - s_lastCheckTime).count();
                
                // If the time between checks is suspiciously long, it might indicate a debugger
                if (elapsed > 5000) { // 5 seconds threshold
                    s_lastCheckTime = now;
                    return true;
                }
            }
            
            s_lastCheckTime = now;
            return false;
        }
        
        // Check for known debugging tools in the process list
#ifdef _WIN32
        static bool DetectDebuggerProcesses() {
            const std::vector<std::wstring> debuggerProcesses = {
                L"ollydbg.exe", L"ida.exe", L"ida64.exe", L"idag.exe", L"idag64.exe",
                L"idaw.exe", L"idaw64.exe", L"idaq.exe", L"idaq64.exe", L"idau.exe",
                L"idau64.exe", L"scylla.exe", L"protection_id.exe", L"x64dbg.exe",
                L"x32dbg.exe", L"windbg.exe", L"reshacker.exe", L"ImportREC.exe",
                L"IMMUNITYDEBUGGER.EXE", L"devenv.exe"
            };
            
            HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hSnapshot == INVALID_HANDLE_VALUE) {
                return false;
            }
            
            PROCESSENTRY32W pe32 = {};
            pe32.dwSize = sizeof(PROCESSENTRY32W);
            
            if (Process32FirstW(hSnapshot, &pe32)) {
                do {
                    for (const auto& debugger : debuggerProcesses) {
                        if (_wcsicmp(pe32.szExeFile, debugger.c_str()) == 0) {
                            CloseHandle(hSnapshot);
                            return true;
                        }
                    }
                } while (Process32NextW(hSnapshot, &pe32));
            }
            
            CloseHandle(hSnapshot);
            return false;
        }
#else
        static bool DetectDebuggerProcesses() {
            // On Unix systems, we could parse /proc to look for debuggers
            // This is a simplified implementation
            return false;
        }
#endif
        
        // Apply anti-tampering measures to the code
        static void ApplyCodeIntegrityChecks() {
            // In a real implementation, this would calculate checksums of critical code sections
            // and verify they haven't been modified
            
            // For demonstration purposes, we'll just add some timing checks
            std::thread([]{
                while (s_timingCheckActive) {
                    if (DetectTimingAnomalies() || IsBeingDebugged()) {
                        // In a real implementation, you might take evasive action here
                        // For now, we'll just sleep to introduce unpredictable behavior
                        std::uniform_int_distribution<> dist(100, 500);
                        std::this_thread::sleep_for(std::chrono::milliseconds(dist(GetRNG())));
                    }
                    
                    // Random sleep to make timing analysis harder
                    std::uniform_int_distribution<> dist(500, 2000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(dist(GetRNG())));
                }
            }).detach();
        }
        
    public:
        // Initialize anti-debugging measures
        static void Initialize() {
            s_timingCheckActive = true;
            s_lastCheckTime = std::chrono::high_resolution_clock::now();
            
            // Start integrity checks in background
            ApplyCodeIntegrityChecks();
        }
        
        // Shutdown anti-debugging measures
        static void Shutdown() {
            s_timingCheckActive = false;
        }
        
        // Apply comprehensive anti-tampering measures
        static void ApplyAntiTamperingMeasures() {
            // Initialize if not already done
            static bool initialized = false;
            if (!initialized) {
                Initialize();
                initialized = true;
            }
            
            // Immediate checks
            if (IsBeingDebugged() || DetectDebuggerProcesses()) {
                // In a real implementation, you might take more drastic measures
                // For now, we'll just introduce random delays to confuse analysis
                std::uniform_int_distribution<> dist(50, 200);
                std::this_thread::sleep_for(std::chrono::milliseconds(dist(GetRNG())));
            }
            
            // Add some junk code that never executes but confuses static analysis
            if (false && IsBeingDebugged()) {
                volatile int x = 0;
                for (int i = 0; i < 1000000; i++) {
                    x += i;
                }
            }
        }
        
        // Check if the current environment is safe for execution
        static bool IsSafeEnvironment() {
            return !IsBeingDebugged() && !DetectDebuggerProcesses() && !DetectTimingAnomalies();
        }
    };
    
    // Initialize static members
    std::atomic<bool> AntiDebug::s_timingCheckActive(false);
    std::mutex AntiDebug::s_timingMutex;
    std::chrono::high_resolution_clock::time_point AntiDebug::s_lastCheckTime;
}
