#pragma once

#include <string>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <utility>

namespace iOS {
    /**
     * @class GameDetector
     * @brief Detects when a player has joined a Roblox game
     * 
     * This class monitors the Roblox memory and objects to determine when
     * a player has fully joined a game. It provides callbacks for game join
     * and exit events, allowing the executor to appear only when in-game.
     * 
     * Features:
     * - Dynamic memory pattern scanning for reliable detection
     * - Accurate state transitions including loading states
     * - Performance optimization with caching and throttling
     * - Detailed game information extraction
     */
    class GameDetector {
    public:
        // Game state enumeration
        enum class GameState {
            Unknown,        // Initial state or error
            NotRunning,     // Roblox is not running
            Menu,           // At menu screens (login, game select, etc.)
            Loading,        // Game is loading
            InGame,         // Fully in a game
            Leaving         // Exiting a game
        };
        
        // Callback for game state changes
        using StateChangeCallback = std::function<void(GameState, GameState)>;
        
    private:
        // Member variables with consistent m_ prefix
        std::atomic<GameState> m_currentState;
        std::atomic<bool> m_running;
        std::thread m_detectionThread;
        std::mutex m_callbackMutex;
        std::vector<std::pair<size_t, StateChangeCallback>> m_callbacks;
        std::atomic<uint64_t> m_lastChecked;
        std::atomic<uint64_t> m_lastGameJoinTime;
        std::string m_currentGameName;
        std::string m_currentPlaceId;
        
        // Private methods
        void DetectionLoop();
        bool CheckForGameObjects();
        bool IsPlayerInGame();
        bool AreGameServicesLoaded();
        bool IsValidCamera();
        bool IsValidLocalPlayer();
        void UpdateGameInfo();
        void UpdateState(GameState newState);
        
        // New private methods
        void UpdateRobloxOffsets();
        bool DetectLoadingState();
        bool ValidatePointer(mach_vm_address_t ptr);
        
    public:
        /**
         * @brief Constructor with enhanced initialization
         */
        GameDetector();
        
        /**
         * @brief Destructor with enhanced cleanup
         */
        ~GameDetector();
        
        /**
         * @brief Start detection thread
         * @return True if started successfully, false otherwise
         */
        bool Start();
        
        /**
         * @brief Stop detection thread
         */
        void Stop();
        
        /**
         * @brief Register a callback for state changes
         * @param callback Function to call when game state changes
         * @return Unique ID for the callback (can be used to remove it)
         * 
         * Enhanced with secure random ID generation to prevent ID collisions
         * and more robust callback storage.
         */
        size_t RegisterCallback(const StateChangeCallback& callback);
        
        /**
         * @brief Remove a registered callback
         * @param id ID of the callback to remove
         * @return True if callback was removed, false if not found
         */
        bool RemoveCallback(size_t id);
        
        /**
         * @brief Get current game state
         * @return Current state of the game
         */
        GameState GetState() const;
        
        /**
         * @brief Check if player is in a game
         * @return True if in a game, false otherwise
         */
        bool IsInGame() const;
        
        /**
         * @brief Get current game name
         * @return Name of the current game, or "Unknown Game" if not in a game or name couldn't be determined
         */
        std::string GetGameName() const;
        
        /**
         * @brief Get current place ID
         * @return Place ID of the current game, or "0" if not in a game or ID couldn't be determined
         */
        std::string GetPlaceId() const;
        
        /**
         * @brief Get time since player joined the game
         * @return Seconds since joining the game, or 0 if not in a game
         */
        uint64_t GetTimeInGame() const;
        
        /**
         * @brief Force a state update check
         * @return Current state after check
         * 
         * Enhanced with more reliable detection and automatic offset updating
         */
        GameState ForceCheck();
    };
}
