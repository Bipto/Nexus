#pragma once

#include "Core/Point.h"
#include "Core/Logging/Log.h"

#include "SDL.h"
#include "SDL_gamecontroller.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

#include <map>
#include <sstream>
#include <vector>

namespace Nexus
{
    /// @brief A class representing an input state of  a window
    class InputState
    {
    public:
        /// @brief A method to cache the previous frame's input
        void CachePreviousInput();

        /// @brief A method to connect a controller to the input state
        /// @param index The index of the controller to connect
        void ConnectController(uint32_t index);

        /// @brief A method to remove a controller from the input state
        /// @param index The index of the controller to disconnect
        void RemoveController(uint32_t index);

        /// @brief A method to retrieve a gamepad by the index
        /// @param index The index of the controller to retrieve
        /// @return A pointer to a gamepad contained at the specified index
        Gamepad *GetGamepadByIndex(uint32_t index);

        /// @brief A method that returns the keyboard of the input state
        /// @return A const reference to the keyboard
        const Keyboard &GetKeyboard() { return m_Keyboard; }

        /// @brief A method that returns the mouse of the input state
        /// @return A const reference to the mouse
        const Mouse &GetMouse() { return m_Mouse; }

        /// @brief A method that returns a const reference to a vector of gamepad pointers
        /// @return A const reference to the input state's vector of gamepads
        const std::vector<Gamepad *> &GetGamepads() { return m_Gamepads; }

    private:
        /// @brief The keyboard associated with the input state
        Keyboard m_Keyboard;

        /// @brief The mouse associated with the input state
        Mouse m_Mouse;

        /// @brief The gamepads associated with the input state
        std::vector<Gamepad *> m_Gamepads;

        /// @brief A friend class to allow the window class to access the input state's private properties
        friend class Window;
    };
}