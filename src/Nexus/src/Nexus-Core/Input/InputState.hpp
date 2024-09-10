#pragma once

#include "Gamepad.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Logging/Log.hpp"
#include "Nexus-Core/Point.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "SDL.h"

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
		const Keyboard &GetKeyboard() const
		{
			return m_Keyboard;
		}

		/// @brief A method that returns the mouse of the input state
		/// @return A const reference to the mouse
		const Mouse &GetMouse() const
		{
			return m_Mouse;
		}

		Nexus::EventHandler<char *> TextInput;

	  private:
		/// @brief The keyboard associated with the input state
		Keyboard m_Keyboard;

		/// @brief The mouse associated with the input state
		Mouse m_Mouse;

		/// @brief A friend class to allow the window class to access the input
		/// state's private properties
		friend class Window;

		friend class Application;
	};
}	 // namespace Nexus