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
}	 // namespace Nexus