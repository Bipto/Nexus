#pragma once

#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"

#include "Nexus-Core/Events/Event.hpp"
#include "Nexus-Core/Point.hpp"

namespace Nexus
{
	struct KeyPressedEvent
	{
		Nexus::KeyCode Key;
	};

	struct KeyReleasedEvent
	{
		Nexus::KeyCode Key;
	};

	struct MouseMovedEvent
	{
		Nexus::Point2D<int> Position;
		Nexus::Point2D<int> Movement;
	};

	struct MouseButtonPressedEvent
	{
		uint8_t MouseButton;
	};

	struct MouseButtonReleasedEvent
	{
		uint8_t MouseButton;
	};

	struct MouseScrolledEvent
	{
		float ScrollX;
		float ScrollY;
	};

	struct InputEvent
	{
		std::variant<KeyPressedEvent, KeyReleasedEvent, MouseMovedEvent, MouseButtonPressedEvent, MouseButtonReleasedEvent, MouseScrolledEvent> Event;
		bool Handled = false;
	};

}	 // namespace Nexus