#include "GameDetector.h"
#include "MemoryAccess.h"
#include "PatternScanner.h"
#include <chrono>
#include <iostream>
#include <unordered_map>

namespace iOS {
    // Constructor
    GameDetector::GameDetector()
        : m_currentState(GameState::Unknown),
          m_running(false),
          m_lastChecked(0),
          m_lastGameJoinTime(0),
          m_currentGameName(""),
          m_currentPlaceId("") {
    }
    
    // Destructor
    GameDetector::~GameDetector() {
        Stop();
    }
    
    // Start detection thread
    bool GameDetector::Start() {
        if (m_running.load()) {
            return true; // Already running
        }
        
        // Initialize memory access if not already initialized
        if (!MemoryAccess::Initialize()) {
            std::cerr << "GameDetector: Failed to initialize memory access" << std::endl;
            return false;
        }
        
        // Set running flag
        m_running.store(true);
        
        // Start detection thread
        m_detectionThread = std::thread(&GameDetector::DetectionLoop, this);
        
        std::cout << "GameDetector: Started detection thread" << std::endl;
        return true;
    }
    
    // Stop detection thread
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
    
    // Register a callback for state changes
    size_t GameDetector::RegisterCallback(const StateChangeCallback& callback) {
        if (!callback) {
            return 0; // Invalid callback
        }
        
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        
        // Find a unique ID
        static size_t nextId = 1;
        size_t id = nextId++;
        
        // Store callback with ID
        m_callbacks.push_back(callback);
        
        return id;
    }
    
    // Remove a registered callback
    bool GameDetector::RemoveCallback(size_t id) {
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        
        // Find and remove callback with matching ID
        for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it) {
            if (it == m_callbacks.begin() + (id - 1)) {
                m_callbacks.erase(it);
                return true;
            }
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
    
    // Get current game name
    std::string GameDetector::GetGameName() const {
        return m_currentGameName;
    }
    
    // Get current place ID
    std::string GameDetector::GetPlaceId() const {
        return m_currentPlaceId;
    }
    
    // Get time since player joined the game
    uint64_t GameDetector::GetTimeInGame() const {
        if (m_currentState.load() != GameState::InGame || m_lastGameJoinTime.load() == 0) {
            return 0;
        }
        
        uint64_t now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        return now - m_lastGameJoinTime.load();
    }
    
    // Force a state update check
    GameDetector::GameState GameDetector::ForceCheck() {
        // Check game objects
        bool inGame = CheckForGameObjects();
        
        // Update state based on check
        if (inGame) {
            UpdateState(GameState::InGame);
        } else {
            // If we were in game but now we're not, we're leaving
            if (m_currentState.load() == GameState::InGame) {
                UpdateState(GameState::Leaving);
            } else {
                // Otherwise we're at the menu or loading
                UpdateState(GameState::Menu);
            }
        }
        
        return m_currentState.load();
    }
    
    // Main detection loop
    void GameDetector::DetectionLoop() {
        // Check interval in milliseconds
        const int CHECK_INTERVAL_MS = 1000; // Check every second
        
        while (m_running.load()) {
            // Check if Roblox is running
            bool robloxRunning = MemoryAccess::GetModuleBase("RobloxPlayer") != 0;
            
            if (!robloxRunning) {
                // Update state to not running
                UpdateState(GameState::NotRunning);
                
                // Wait before checking again
                std::this_thread::sleep_for(std::chrono::milliseconds(CHECK_INTERVAL_MS));
                continue;
            }
            
            // Check if player is in a game
            bool inGame = CheckForGameObjects();
            
            // Update state based on check
            if (inGame) {
                if (m_currentState.load() != GameState::InGame) {
                    // Player just joined a game
                    UpdateState(GameState::InGame);
                    
                    // Update game info
                    UpdateGameInfo();
                    
                    // Set join time
                    m_lastGameJoinTime.store(std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count());
                }
            } else {
                // If we were in game but now we're not, we're leaving
                if (m_currentState.load() == GameState::InGame) {
                    UpdateState(GameState::Leaving);
                    
                    // Clear game info
                    m_currentGameName = "";
                    m_currentPlaceId = "";
                    m_lastGameJoinTime.store(0);
                } else if (m_currentState.load() == GameState::Leaving) {
                    // We've finished leaving, now at menu
                    UpdateState(GameState::Menu);
                } else if (m_currentState.load() == GameState::Unknown || 
                           m_currentState.load() == GameState::NotRunning) {
                    // We were not in a game, so we're at the menu
                    UpdateState(GameState::Menu);
                }
                // If already at menu, stay at menu
            }
            
            // Update last checked time
            m_lastChecked.store(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
            
            // Wait before checking again
            std::this_thread::sleep_for(std::chrono::milliseconds(CHECK_INTERVAL_MS));
        }
    }
    
    // Check for game objects to determine if player is in a game
    bool GameDetector::CheckForGameObjects() {
        // Check multiple indicators to ensure we're really in a game
        // All must be true for us to consider the player in-game
        
        // 1. Check if player is in game
        if (!IsPlayerInGame()) {
            return false;
        }
        
        // 2. Check if game services are loaded
        if (!AreGameServicesLoaded()) {
            return false;
        }
        
        // 3. Check if camera is valid
        if (!IsValidCamera()) {
            return false;
        }
        
        // 4. Check if local player is valid
        if (!IsValidLocalPlayer()) {
            return false;
        }
        
        // All checks passed, player is in game
        return true;
    }
    
    // Check if player is in game by looking for a valid Workspace
    bool GameDetector::IsPlayerInGame() {
        // In a real implementation, we would use pattern scanning and memory access
        // to find the Workspace object and verify it's properly initialized
        
        // This is a simplified implementation for demonstration purposes
        // In a real version, we would:
        // 1. Find the DataModel
        // 2. Access its Workspace property
        // 3. Verify Workspace has valid children
        
        try {
            // Find pattern for "Workspace" string reference
            PatternScanner::ScanResult result = PatternScanner::FindStringReference("RobloxPlayer", "Workspace");
            
            if (result.IsValid()) {
                // Found a reference to Workspace, now we need to check if it's properly initialized
                // This would involve following pointers and checking object states
                
                // For demonstration, we'll just check if we can find other required objects too
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error checking workspace: " << e.what() << std::endl;
        }
        
        return false;
    }
    
    // Check if game services are loaded
    bool GameDetector::AreGameServicesLoaded() {
        // In a real implementation, we would check for essential game services:
        // - ReplicatedStorage
        // - ServerStorage
        // - CoreGui
        // - Lighting
        // etc.
        
        try {
            // Check for essential services by finding string references
            std::vector<std::string> essentialServices = {
                "ReplicatedStorage",
                "Lighting",
                "CoreGui"
            };
            
            for (const auto& service : essentialServices) {
                PatternScanner::ScanResult result = PatternScanner::FindStringReference("RobloxPlayer", service);
                if (!result.IsValid()) {
                    // Service not found
                    return false;
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error checking game services: " << e.what() << std::endl;
        }
        
        return false;
    }
    
    // Check if camera is valid
    bool GameDetector::IsValidCamera() {
        try {
            // Check for Camera by finding string reference
            PatternScanner::ScanResult result = PatternScanner::FindStringReference("RobloxPlayer", "Camera");
            
            if (result.IsValid()) {
                // Found a reference to Camera
                // In a real implementation, we would verify the Camera is properly initialized
                // by checking its CFrame, ViewportSize, etc.
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error checking camera: " << e.what() << std::endl;
        }
        
        return false;
    }
    
    // Check if local player is valid
    bool GameDetector::IsValidLocalPlayer() {
        try {
            // Check for LocalPlayer by finding string reference
            PatternScanner::ScanResult result = PatternScanner::FindStringReference("RobloxPlayer", "LocalPlayer");
            
            if (result.IsValid()) {
                // Found a reference to LocalPlayer
                // In a real implementation, we would verify the LocalPlayer is properly initialized
                // by checking its Character, Name, etc.
                
                // We would also check if player has spawned by verifying Character exists
                PatternScanner::ScanResult charResult = PatternScanner::FindStringReference("RobloxPlayer", "Character");
                
                return charResult.IsValid();
            }
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error checking local player: " << e.what() << std::endl;
        }
        
        return false;
    }
    
    // Update game info
    void GameDetector::UpdateGameInfo() {
        try {
            // In a real implementation, we would read game name and place ID from memory
            // by finding the DataModel and accessing its properties
            
            // For demonstration purposes, we'll use placeholder values
            m_currentGameName = "Unknown Game";
            m_currentPlaceId = "0";
            
            // Find game name pattern
            PatternScanner::ScanResult nameResult = PatternScanner::FindStringReference("RobloxPlayer", "Name");
            if (nameResult.IsValid()) {
                // In a real implementation, we would follow pointers to read the game name string
                // For demonstration, we'll leave as "Unknown Game"
            }
            
            // Find place ID pattern
            PatternScanner::ScanResult placeResult = PatternScanner::FindStringReference("RobloxPlayer", "PlaceId");
            if (placeResult.IsValid()) {
                // In a real implementation, we would follow pointers to read the place ID
                // For demonstration, we'll leave as "0"
            }
        } catch (const std::exception& e) {
            std::cerr << "GameDetector: Error updating game info: " << e.what() << std::endl;
        }
    }
    
    // Update state and notify callbacks
    void GameDetector::UpdateState(GameState newState) {
        // Get old state
        GameState oldState = m_currentState.load();
        
        // If state hasn't changed, do nothing
        if (oldState == newState) {
            return;
        }
        
        // Update state
        m_currentState.store(newState);
        
        // Log state change
        std::cout << "GameDetector: State changed from " << static_cast<int>(oldState) 
                  << " to " << static_cast<int>(newState) << std::endl;
        
        // Notify callbacks
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        for (const auto& callback : m_callbacks) {
            callback(oldState, newState);
        }
    }
}
