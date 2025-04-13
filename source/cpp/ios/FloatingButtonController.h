#pragma once

#include <string>
#include <functional>

namespace iOS {
    /**
     * @class FloatingButtonController
     * @brief Controls a persistent floating button on the iOS screen
     * 
     * This class manages a small button that stays on screen at all times,
     * allowing users to quickly access the executor. The button can be moved
     * to different screen edges and has adjustable opacity.
     */
    class FloatingButtonController {
    public:
        // Callback type for button tap events
        using TapCallback = std::function<void()>;
        
        // Button position enumeration
        enum class Position {
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
            Custom // For user-defined positions
        };
        
    private:
        // Member variables with consistent m_ prefix
        void* m_buttonView; // Opaque pointer to UIView (keeps C++ header clean)
        bool m_isVisible;
        Position m_position;
        float m_opacity;
        float m_customX;
        float m_customY;
        float m_size;
        TapCallback m_tapCallback;
        bool m_isBeingDragged;
        
        // Private methods
        void UpdateButtonPosition();
        void SavePosition();
        void LoadPosition();
        
    public:
        /**
         * @brief Constructor
         * @param initialPosition Initial button position
         * @param size Button size in points
         * @param opacity Button opacity (0.0 - 1.0)
         */
        FloatingButtonController(Position initialPosition = Position::BottomRight, 
                                float size = 50.0f, float opacity = 0.7f);
        
        /**
         * @brief Destructor
         */
        ~FloatingButtonController();
        
        /**
         * @brief Show the floating button
         */
        void Show();
        
        /**
         * @brief Hide the floating button
         */
        void Hide();
        
        /**
         * @brief Toggle button visibility
         * @return New visibility state
         */
        bool Toggle();
        
        /**
         * @brief Check if button is visible
         * @return True if visible, false otherwise
         */
        bool IsVisible() const;
        
        /**
         * @brief Set button position
         * @param position New position
         */
        void SetPosition(Position position);
        
        /**
         * @brief Set custom button position
         * @param x X coordinate (0.0 - 1.0, percentage of screen width)
         * @param y Y coordinate (0.0 - 1.0, percentage of screen height)
         */
        void SetCustomPosition(float x, float y);
        
        /**
         * @brief Get current button position
         * @return Current position
         */
        Position GetPosition() const;
        
        /**
         * @brief Get custom X coordinate
         * @return X coordinate as percentage of screen width
         */
        float GetCustomX() const;
        
        /**
         * @brief Get custom Y coordinate
         * @return Y coordinate as percentage of screen height
         */
        float GetCustomY() const;
        
        /**
         * @brief Set button opacity
         * @param opacity New opacity (0.0 - 1.0)
         */
        void SetOpacity(float opacity);
        
        /**
         * @brief Get button opacity
         * @return Current opacity
         */
        float GetOpacity() const;
        
        /**
         * @brief Set button size
         * @param size New size in points
         */
        void SetSize(float size);
        
        /**
         * @brief Get button size
         * @return Current size in points
         */
        float GetSize() const;
        
        /**
         * @brief Set tap callback
         * @param callback Function to call when button is tapped
         */
        void SetTapCallback(TapCallback callback);
        
        /**
         * @brief Enable/disable button dragging
         * @param enabled True to enable dragging, false to disable
         */
        void SetDraggable(bool enabled);
        
        /**
         * @brief Check if button is being dragged
         * @return True if being dragged, false otherwise
         */
        bool IsBeingDragged() const;
    };
}
