#pragma once

#include "Nexus/Point.hpp"
#include "Nexus/Logging/Log.hpp"

#include "SDL.h"
#include "SDL_gamecontroller.h"

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Gamepad.hpp"

#include "Nexus/Events/EventHandler.hpp"

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
        void CacheInput();

        /// @brief A method that returns the keyboard of the input state
        /// @return A const reference to the keyboard
        const Keyboard &GetKeyboard() { return m_Keyboard; }

        /// @brief A method that returns the mouse of the input state
        /// @return A const reference to the mouse
        const Mouse &GetMouse() { return m_Mouse; }

        Nexus::EventHandler<char> TextInput;

    private:
        /// @brief The keyboard associated with the input state
        Keyboard m_Keyboard;

        /// @brief The mouse associated with the input state
        Mouse m_Mouse;

        /// @brief A friend class to allow the window class to access the input state's private properties
        friend class Window;

        friend class Application;
    };
}