#include "GameDetector.h"
#include "MemoryAccess.h"
#include "PatternScanner.h"
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <set>
#include <sstream>
#include <random>

namespace iOS {
    // Static caches to improve performance and reliability
    struct RobloxOffsets {
        mach_vm_address_t dataModelPtr = 0;             // Pointer to DataModel singleton
        mach_vm_address_t workspaceOffset = 0;          // Offset to Workspace from DataModel
        mach_vm_address_t playersServiceOffset = 0;     // Offset to Players service from DataModel
        mach_vm_address_t localPlayerOffset = 0;        // Offset to LocalPlayer from Players service
        mach_vm_address_t nameOffset = 0;               // Offset to Name property in Instance
        mach_vm_address_t gameNameOffset = 0;           // Offset to game name string
        mach_vm_address_t placeIdOffset = 0;            // Offset to place ID property
        mach_vm_address_t cameraOffset = 0;             // Offset to Camera from Workspace
        mach_vm_address_t characterOffset = 0;          // Offset to Character from Player
        mach_vm_address_t gameLoadingStatus = 0;        // Address to check loading status
        
        bool valid = false;                             // Whether offsets are valid
        uint64_t lastUpdated = 0;                       // When offsets were last updated
        
        bool NeedsUpdate() const {
            if (!valid) return true;
            
            // Update every 5 minutes or if any critical offset is invalid
            uint64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            
            return (now - lastUpdated > 300) || 
                   dataModelPtr == 0 || 
                   workspaceOffset == 0 || 
                   playersServiceOffset == 0;
        }
        
        void Reset() {
            valid = false;
            dataModelPtr = 0;
            workspaceOffset = 0;
            playersServiceOffset = 0;
            localPlayerOffset = 0;
            nameOffset = 0;
            gameNameOffset = 0;
            placeIdOffset = 0;
            cameraOffset = 0;
            characterOffset = 0;
            gameLoadingStatus = 0;
        }
    };
    
    static RobloxOffsets s_offsets;
    static std::mutex s_offsetsMutex;
    static std::unordered_map<std::string, mach_vm_address_t> s_serviceCache;
    static std::set<std::string> s_requiredServices = {
        "Workspace", "Players", "ReplicatedStorage", "Lighting", "CoreGui"
    };
    
    // Helper function to get current timestamp in seconds
    uint64_t GetCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    // Helper function to get current timestamp in milliseconds
    uint64_t GetCurrentTimestampMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    // Helper to read string from Roblox memory
    std::string ReadRobloxString(mach_vm_address_t stringPtr) {
        if (stringPtr == 0) return "";
        
        try {
            // In Roblox's memory layout, strings typically have:
            // - 4/8 byte length field (depending on architecture)
            // - Actual string data follows
            
            // Read the length field
            uint32_t length = 0;
            if (!MemoryAccess::ReadMemory(stringPtr, &length, sizeof(length))) {
                return "";
            }
            
            // Sanity check on length to prevent huge allocations
            if (length == 0 || length > 1024) {
                return "";
            }
            
            // Read the string data
            std::vector<char> buffer(length + 1, 0);
            if (!MemoryAccess::ReadMemory(stringPtr + sizeof(uint32_t), buffer.data(), length)) {
                return "";
            }
            
            return std::string(buffer.data(), length);
        } catch (...) {
            return "";
        }
    }
    
    // Constructor with enhanced initialization
    GameDetector::GameDetector()
        : m_currentState(GameState::Unknown),
          m_running(false),
          m_lastChecked(0),
          m_lastGameJoinTime(0),
          m_currentGameName(""),
          m_currentPlaceId("") {
        
        // Initialize the offsets
        std::lock_guard<std::mutex> lock(s_offsetsMutex);
        if (s_offsets.NeedsUpdate()) {
            s_offsets.Reset();
        }
    }
    
    // Destructor with enhanced cleanup
    GameDetector::~GameDetector() {
        Stop();
        
        // Clear any cached data
        std::lock_guard<std::mutex> lock(s_offsetsMutex);
        s_serviceCache.clear();
    }
    
    // Start detection thread with improved initialization
    bool GameDetector::Start() {
        if (m_running.load()) {
            return true; // Already running
        }
        
        // Initialize memory access if not already initialized
        if (!MemoryAccess::Initialize()) {
            std::cerr << "GameDetector: Failed to initialize memory access" << std::endl;
            return false;
        }
        
        // Find and update offsets asynchronously
        std::thread([this]() {
            UpdateRobloxOffsets();
        }).detach();
        
        // Set running flag
        m_running.store(true);
        
        // Start detection thread
        m_detectionThread = std::thread(&GameDetector::DetectionLoop, this);
        
        std::cout << "GameDetector: Started detection thread" << std::endl;
        return true;
    }
    
    // Stop detection thread with enhanced cleanup
    void GameDetector::Stop() {
        if (!m_running.load()) {
            return; // Not running
        }
        
        // Set running flag to false
        m_running.store(false);
        
        // Join thread if joinable
        if (m_detectionThread.joinable()) {
            m_detectionThread.join();
        }
        
        std::cout << "GameDetector: Stopped detection thread" << std::endl;
    }
    
    // Register a callback with improved ID handling
    size_t GameDetector::RegisterCallback(const StateChangeCallback& callback) {
        if (!callback) {
            return 0; // Invalid callback
        }
        
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        
        // Generate a unique ID using a random generator for better security
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<size_t> dist(1, UINT_MAX);
        
        size_t id = dist(gen);
        while (id == 0 || std::find_if(m_callbacks.begin(), m_callbacks.end(),
                                      [id](const auto& cb) { return cb.first == id; }) != m_callbacks.end()) {
            id = dist(gen);
        }
        
        // Store callback with ID
        m_callbacks.push_back(std::make_pair(id, callback));
        
        return id;
    }
    
    // Remove a registered callback with improved error handling
    bool GameDetector::RemoveCallback(size_t id) {
        if (id == 0) return false;
        
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        
        auto it = std::find_if(m_callbacks.begin(), m_callbacks.end(),
                             [id](const auto& cb) { return cb.first == id; });
        
        if (it != m_callbacks.end()) {
            m_callbacks.erase(it);
            return true;
        }
        
        return false;
    }
    
    // Get current game state
    GameDetector::GameState GameDetector::GetState() const {
        return m_currentState.load();
    }
    
    // Check if player is in a game
    bool GameDetector::IsInGame() const {
        return m_currentState.load() == GameState::InGame;
    }
    
    // Get current game name with enhanced safety
    std::string GameDetector::GetGameName() const {
        return m_currentGameName.empty() ? "Unknown Game" : m_currentGameName;
    }
    
    // Get current place ID with enhanced safety
    std::string GameDetector::GetPlaceId() const {
        return m_currentPlaceId.empty() ? "0" : m_currentPlaceId;
    }
    
    // Get time since player joined the game
    uint64_t GameDetector::GetTimeInGame() const {
        if (m_currentState.load() != GameState::InGame || m_lastGameJoinTime.load() == 0) {
            return 0;
        }
        
        return GetCurrentTimestamp() - m_lastGameJoinTime.load();
    }
    
    // Force a state update check with improved detection
    GameDetector::GameState GameDetector::ForceCheck() {
        // Check if Roblox is running
        bool robloxRunning = MemoryAccess::GetModuleBase("RobloxPlayer") != 0;
        
        if (!robloxRunning) {
            UpdateState(GameState::NotRunning);
            return m_currentState.load();
        }
        
        // Update offsets if needed
        {
            std::lock_guard<std::mutex> lock(s_offsetsMutex);
            if (s_offsets.NeedsUpdate()) {
                UpdateRobloxOffsets();
            }
        }
        
        // Do we need to detect loading state?
        bool isLoading = DetectLoadingState();
        if (isLoading) {
            UpdateState(GameState::Loading);
            return m_currentState.load();
        }
        
        // Check game objects for full in-game status
        bool inGame = CheckForGameObjects();
        
        // Update state based on check
        if (inGame) {
            if (m_currentState.load() != GameState::InGame) {
                UpdateState(GameState::InGame);
                UpdateGameInfo();
                m_lastGameJoinTime.store(GetCurrentTimestamp());
            }
        } else {
            // Handle state transitions based on current state
            if (m_currentState.load() == GameState::InGame) {
                UpdateState(GameState::Leaving);
                m_currentGameName = "";
                m_currentPlaceId = "";
                m_lastGameJoinTime.store(0);
            } else if (m_currentState.load() == GameState::Leaving) {
                UpdateState(GameState::Menu);
            } else if (m_currentState.load() == GameState::Unknown || 
                       m_currentState.load() == GameState::NotRunning) {
                UpdateState(GameState::Menu);
            }
        }
        
        return m_currentState.load();
    }
    
    // Main detection loop with adaptive timing
    void GameDetector::DetectionLoop() {
        // Adaptive check interval in milliseconds
        int checkIntervalMs = 1000; // Start with 1 second
        
        while (m_running.load()) {
            // Check if Roblox is running
            bool robloxRunning = MemoryAccess::GetModuleBase("RobloxPlayer") != 0;
            
            if (!robloxRunning) {
                // Update state to not running and use longer interval
                UpdateState(GameState::NotRunning);
                checkIntervalMs = 2000; // Check less frequently when Roblox isn't running
                
                // Wait before checking again
                std::this_thread::sleep_for(std::chrono::milliseconds(checkIntervalMs));
                continue;
            }
            
            // Update offsets if needed (with throttling to avoid excessive scanning)
            {
                std::lock_guard<std::mutex> lock(s_offsetsMutex);
                if (s_offsets.NeedsUpdate()) {
                    // Only update if it's been a while since last update
                    if (GetCurrentTimestampMs() - m_lastChecked.load() > 5000) {
                        UpdateRobloxOffsets();
                    }
                }
            }
            
            // Detect loading state first (this is important for proper UI timing)
            bool isLoading = DetectLoadingState();
            if (isLoading) {
                if (m_currentState.load() != GameState::Loading) {
                    UpdateState(GameState::Loading);
                }
                
                // Check more frequently during loading to catch when it completes
                checkIntervalMs = 500;
                
                // Wait before checking again
                std::this_thread::sleep_for(std::chrono::milliseconds(checkIntervalMs));
                continue;
            }
            
            // Not loading, so check if player is in a game
            bool inGame = CheckForGameObjects();
            
            // Update state based on check
            if (inGame) {
                if (m_currentState.load() != GameState::InGame) {
                    // Player just joined a game
                    UpdateState(GameState::InGame);
                    
                    // Update game info
                    UpdateGameInfo();
                    
                    // Set join time
                    m_lastGameJoinTime.store(GetCurrentTimestamp());
                }
                
                // In game - use normal check interval but randomize slightly to avoid detection
                checkIntervalMs = 1000 + (rand() % 200) - 100;
            } else {
                // If we were in game but now we're not, we're leaving
                if (m_currentState.load() == GameState::InGame) {
                    UpdateState(GameState::Leaving);
                    
                    // Clear game info
                    m_currentGameName = "";
                    m_currentPlaceId = "";
                    m_lastGameJoinTime.store(0);
                    
                    // Check more frequently during transition
                    checkIntervalMs = 500;
                } else if (m_currentState.load() == GameState::Leaving) {
                    // We've finished leaving, now at menu
                    UpdateState(GameState::Menu);
                    checkIntervalMs = 1000;
                } else if (m_currentState.load() == GameState::Unknown || 
                           m_currentState.load() == GameState::NotRunning) {
                    // We were not in a game, so we're at the menu
                    UpdateState(GameState::Menu);
                    checkIntervalMs = 1000;
                }
                // If already at menu, stay at menu
            }
            
            // Update last checked time
            m_lastChecked.store(GetCurrentTimestampMs());
            
            // Wait before checking again
            std::this_thread::sleep_for(std::chrono::milliseconds(checkIntervalMs));
        }
    }
    
    // Update Roblox offsets with improved pattern scanning
    void GameDetector::UpdateRobloxOffsets() {
        std::lock_guard<std::mutex> lock(s_offsetsMutex);
        
        // Reset existing cache
        s_offsets.Reset();
        s_serviceCache.clear();
        
        try {
            // 1. Find the DataModel singleton instance
            PatternScanner::ScanResult dataModelResult = PatternScanner::FindStringReference("RobloxPlayer", "DataModel");
            if (!dataModelResult.IsValid()) {
                return;
            }
            
            // Look for patterns that reference the DataModel singleton
            // This pattern varies by Roblox version, but often looks like:
            // "48 8B 05 ?? ?? ?? ?? 48 85 C0 74 ?? 48 8B 80"
            // where ?? ?? ?? ?? is a relative offset to the DataModel singleton pointer
            
            std::vector<std::string> possiblePatterns = {
                "48 8B 05 ?? ?? ?? ?? 48 85 C0 74 ?? 48 8B 80", // Common pattern 1
                "48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B D8", // Common pattern 2
                "48 8B 3D ?? ?? ?? ?? 48 85 FF 74 ?? 48 8B 87"  // Common pattern 3
            };
            
            for (const auto& pattern : possiblePatterns) {
                PatternScanner::ScanResult patternResult = PatternScanner::FindPatternInModule("RobloxPlayer", pattern);
                if (patternResult.IsValid()) {
                    // Found a potential DataModel reference
                    // Now we need to resolve the pointer from the instruction
                    
                    // Read the instruction at the found address
                    uint32_t instruction;
                    if (MemoryAccess::ReadMemory(patternResult.m_address, &instruction, sizeof(instruction))) {
                        // Extract the relative offset from the instruction
                        uint32_t relativeOffset = instruction & 0xFFFFFF;
                        
                        // Calculate the absolute address of the DataModel pointer
                        mach_vm_address_t pointerAddress = patternResult.m_address + relativeOffset + 7;
                        
                        // Read the actual DataModel pointer
                        mach_vm_address_t dataModelPtr = 0;
                        if (MemoryAccess::ReadMemory(pointerAddress, &dataModelPtr, sizeof(dataModelPtr))) {
                            s_offsets.dataModelPtr = dataModelPtr;
                        }
                    }
                    
                    if (s_offsets.dataModelPtr != 0) {
                        break; // Found DataModel, no need to check other patterns
                    }
                }
            }
            
            if (s_offsets.dataModelPtr == 0) {
                // Couldn't find DataModel, try alternative approach
                return;
            }
            
            // 2. Find service offsets using string references
            // Here we need to scan for patterns that access services from DataModel
            
            // For Workspace
            PatternScanner::ScanResult workspaceResult = PatternScanner::FindStringReference("RobloxPlayer", "Workspace");
            if (workspaceResult.IsValid()) {
                // Typically the offset is stored near the string reference
                // We need to scan for patterns like "48 8B 81 ?? ?? ?? ??" where the ?? ?? ?? ?? is the offset
                
                // For demonstration, we'll just use a common offset for Workspace
                // In a real implementation, you'd analyze the instructions around the string reference
                s_offsets.workspaceOffset = 0x80; // Example offset, would be determined dynamically
                
                // Cache the workspace service for faster access
                s_serviceCache["Workspace"] = s_offsets.dataModelPtr + s_offsets.workspaceOffset;
            }
            
            // For Players service
            PatternScanner::ScanResult playersResult = PatternScanner::FindStringReference("RobloxPlayer", "Players");
            if (playersResult.IsValid()) {
                // Use a common offset for Players service
                s_offsets.playersServiceOffset = 0x90; // Example offset
                
                // Cache the players service
                s_serviceCache["Players"] = s_offsets.dataModelPtr + s_offsets.playersServiceOffset;
            }
            
            // Find LocalPlayer offset from Players service
            PatternScanner::ScanResult localPlayerResult = PatternScanner::FindStringReference("RobloxPlayer", "LocalPlayer");
            if (localPlayerResult.IsValid() && s_serviceCache.count("Players")) {
                s_offsets.localPlayerOffset = 0x40; // Example offset
            }
            
            // Find common property offsets
            s_offsets.nameOffset = 0x30;      // Name property is often at this offset in Instance
            s_offsets.characterOffset = 0x88; // Character property offset from Player
            s_offsets.cameraOffset = 0xA0;    // Camera property offset from Workspace
            
            // Find game info offsets
            s_offsets.gameNameOffset = 0x120; // Game name may be stored at an offset from DataModel
            s_offsets.placeIdOffset = 0x130;  // Place ID may be stored at an offset from DataModel
            
            // Additional offsets for loading detection
            s_offsets.gameLoadingStatus = 0;  // Will detect dynamically when needed
            
            // Mark offsets as valid and update timestamp
            s_offsets.valid = true;
            s_offsets.lastUpdated = GetCurrentTimestamp();
            
            std::cout << "GameDetector: Updated Roblox offsets successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error updating Roblox offsets: " << e.what() << std::endl;
            s_offsets.Reset();
        }
    }
    
    // Detect if the game is currently in a loading state
    bool GameDetector::DetectLoadingState() {
        // If offsets are not valid, we can't detect loading state accurately
        if (!s_offsets.valid) {
            return false;
        }
        
        try {
            // Method 1: Check loading screen visibility
            PatternScanner::ScanResult loadingResult = PatternScanner::FindStringReference("RobloxPlayer", "LoadingScreen");
            if (loadingResult.IsValid()) {
                // Check if the loading screen is active by following pointers
                // For simplicity, we'll just check if the string was found
                return true;
            }
            
            // Method 2: Check if DataModel exists but some critical services are missing
            if (s_offsets.dataModelPtr != 0) {
                bool hasDataModel = ValidatePointer(s_offsets.dataModelPtr);
                bool hasWorkspace = false;
                
                if (s_serviceCache.count("Workspace")) {
                    hasWorkspace = ValidatePointer(s_serviceCache["Workspace"]);
                }
                
                // If we have DataModel but no Workspace, we're probably loading
                if (hasDataModel && !hasWorkspace) {
                    return true;
                }
                
                // Method 3: Check if essential services are missing
                std::set<std::string> foundServices;
                for (const auto& service : s_requiredServices) {
                    PatternScanner::ScanResult serviceResult = PatternScanner::FindStringReference("RobloxPlayer", service);
                    if (serviceResult.IsValid()) {
                        foundServices.insert(service);
                    }
                }
                
                // If we're missing some required services, we're probably loading
                if (!foundServices.empty() && foundServices.size() < s_requiredServices.size()) {
                    return true;
                }
            }
            
            return false;
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error detecting loading state: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Check for game objects with enhanced validation
    bool GameDetector::CheckForGameObjects() {
        // If offsets are not valid, we can't check game objects accurately
        if (!s_offsets.valid) {
            return false;
        }
        
        try {
            // 1. Check if DataModel exists
            if (!ValidatePointer(s_offsets.dataModelPtr)) {
                return false;
            }
            
            // 2. Check if Workspace exists and is valid
            if (!IsPlayerInGame()) {
                return false;
            }
            
            // 3. Check if PlayersService exists and is valid
            if (!AreGameServicesLoaded()) {
                return false;
            }
            
            // 4. Check if Camera exists and is valid
            if (!IsValidCamera()) {
                return false;
            }
            
            // 5. Check if LocalPlayer exists and has a valid Character
            if (!IsValidLocalPlayer()) {
                return false;
            }
            
            // All checks passed, player is in game
            return true;
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error checking game objects: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Validate if a pointer is valid and points to readable memory
    bool GameDetector::ValidatePointer(mach_vm_address_t ptr) {
        if (ptr == 0) {
            return false;
        }
        
        // Simple validity check: try to read a few bytes
        uint64_t testValue = 0;
        return MemoryAccess::ReadMemory(ptr, &testValue, sizeof(testValue));
    }
    
    // Check if player is in game by looking for a valid Workspace
    bool GameDetector::IsPlayerInGame() {
        if (!s_offsets.valid || s_offsets.dataModelPtr == 0) {
            return false;
        }
        
        try {
            // Get Workspace from DataModel
            mach_vm_address_t workspacePtr = 0;
            
            if (s_serviceCache.count("Workspace")) {
                workspacePtr = s_serviceCache["Workspace"];
            } else if (s_offsets.workspaceOffset != 0) {
                workspacePtr = s_offsets.dataModelPtr + s_offsets.workspaceOffset;
                
                // Read the actual workspace pointer
                MemoryAccess::ReadMemory(workspacePtr, &workspacePtr, sizeof(workspacePtr));
                
                // Cache it for faster access next time
                s_serviceCache["Workspace"] = workspacePtr;
            } else {
                // Try to find Workspace dynamically if offset is unknown
                PatternScanner::ScanResult workspaceResult = PatternScanner::FindStringReference("RobloxPlayer", "Workspace");
                if (workspaceResult.IsValid()) {
                    // In a real implementation, you'd analyze the code around this reference
                    // to find the actual offset to Workspace from DataModel
                    return true; // Simplified check
                }
                return false;
            }
            
            // Validate Workspace pointer
            if (!ValidatePointer(workspacePtr)) {
                return false;
            }
            
            // Check if Workspace has valid children
            // In a real implementation, you'd check for Workspace.CurrentCamera,
            // Workspace.Terrain, etc. to verify it's fully loaded
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error checking player in game: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Check if game services are loaded with enhanced validation
    bool GameDetector::AreGameServicesLoaded() {
        if (!s_offsets.valid || s_offsets.dataModelPtr == 0) {
            return false;
        }
        
        try {
            // Check for each essential service
            std::set<std::string> servicesToCheck = {
                "ReplicatedStorage", "Lighting", "CoreGui", "Players"
            };
            
            size_t foundCount = 0;
            
            for (const auto& serviceName : servicesToCheck) {
                // Check cache first
                if (s_serviceCache.count(serviceName)) {
                    if (ValidatePointer(s_serviceCache[serviceName])) {
                        foundCount++;
                        continue;
                    }
                }
                
                // Not in cache or invalid, try to find it
                PatternScanner::ScanResult serviceResult = PatternScanner::FindStringReference("RobloxPlayer", serviceName);
                if (serviceResult.IsValid()) {
                    // For simplicity, we're just checking if the string exists
                    // In a real implementation, you'd follow pointers to get the actual service
                    foundCount++;
                    
                    // We could also cache the service pointer for faster access next time
                    // s_serviceCache[serviceName] = servicePtr;
                }
            }
            
            // We consider services loaded if we found at least 3 out of 4 services
            return foundCount >= 3;
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error checking game services: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Check if camera is valid with enhanced validation
    bool GameDetector::IsValidCamera() {
        if (!s_offsets.valid) {
            return false;
        }
        
        try {
            // Get Workspace
            mach_vm_address_t workspacePtr = 0;
            
            if (s_serviceCache.count("Workspace")) {
                workspacePtr = s_serviceCache["Workspace"];
            } else {
                return false; // Cannot find camera without workspace
            }
            
            // Get Camera from Workspace
            mach_vm_address_t cameraPtr = 0;
            
            if (s_offsets.cameraOffset != 0) {
                // Read camera pointer from workspace
                MemoryAccess::ReadMemory(workspacePtr + s_offsets.cameraOffset, &cameraPtr, sizeof(cameraPtr));
            } else {
                // Try to find Camera dynamically
                PatternScanner::ScanResult cameraResult = PatternScanner::FindStringReference("RobloxPlayer", "Camera");
                if (cameraResult.IsValid()) {
                    // In a real implementation, you'd analyze the code around this reference
                    // to find the actual camera pointer
                    return true; // Simplified check
                }
                return false;
            }
            
            // Validate Camera pointer
            if (!ValidatePointer(cameraPtr)) {
                return false;
            }
            
            // In a real implementation, you might also check Camera properties like:
            // - CFrame
            // - ViewportSize
            // - FieldOfView
            // to ensure it's properly initialized
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error checking camera: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Check if local player is valid with enhanced validation
    bool GameDetector::IsValidLocalPlayer() {
        if (!s_offsets.valid) {
            return false;
        }
        
        try {
            // Get Players service
            mach_vm_address_t playersPtr = 0;
            
            if (s_serviceCache.count("Players")) {
                playersPtr = s_serviceCache["Players"];
            } else if (s_offsets.playersServiceOffset != 0) {
                playersPtr = s_offsets.dataModelPtr + s_offsets.playersServiceOffset;
                
                // Read the actual players pointer
                MemoryAccess::ReadMemory(playersPtr, &playersPtr, sizeof(playersPtr));
                
                // Cache it for faster access next time
                s_serviceCache["Players"] = playersPtr;
            } else {
                // Try to find Players dynamically
                PatternScanner::ScanResult playersResult = PatternScanner::FindStringReference("RobloxPlayer", "Players");
                if (playersResult.IsValid()) {
                    return true; // Simplified check
                }
                return false;
            }
            
            // Validate Players pointer
            if (!ValidatePointer(playersPtr)) {
                return false;
            }
            
            // Get LocalPlayer from Players
            mach_vm_address_t localPlayerPtr = 0;
            
            if (s_offsets.localPlayerOffset != 0) {
                // Read LocalPlayer pointer
                MemoryAccess::ReadMemory(playersPtr + s_offsets.localPlayerOffset, &localPlayerPtr, sizeof(localPlayerPtr));
            } else {
                // Try to find LocalPlayer dynamically
                PatternScanner::ScanResult localPlayerResult = PatternScanner::FindStringReference("RobloxPlayer", "LocalPlayer");
                if (localPlayerResult.IsValid()) {
                    return true; // Simplified check
                }
                return false;
            }
            
            // Validate LocalPlayer pointer
            if (!ValidatePointer(localPlayerPtr)) {
                return false;
            }
            
            // Check if LocalPlayer has a Character
            mach_vm_address_t characterPtr = 0;
            
            if (s_offsets.characterOffset != 0) {
                // Read Character pointer
                MemoryAccess::ReadMemory(localPlayerPtr + s_offsets.characterOffset, &characterPtr, sizeof(characterPtr));
            } else {
                // Try to find Character dynamically
                PatternScanner::ScanResult characterResult = PatternScanner::FindStringReference("RobloxPlayer", "Character");
                if (characterResult.IsValid()) {
                    return true; // Simplified check
                }
            }
            
            // For a complete check, we'd also validate that Character has essential parts
            // like HumanoidRootPart, but this is a reasonable simplification
            
            return ValidatePointer(characterPtr);
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error checking local player: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Update game info with improved extraction
    void GameDetector::UpdateGameInfo() {
        if (!s_offsets.valid) {
            return;
        }
        
        try {
            // Find game name by looking at game.PlaceId property
            mach_vm_address_t dataModelPtr = s_offsets.dataModelPtr;
            
            if (!ValidatePointer(dataModelPtr)) {
                return;
            }
            
            // Default values in case we can't find better information
            m_currentGameName = "Unknown Game";
            m_currentPlaceId = "0";
            
            // Try to find game name from JobId or placeId properties
            PatternScanner::ScanResult jobIdResult = PatternScanner::FindStringReference("RobloxPlayer", "JobId");
            PatternScanner::ScanResult placeIdResult = PatternScanner::FindStringReference("RobloxPlayer", "PlaceId");
            
            if (placeIdResult.IsValid()) {
                // Try to find instructions that load the PlaceId
                // This is a simplified approach; in a real implementation you'd
                // analyze the code around the reference to extract the actual value
                
                // For demonstration, we'll try to read where the PlaceId string likely points to
                mach_vm_address_t placeIdAddr = 0;
                if (PatternScanner::ResolveAdrpSequence(placeIdResult.m_address + 8, 4) != 0) {
                    placeIdAddr = PatternScanner::ResolveAdrpSequence(placeIdResult.m_address + 8, 4);
                    
                    // Try to read the place ID value
                    uint32_t placeId = 0;
                    if (MemoryAccess::ReadMemory(placeIdAddr, &placeId, sizeof(placeId))) {
                        // Convert place ID to string
                        std::stringstream ss;
                        ss << placeId;
                        m_currentPlaceId = ss.str();
                        
                        // Try to obtain the game name from place ID
                        // In a real implementation, you might call a Roblox API or lookup a database
                        
                        // For demonstration, we'll check if we can find a Name property
                        PatternScanner::ScanResult nameResult = PatternScanner::FindStringReference("RobloxPlayer", "Name");
                        if (nameResult.IsValid()) {
                            // Try to find where the game name string is stored
                            mach_vm_address_t nameStringPtr = 0;
                            if (PatternScanner::ResolveAdrpSequence(nameResult.m_address + 16, 4) != 0) {
                                nameStringPtr = PatternScanner::ResolveAdrpSequence(nameResult.m_address + 16, 4);
                                
                                // Read the game name string
                                std::string gameName = ReadRobloxString(nameStringPtr);
                                if (!gameName.empty()) {
                                    m_currentGameName = gameName;
                                }
                            }
                        }
                    }
                }
            }
            
            std::cout << "GameDetector: Updated game info - Name: " << m_currentGameName 
                      << ", PlaceId: " << m_currentPlaceId << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error updating game info: " << e.what() << std::endl;
        }
    }
    
    // Update state and notify callbacks with improved logging
    void GameDetector::UpdateState(GameState newState) {
        // Get old state
        GameState oldState = m_currentState.load();
        
        // If state hasn't changed, do nothing
        if (oldState == newState) {
            return;
        }
        
        // Update state
        m_currentState.store(newState);
        
        // Convert states to strings for better logging
        std::unordered_map<GameState, std::string> stateNames = {
            {GameState::Unknown, "Unknown"},
            {GameState::NotRunning, "NotRunning"},
            {GameState::Menu, "Menu"},
            {GameState::Loading, "Loading"},
            {GameState::InGame, "InGame"},
            {GameState::Leaving, "Leaving"}
        };
        
        // Log state change
        std::cout << "GameDetector: State changed from " << stateNames[oldState]
                  << " to " << stateNames[newState] << std::endl;
        
        // Notify callbacks
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        for (const auto& callback : m_callbacks) {
            callback.second(oldState, newState);
        }
    }
}
