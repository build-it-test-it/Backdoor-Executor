#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include "ScriptEditorViewController.h"
#include "ScriptManagementViewController.h"
#include "../GameDetector.h"
#include "../ai_features/ScriptAssistant.h"

namespace iOS {
namespace UI {

    /**
     * @class MainViewController
     * @brief Primary UI controller for the executor with advanced visual effects
     * 
     * This class integrates all UI components into a cohesive interface with
     * memory optimization, beautiful visual effects, and complete script management.
     * Designed for iOS 15-18+ with a focus on user experience and aesthetics.
     */
    class MainViewController {
    public:
        // Tab enumeration
        enum class Tab {
            Editor,
            Scripts,
            Console,
            Settings,
            Assistant
        };
        
        // Visual style enumeration
        enum class VisualStyle {
            Minimal,      // Clean, simple UI
            Dynamic,      // Dynamic LED effects and animations
            Futuristic,   // High-tech futuristic interface
            Retro,        // Retro style with pixel effects
            Adaptive      // Adapts to current game theme
        };
        
        // Navigation mode enumeration
        enum class NavigationMode {
            Tabs,         // Standard tab bar
            Sidebar,      // iPad-style sidebar
            Gestures,     // Gesture-based navigation
            Combined      // Combination of tabs and gestures
        };
        
        // Notification structure
        struct Notification {
            std::string m_title;          // Notification title
            std::string m_message;        // Notification message
            bool m_isError;               // Is an error notification
            uint64_t m_timestamp;         // Notification timestamp
            
            Notification() : m_isError(false), m_timestamp(0) {}
            
            Notification(const std::string& title, const std::string& message, bool isError = false)
                : m_title(title), m_message(message), m_isError(isError),
                  m_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()) {}
        };
        
        // UI callback types
        using TabChangedCallback = std::function<void(Tab)>;
        using VisibilityChangedCallback = std::function<void(bool)>;
        using ExecutionCallback = std::function<void(const ScriptEditorViewController::ExecutionResult&)>;
        
    private:
        // Member variables with consistent m_ prefix
        void* m_viewController;            // Opaque pointer to UIViewController
        void* m_tabBar;                    // Opaque pointer to tab bar
        void* m_navigationController;      // Opaque pointer to navigation controller
        void* m_floatingButton;            // Opaque pointer to floating button
        void* m_notificationView;          // Opaque pointer to notification view
        void* m_visualEffectsEngine;       // Opaque pointer to visual effects engine
        void* m_memoryManager;             // Opaque pointer to memory manager
        void* m_blurEffectView;            // Opaque pointer to background blur
        std::shared_ptr<ScriptEditorViewController> m_editorViewController; // Editor view controller
        std::shared_ptr<ScriptManagementViewController> m_scriptsViewController; // Scripts view controller
        std::shared_ptr<GameDetector> m_gameDetector; // Game detector
        std::shared_ptr<AIFeatures::ScriptAssistant> m_scriptAssistant; // Script assistant
        std::unordered_map<Tab, void*> m_tabViewControllers; // Tab view controllers
        std::vector<Notification> m_notifications; // Recent notifications
        Tab m_currentTab;                  // Current tab
        VisualStyle m_visualStyle;         // Current visual style
        NavigationMode m_navigationMode;   // Current navigation mode
        TabChangedCallback m_tabChangedCallback; // Tab changed callback
        VisibilityChangedCallback m_visibilityChangedCallback; // Visibility changed callback
        ExecutionCallback m_executionCallback; // Script execution callback
        bool m_isVisible;                  // Is UI visible
        bool m_isFloatingButtonVisible;    // Is floating button visible
        bool m_isInGame;                   // Is currently in a game
        bool m_useHapticFeedback;          // Use haptic feedback
        bool m_useAnimations;              // Use animations
        bool m_reduceTransparency;         // Reduce transparency for accessibility
        bool m_reducedMemoryMode;          // Memory optimization mode
        std::unordered_map<std::string, void*> m_ledEffects; // LED effect layers
        int m_colorScheme;                 // Active color scheme (0-5)
        
        // Private methods
        void InitializeUI();
        void SetupTabBar();
        void SetupFloatingButton();
        void SetupNotificationView();
        void SetupVisualEffects();
        void SetupMemoryManagement();
        void SetupGameDetection();
        void SetupAIAssistant();
        void CreateTabViewControllers();
        void SwitchToTab(Tab tab, bool animated);
        void UpdateFloatingButtonVisibility();
        void ShowNotification(const Notification& notification);
        void HandleGameStateChanged(GameDetector::GameState oldState, GameDetector::GameState newState);
        void ApplyVisualStyle(VisualStyle style);
        void SetupLEDEffects();
        void PulseLEDEffect(void* layer, float duration, float intensity);
        void FadeInUI(float duration);
        void FadeOutUI(float duration);
        void OptimizeUIForCurrentMemoryUsage();
        void ReleaseUnusedViewControllers();
        void UpdateNavigationMode(NavigationMode mode);
        void SetupToolbar();
        void AnimateTabTransition(Tab fromTab, Tab toTab);
        void ConfigureForCurrentDevice();
        void UpdateColorScheme(int scheme);
        void StoreUIState();
        void RestoreUIState();
        void CreateSubviewHierarchy();
        void ConfigureConstraints();
        void RegisterForNotifications();
        void UnregisterFromNotifications();
        
    public:
        /**
         * @brief Constructor
         */
        MainViewController();
        
        /**
         * @brief Destructor
         */
        ~MainViewController();
        
        /**
         * @brief Initialize the view controller
         * @return True if initialization succeeded, false otherwise
         */
        bool Initialize();
        
        /**
         * @brief Get the native view controller
         * @return Opaque pointer to UIViewController
         */
        void* GetViewController() const;
        
        /**
         * @brief Show the UI
         */
        void Show();
        
        /**
         * @brief Hide the UI
         */
        void Hide();
        
        /**
         * @brief Toggle UI visibility
         * @return New visibility state
         */
        bool Toggle();
        
        /**
         * @brief Check if UI is visible
         * @return True if visible, false otherwise
         */
        bool IsVisible() const;
        
        /**
         * @brief Show the floating button
         */
        void ShowFloatingButton();
        
        /**
         * @brief Hide the floating button
         */
        void HideFloatingButton();
        
        /**
         * @brief Set the current tab
         * @param tab Tab to switch to
         */
        void SetTab(Tab tab);
        
        /**
         * @brief Get the current tab
         * @return Current tab
         */
        Tab GetCurrentTab() const;
        
        /**
         * @brief Set visual style
         * @param style Visual style to use
         */
        void SetVisualStyle(VisualStyle style);
        
        /**
         * @brief Get current visual style
         * @return Current visual style
         */
        VisualStyle GetVisualStyle() const;
        
        /**
         * @brief Set navigation mode
         * @param mode Navigation mode to use
         */
        void SetNavigationMode(NavigationMode mode);
        
        /**
         * @brief Get current navigation mode
         * @return Current navigation mode
         */
        NavigationMode GetNavigationMode() const;
        
        /**
         * @brief Execute a script
         * @param script Script to execute
         * @return Execution result
         */
        ScriptEditorViewController::ExecutionResult ExecuteScript(const std::string& script);
        
        /**
         * @brief Debug a script
         * @param script Script to debug
         * @return Debug information
         */
        std::vector<ScriptEditorViewController::DebugInfo> DebugScript(const std::string& script);
        
        /**
         * @brief Set the tab changed callback
         * @param callback Function to call when tab changes
         */
        void SetTabChangedCallback(const TabChangedCallback& callback);
        
        /**
         * @brief Set the visibility changed callback
         * @param callback Function to call when visibility changes
         */
        void SetVisibilityChangedCallback(const VisibilityChangedCallback& callback);
        
        /**
         * @brief Set the execution callback
         * @param callback Function to call for script execution
         */
        void SetExecutionCallback(const ExecutionCallback& callback);
        
        /**
         * @brief Set the game detector
         * @param gameDetector Game detector to use
         */
        void SetGameDetector(std::shared_ptr<GameDetector> gameDetector);
        
        /**
         * @brief Set the script assistant
         * @param scriptAssistant Script assistant to use
         */
        void SetScriptAssistant(std::shared_ptr<AIFeatures::ScriptAssistant> scriptAssistant);
        
        /**
         * @brief Get the editor view controller
         * @return Editor view controller
         */
        std::shared_ptr<ScriptEditorViewController> GetEditorViewController() const;
        
        /**
         * @brief Get the scripts view controller
         * @return Scripts view controller
         */
        std::shared_ptr<ScriptManagementViewController> GetScriptsViewController() const;
        
        /**
         * @brief Enable or disable haptic feedback
         * @param enable Whether to enable haptic feedback
         */
        void SetUseHapticFeedback(bool enable);
        
        /**
         * @brief Check if haptic feedback is enabled
         * @return True if haptic feedback is enabled, false otherwise
         */
        bool GetUseHapticFeedback() const;
        
        /**
         * @brief Enable or disable animations
         * @param enable Whether to enable animations
         */
        void SetUseAnimations(bool enable);
        
        /**
         * @brief Check if animations are enabled
         * @return True if animations are enabled, false otherwise
         */
        bool GetUseAnimations() const;
        
        /**
         * @brief Enable or disable reduced memory mode
         * @param enable Whether to enable reduced memory mode
         */
        void SetReducedMemoryMode(bool enable);
        
        /**
         * @brief Check if reduced memory mode is enabled
         * @return True if reduced memory mode is enabled, false otherwise
         */
        bool GetReducedMemoryMode() const;
        
        /**
         * @brief Set color scheme
         * @param scheme Color scheme index (0-5)
         */
        void SetColorScheme(int scheme);
        
        /**
         * @brief Get current color scheme
         * @return Current color scheme index
         */
        int GetColorScheme() const;
        
        /**
         * @brief Reset UI settings to defaults
         */
        void ResetSettings();
        
        /**
         * @brief Get memory usage
         * @return Memory usage in bytes
         */
        uint64_t GetMemoryUsage() const;
        
        /**
         * @brief Get UI element by identifier
         * @param identifier Element identifier
         * @return Opaque pointer to UI element
         */
        void* GetUIElement(const std::string& identifier) const;
        
        /**
         * @brief Register custom view
         * @param identifier View identifier
         * @param view Opaque pointer to view
         */
        void RegisterCustomView(const std::string& identifier, void* view);
    };

} // namespace UI
} // namespace iOS
