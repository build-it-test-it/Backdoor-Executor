#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <atomic>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include <future>
#include <memory>
#include <queue>
#include <deque>
#include <chrono>

// Include MemoryAccess.h first as it contains the mach_vm typedefs and compatibility wrappers
#include "MemoryAccess.h"

namespace iOS {
    /**
     * @class PatternScanner
     * @brief High-performance pattern scanner specialized for ARM64 architecture on iOS
     * 
     * This class provides advanced pattern scanning functionality optimized for ARM64
     * instruction patterns and iOS memory layout. It uses sophisticated algorithms and
     * parallel processing to efficiently scan large memory regions.
     * 
     * Features:
     * - Thread-safe implementation with intelligent caching
     * - Advanced Boyer-Moore-Horspool algorithm with SIMD acceleration
     * - Adaptive multi-threaded scanning with work-stealing scheduler
     * - Memory-efficient chunk-based scanning with memory pooling
     * - Comprehensive ARM64 instruction parsing and pattern analysis
     * - Automatic tuning based on device capabilities
     * - Support for fuzzy pattern matching with similarity thresholds
     */
    class PatternScanner {
    public:
        // Scan modes for different performance profiles
        enum class ScanMode {
            Normal,     // Default balance of performance and memory usage
            Fast,       // Prioritize speed over memory usage
            LowMemory,  // Prioritize low memory usage over speed
            Stealth     // Avoid detection by hiding memory access patterns
        };
        
        // Pattern match confidence levels
        enum class MatchConfidence {
            Exact,      // Pattern matches exactly
            High,       // Pattern matches with high confidence (>90%)
            Medium,     // Pattern matches with medium confidence (>70%)
            Low         // Pattern matches with low confidence (>50%)
        };
        
        /**
         * @struct ScanResult
         * @brief Comprehensive result of a pattern scan with detailed metadata
         */
        struct ScanResult {
            mach_vm_address_t m_address;      // The address where the pattern was found
            std::string m_moduleName;         // The module name containing the pattern
            size_t m_offset;                  // Offset from module base address
            MatchConfidence m_confidence;     // Confidence level of the match
            uint64_t m_scanTime;              // Time taken to find this result in microseconds
            std::vector<uint8_t> m_context;   // Memory context surrounding the match (optional)
            
            ScanResult() 
                : m_address(0), m_moduleName(""), m_offset(0), 
                  m_confidence(MatchConfidence::Exact), m_scanTime(0) {}
            
            ScanResult(mach_vm_address_t address, const std::string& moduleName, size_t offset,
                      MatchConfidence confidence = MatchConfidence::Exact, uint64_t scanTime = 0) 
                : m_address(address), m_moduleName(moduleName), m_offset(offset),
                  m_confidence(confidence), m_scanTime(scanTime) {}
            
            bool IsValid() const { return m_address != 0; }
            
            // Helper for sorting results by confidence
            bool IsBetterThan(const ScanResult& other) const {
                // First compare by confidence level
                if (m_confidence != other.m_confidence) {
                    return static_cast<int>(m_confidence) < static_cast<int>(other.m_confidence);
                }
                // Then by module name (main module preferred)
                if (m_moduleName != other.m_moduleName) {
                    // Main module is preferred
                    if (m_moduleName == "RobloxPlayer") return true;
                    if (other.m_moduleName == "RobloxPlayer") return false;
                }
                // Finally by address (prefer lower addresses)
                return m_address < other.m_address;
            }
        };
        
    private:
        // Thread pool for parallel scanning
        class ScannerThreadPool {
        private:
            std::vector<std::thread> m_threads;
            std::queue<std::function<void()>> m_tasks;
            std::mutex m_queueMutex;
            std::condition_variable m_condition;
            std::atomic<bool> m_stop;
            std::atomic<uint32_t> m_activeThreads;
            uint32_t m_threadCount;
            
        public:
            ScannerThreadPool(uint32_t numThreads = 0) 
                : m_stop(false), m_activeThreads(0) {
                // Auto-detect number of threads if not specified
                m_threadCount = (numThreads > 0) ? numThreads : std::thread::hardware_concurrency();
                // Always use at least 1 thread, but no more than 8
                m_threadCount = std::max(1u, std::min(8u, m_threadCount));
                
                for (uint32_t i = 0; i < m_threadCount; ++i) {
                    m_threads.emplace_back([this] {
                        while (true) {
                            std::function<void()> task;
                            {
                                std::unique_lock<std::mutex> lock(m_queueMutex);
                                m_condition.wait(lock, [this] { 
                                    return m_stop || !m_tasks.empty(); 
                                });
                                
                                if (m_stop && m_tasks.empty()) return;
                                
                                task = std::move(m_tasks.front());
                                m_tasks.pop();
                            }
                            
                            m_activeThreads++;
                            task();
                            m_activeThreads--;
                        }
                    });
                }
            }
            
            ~ScannerThreadPool() {
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    m_stop = true;
                }
                
                m_condition.notify_all();
                for (auto& thread : m_threads) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
            }
            
            template<typename F, typename... Args>
            auto Enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
                using ReturnType = decltype(f(args...));
                auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );
                
                std::future<ReturnType> result = task->get_future();
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    if (m_stop) {
                        throw std::runtime_error("Thread pool is stopping");
                    }
                    
                    m_tasks.emplace([task] { (*task)(); });
                }
                
                m_condition.notify_one();
                return result;
            }
            
            uint32_t GetActiveThreadCount() const {
                return m_activeThreads;
            }
            
            uint32_t GetThreadCount() const {
                return m_threadCount;
            }
            
            uint32_t GetQueueSize() {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                return static_cast<uint32_t>(m_tasks.size());
            }
        };
        
        // Memory chunk pool for efficient reuse
        class MemoryChunkPool {
        private:
            static constexpr size_t DEFAULT_CHUNK_SIZE = 4 * 1024 * 1024; // 4 MB
            static constexpr size_t MAX_POOLED_CHUNKS = 8;  // Maximum number of pooled chunks
            
            std::mutex m_poolMutex;
            std::deque<std::vector<uint8_t>> m_pool;
            size_t m_chunkSize;
            
        public:
            MemoryChunkPool(size_t chunkSize = DEFAULT_CHUNK_SIZE) 
                : m_chunkSize(chunkSize) {}
            
            ~MemoryChunkPool() {
                Clear();
            }
            
            std::vector<uint8_t> GetChunk() {
                std::lock_guard<std::mutex> lock(m_poolMutex);
                if (!m_pool.empty()) {
                    auto chunk = std::move(m_pool.front());
                    m_pool.pop_front();
                    return chunk;
                }
                
                // Create a new chunk if the pool is empty
                return std::vector<uint8_t>(m_chunkSize);
            }
            
            void ReturnChunk(std::vector<uint8_t>&& chunk) {
                std::lock_guard<std::mutex> lock(m_poolMutex);
                if (m_pool.size() < MAX_POOLED_CHUNKS) {
                    m_pool.push_back(std::move(chunk));
                }
                // If the pool is full, the chunk will be deallocated
            }
            
            void Clear() {
                std::lock_guard<std::mutex> lock(m_poolMutex);
                m_pool.clear();
            }
            
            size_t GetPoolSize() const {
                std::lock_guard<std::mutex> lock(m_poolMutex);
                return m_pool.size();
            }
        };
        
        // Constants
        static const size_t ARM64_INSTRUCTION_SIZE = 4; // ARM64 instructions are 4 bytes
        static constexpr size_t SCAN_CHUNK_SIZE = 1024 * 1024; // 1 MB chunks for scanning
        static constexpr size_t MAX_CACHE_ENTRIES = 128; // Maximum number of cached patterns
        static constexpr uint64_t CACHE_EXPIRY_TIME = 60000; // 60 seconds cache lifetime
        
        // Static member variables
        static ScannerThreadPool s_threadPool;
        static MemoryChunkPool s_chunkPool;
        static std::atomic<bool> s_useParallelScanning;
        static std::atomic<ScanMode> s_scanMode;
        static std::mutex s_cacheMutex;
        
        // Cache structures
        struct CacheEntry {
            ScanResult result;
            uint64_t timestamp;
            
            CacheEntry(const ScanResult& r) 
                : result(r), timestamp(GetCurrentTimestamp()) {}
        };
        
        static std::unordered_map<std::string, CacheEntry> s_patternCache;
        static std::unordered_map<std::string, std::vector<CacheEntry>> s_multiPatternCache;
        static std::unordered_map<std::string, CacheEntry> s_stringRefCache;
        
        // Helper methods
        static uint64_t GetCurrentTimestamp() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
        }
        
        static void PruneExpiredCacheEntries() {
            std::lock_guard<std::mutex> lock(s_cacheMutex);
            uint64_t now = GetCurrentTimestamp();
            
            // Prune single pattern cache
            for (auto it = s_patternCache.begin(); it != s_patternCache.end();) {
                if (now - it->second.timestamp > CACHE_EXPIRY_TIME) {
                    it = s_patternCache.erase(it);
                } else {
                    ++it;
                }
            }
            
            // Prune multi-pattern cache
            for (auto it = s_multiPatternCache.begin(); it != s_multiPatternCache.end();) {
                bool anyExpired = false;
                for (auto entryIt = it->second.begin(); entryIt != it->second.end();) {
                    if (now - entryIt->timestamp > CACHE_EXPIRY_TIME) {
                        entryIt = it->second.erase(entryIt);
                        anyExpired = true;
                    } else {
                        ++entryIt;
                    }
                }
                
                if (it->second.empty()) {
                    it = s_multiPatternCache.erase(it);
                } else {
                    ++it;
                }
            }
            
            // Prune string reference cache
            for (auto it = s_stringRefCache.begin(); it != s_stringRefCache.end();) {
                if (now - it->second.timestamp > CACHE_EXPIRY_TIME) {
                    it = s_stringRefCache.erase(it);
                } else {
                    ++it;
                }
            }
        }
        
        static bool IsCacheValid() {
            // Check if cache is still valid (e.g., process hasn't been updated)
            return true; // Simplified implementation
        }
        
        // Enhanced scanning methods
        static mach_vm_address_t ScanChunkInternal(
            mach_vm_address_t startAddress, const uint8_t* buffer, size_t bufferSize,
            const std::vector<uint8_t>& pattern, const std::string& mask,
            MatchConfidence minConfidence);
        
        static std::vector<mach_vm_address_t> ScanChunkForMultipleMatches(
            mach_vm_address_t startAddress, const uint8_t* buffer, size_t bufferSize,
            const std::vector<uint8_t>& pattern, const std::string& mask,
            MatchConfidence minConfidence, size_t maxMatches);
        
    public:
        /**
         * @brief Initialize the pattern scanner
         * @param scanMode Initial scan mode (default: Normal)
         * @param parallelThreads Number of threads for parallel scanning (0 = auto)
         * @return True if initialization succeeded
         */
        static bool Initialize(ScanMode scanMode = ScanMode::Normal, uint32_t parallelThreads = 0);
        
        /**
         * @brief Set the scan mode for subsequent operations
         * @param mode New scan mode
         */
        static void SetScanMode(ScanMode mode);
        
        /**
         * @brief Get the current scan mode
         * @return Current scan mode
         */
        static ScanMode GetScanMode();
        
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
         * @param minConfidence Minimum confidence level for matches
         * @return ScanResult containing the found address and metadata, or invalid result if not found
         * 
         * Enhanced with adaptive multi-threaded scanning and intelligent caching
         */
        static ScanResult FindPatternInModule(
            const std::string& moduleName, 
            const std::string& patternStr,
            MatchConfidence minConfidence = MatchConfidence::Exact);
        
        /**
         * @brief Find a pattern in memory within Roblox's main module
         * @param patternStr Pattern string with wildcards (e.g., "48 8B ? ? 90")
         * @param minConfidence Minimum confidence level for matches
         * @return ScanResult containing the found address and metadata, or invalid result if not found
         */
        static ScanResult FindPatternInRoblox(
            const std::string& patternStr,
            MatchConfidence minConfidence = MatchConfidence::Exact);
        
        /**
         * @brief Find all occurrences of a pattern in a specific module
         * @param moduleName Name of the module to scan
         * @param patternStr Pattern string with wildcards (e.g., "48 8B ? ? 90")
         * @param minConfidence Minimum confidence level for matches
         * @param maxMatches Maximum number of matches to return (0 = unlimited)
         * @return Vector of ScanResults for all occurrences
         * 
         * Enhanced with chunk-based parallel scanning and memory-efficient processing
         */
        static std::vector<ScanResult> FindAllPatternsInModule(
            const std::string& moduleName, 
            const std::string& patternStr,
            MatchConfidence minConfidence = MatchConfidence::Exact,
            size_t maxMatches = 0);
        
        /**
         * @brief Find patterns across multiple modules
         * @param patternStr Pattern string with wildcards
         * @param modules Vector of module names to scan (empty = all modules)
         * @param minConfidence Minimum confidence level for matches
         * @return Vector of ScanResults for all occurrences across modules
         */
        static std::vector<ScanResult> FindPatternInAllModules(
            const std::string& patternStr,
            const std::vector<std::string>& modules = {},
            MatchConfidence minConfidence = MatchConfidence::Exact);
        
        /**
         * @brief Resolve an ARM64 branch instruction's target address
         * @param instructionAddress Address of the branch instruction
         * @return Target address the instruction branches to, or 0 if invalid
         * 
         * Supports B, BL, CBZ, CBNZ, TBZ, and TBNZ instruction types
         */
        static mach_vm_address_t ResolveBranchTarget(mach_vm_address_t instructionAddress);
        
        /**
         * @brief Resolve an ARM64 ADRP+ADD/LDR sequence to get the target address
         * @param adrpInstructionAddress Address of the ADRP instruction
         * @param nextInstructionOffset Offset to the next instruction (ADD or LDR)
         * @return Target address calculated from the instruction sequence, or 0 if invalid
         * 
         * Enhanced to support all ARM64 addressing modes including ADRP+ADD, ADRP+LDR, and more
         */
        static mach_vm_address_t ResolveAdrpSequence(
            mach_vm_address_t adrpInstructionAddress, 
            size_t nextInstructionOffset = ARM64_INSTRUCTION_SIZE);
        
        /**
         * @brief Analyze a function to find all its references
         * @param functionAddress Start address of the function
         * @param maxReferencesToFind Maximum number of references to find (0 = unlimited)
         * @return Vector of addresses that reference the function
         * 
         * This method scans memory for any instructions that might reference the given function
         */
        static std::vector<mach_vm_address_t> FindFunctionReferences(
            mach_vm_address_t functionAddress,
            size_t maxReferencesToFind = 0);
        
        /**
         * @brief Find a reference to a string in the module
         * @param moduleName Name of the module to scan
         * @param str String to find references to
         * @param exactMatch Whether to match the string exactly or as a substring
         * @return ScanResult containing the address of a reference to the string
         * 
         * Enhanced with multi-threaded scanning and advanced string matching
         */
        static ScanResult FindStringReference(
            const std::string& moduleName, 
            const std::string& str,
            bool exactMatch = true);
        
        /**
         * @brief Find all references to a string in all modules
         * @param str String to find references to
         * @param exactMatch Whether to match the string exactly or as a substring
         * @return Vector of ScanResults for all string references
         */
        static std::vector<ScanResult> FindAllStringReferences(
            const std::string& str,
            bool exactMatch = true);
        
        /**
         * @brief Find the address of a specific imported function
         * @param moduleName Name of the module to check
         * @param importName Name of the imported function
         * @return Address of the import or 0 if not found
         */
        static mach_vm_address_t FindImportedFunction(
            const std::string& moduleName,
            const std::string& importName);
        
        /**
         * @brief Find the address of a specific exported function
         * @param moduleName Name of the module to check
         * @param exportName Name of the exported function
         * @return Address of the export or 0 if not found
         */
        static mach_vm_address_t FindExportedFunction(
            const std::string& moduleName,
            const std::string& exportName);
        
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
         * @brief Get the number of threads available for parallel scanning
         * @return Number of threads in the thread pool
         */
        static uint32_t GetThreadCount();
        
        /**
         * @brief Get the number of patterns currently cached
         * @return Total number of cached patterns
         */
        static size_t GetCacheSize();
        
        /**
         * @brief Clear all pattern caches
         * 
         * This is useful when memory has been modified and cached results may be invalid,
         * or to free up memory. Call this after major memory operations like module loading.
         */
        static void ClearCache();
        
        /**
         * @brief Release unused memory resources
         * 
         * This frees memory used by the scanner's internal pools and caches.
         * Call this during low memory conditions or when the scanner will not be used for a while.
         */
        static void ReleaseResources();
    };
    
    /**
     * @brief Advanced memory scanner using optimized Boyer-Moore-Horspool algorithm
     * @param haystack Buffer to scan
     * @param haystackSize Size of the buffer
     * @param needle Pattern to find
     * @param mask Mask for the pattern ('x' for match, '?' for wildcard)
     * @param similarityThreshold Minimum similarity threshold (0.0-1.0) for fuzzy matching
     * @return Offset where the pattern was found, or 0 if not found
     */
    mach_vm_address_t ScanWithBoyerMooreHorspool(
        const uint8_t* haystack, size_t haystackSize,
        const std::vector<uint8_t>& needle, const std::string& mask,
        float similarityThreshold = 1.0f);
    
    /**
     * @brief High-performance parallel pattern scanner with work stealing
     * @param startAddress Base address of the memory region
     * @param buffer Buffer containing the memory data
     * @param bufferSize Size of the buffer
     * @param pattern Pattern to find
     * @param mask Mask for the pattern ('x' for match, '?' for wildcard)
     * @param maxMatches Maximum number of matches to find (0 = unlimited)
     * @param similarityThreshold Minimum similarity threshold (0.0-1.0) for fuzzy matching
     * @return Vector of addresses where the pattern was found
     */
    std::vector<mach_vm_address_t> ScanMemoryRegionParallel(
        mach_vm_address_t startAddress, const uint8_t* buffer, size_t bufferSize,
        const std::vector<uint8_t>& pattern, const std::string& mask,
        size_t maxMatches = 1, float similarityThreshold = 1.0f);
    
    /**
     * @brief Memory-efficient sequential scanner for resource-constrained environments
     * @param startAddress Base address of the memory region
     * @param buffer Buffer containing the memory data
     * @param bufferSize Size of the buffer
     * @param pattern Pattern to find
     * @param mask Mask for the pattern ('x' for match, '?' for wildcard)
     * @param maxMatches Maximum number of matches to find (0 = unlimited)
     * @param similarityThreshold Minimum similarity threshold (0.0-1.0) for fuzzy matching
     * @return Vector of addresses where the pattern was found
     */
    std::vector<mach_vm_address_t> ScanMemoryRegionSequential(
        mach_vm_address_t startAddress, const uint8_t* buffer, size_t bufferSize,
        const std::vector<uint8_t>& pattern, const std::string& mask,
        size_t maxMatches = 1, float similarityThreshold = 1.0f);
}
