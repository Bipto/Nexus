#pragma once

#include "Nexus/Point.hpp"

namespace Nexus
{
    /// @brief An enum representing the state of a mouse button
    enum class MouseButtonState
    {
        /// @brief A value representing that the button is currently released
        Released,

        /// @brief A value representing that the button is currently pressed
        Pressed,
    };

    /// @brief A struct containing the current state of the mouse
    struct MouseState
    {
        /// @brief A enum value representing the current state of the left mouse button
        MouseButtonState LeftButton;

        /// @brief A enum value representing the current state of the right mouse button
        MouseButtonState RightButton;

        /// @brief An enum value representing the current state of the middle mouse button
        MouseButtonState MiddleButton;

        /// @brief A Nexus::Point containing two integers representing the position of the mouse cursor
        Point<int> MousePosition;

        /// @brief A Nexus::Point containing two integers representing the current position of the mouse wheel
        Point<int> MouseWheel;
    };

    /// @brief A class representing the current state of the mouse
    class Mouse
    {
    public:
        /// @brief A method that caches the previous frames mouse states
        void CacheInput();

        /// @brief A method that checks whether the left mouse buttons was clicked
        /// @return A boolean value representing if the left mouse button was clicked
        bool WasLeftMouseClicked();

        /// @brief A method that checks whether the left mouse buttons was released
        /// @return A boolean value representing if the left mouse button was released
        bool WasLeftMouseReleased();

        /// @brief A method that checks whether the left mouse buttons was held
        /// @return A boolean value representing if the left mouse button was held
        bool IsLeftMouseHeld();

        /// @brief A method that checks whether the right mouse buttons was clicked
        /// @return A boolean value representing if the right mouse button was clicked
        bool WasRightMouseClicked();

        /// @brief A method that checks whether the right mouse buttons was released
        /// @return A boolean value representing if the right mouse button was released
        bool WasRightMouseReleased();

        /// @brief A method that checks whether the right mouse buttons was held
        /// @return A boolean value representing if the right mouse button was held
        bool IsRightMouseHeld();

        /// @brief A method that checks whether the middle mouse buttons was clicked
        /// @return A boolean value representing if the middle mouse button was clicked
        bool WasMiddleMouseClicked();

        /// @brief A method that checks whether the left mouse buttons was released
        /// @return A boolean value representing if the middle mouse button was released
        bool WasMiddleMouseReleased();

        /// @brief A method that checks whether the left mouse buttons was held
        /// @return A boolean value representing if the middle mouse button was held
        bool IsMiddleMouseHeld();

        /// @brief A method that returns the current position of the mouse pointer
        /// @return A Nexus::Point containing two integer values representing the position of the mouse
        Point<int> GetMousePosition();

        /// @brief A method that returns the movement of the mouse pointer
        /// @return A Nexus::Point containing two integer values representing the movement of the mouse
        Point<int> GetMouseMovement();

        /// @brief A method that returns the current scroll value of the mouse
        /// @return A Nexus::Point containing two integer values representing the current scroll of the mouse
        Point<int> GetScroll();

        /// @brief A method that returns the scroll movement of the mouse
        /// @return A Nexus::Point containing two integer values representing the scroll movement of the mouse
        Point<int> GetScrollMovement();

    private:
        /// @brief A mouse state containing the current state of the mouse pointer and buttons
        MouseState m_CurrentState;

        /// @brief A mouse state containing the previous state of the mouse pointer and buttons
        MouseState m_PreviousState;

        /// @brief A friend class to allow a window to have access to the mouse's private members
        friend class Window;
    };
}