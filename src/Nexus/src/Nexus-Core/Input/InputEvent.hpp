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
		Nexus::KeyCode		KeyCode;
		Nexus::ScanCode		ScanCode;
		uint8_t				Repeat;
		int32_t				Unicode;
		Keyboard::Modifiers Mods;
		uint32_t			KeyboardID;
	};

	struct KeyReleasedEvent
	{
		Nexus::KeyCode Key;
	};

	struct MouseMovedEvent
	{
		Nexus::Point2D<float> Position;
		Nexus::Point2D<float> Movement;
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

	using InputEvent =
	std::variant<KeyPressedEvent, KeyReleasedEvent, MouseMovedEvent, MouseButtonPressedEvent, MouseButtonReleasedEvent, MouseScrolledEvent>;

}	 // namespace Nexus