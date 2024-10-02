#pragma once

#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"

#include "Nexus-Core/Events/Event.hpp"
#include "Nexus-Core/Point.hpp"

namespace Nexus
{
	enum class MouseType
	{
		Mouse,
		Touch
	};

	enum class ScrollDirection
	{
		Normal,
		Flipped
	};

	struct KeyPressedEvent
	{
		Nexus::KeyCode		KeyCode	   = {};
		Nexus::ScanCode		ScanCode   = {};
		uint8_t				Repeat	   = {};
		int32_t				Unicode	   = {};
		Keyboard::Modifiers Mods	   = {};
		uint32_t			KeyboardID = {};
	};

	struct KeyReleasedEvent
	{
		Nexus::KeyCode	KeyCode	   = {};
		Nexus::ScanCode ScanCode   = {};
		int32_t			Unicode	   = {};
		uint32_t		KeyboardID = {};
	};

	struct MouseMovedEvent
	{
		Nexus::Point2D<float> Position = {};
		Nexus::Point2D<float> Movement = {};
		uint32_t			  MouseID  = {};
		MouseType			  Type	   = {};
	};

	struct MouseButtonPressedEvent
	{
		MouseButton			  Button	  = {};
		Nexus::Point2D<float> Position	  = {};
		uint32_t			  Clicks	  = {};
		uint32_t			  MouseID	  = {};
		MouseType			  Type		  = {};
	};

	struct MouseButtonReleasedEvent
	{
		MouseButton			  Button	  = {};
		Nexus::Point2D<float> Position	  = {};
		uint32_t			  MouseID	  = {};
		MouseType			  Type		  = {};
	};

	struct MouseScrolledEvent
	{
		Nexus::Point2D<float> Scroll	= {};
		Nexus::Point2D<float> Position	= {};
		uint32_t			  MouseID	= {};
		MouseType			  Type		= {};
		ScrollDirection		  Direction = {};
	};

	enum class FileDropType
	{
		Begin,
		Complete,
		File,
		Text,
		Position
	};

	struct FileDropEvent
	{
		FileDropType   Type		 = {};
		Point2D<float> Position	 = {};
		std::string	   SourceApp = {};
		std::string	   Data		 = {};
	};

	using InputEvent =
	std::variant<KeyPressedEvent, KeyReleasedEvent, MouseMovedEvent, MouseButtonPressedEvent, MouseButtonReleasedEvent, MouseScrolledEvent>;

}	 // namespace Nexus