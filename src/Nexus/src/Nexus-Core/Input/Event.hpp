#pragma once

#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"

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

	struct KeyPressedEventArgs
	{
		Nexus::KeyCode	KeyCode	   = {};
		Nexus::ScanCode ScanCode   = {};
		uint8_t			Repeat	   = {};
		int32_t			Unicode	   = {};
		uint16_t		Mods	   = {};
		uint32_t		KeyboardID = {};
	};

	struct KeyReleasedEventArgs
	{
		Nexus::KeyCode	KeyCode	   = {};
		Nexus::ScanCode ScanCode   = {};
		int32_t			Unicode	   = {};
		uint32_t		KeyboardID = {};
	};

	struct MouseMovedEventArgs
	{
		Nexus::Point2D<float> Position = {};
		Nexus::Point2D<float> Movement = {};
		uint32_t			  MouseID  = {};
		MouseType			  Type	   = {};
	};

	struct MouseButtonPressedEventArgs
	{
		MouseButton			  Button   = {};
		Nexus::Point2D<float> Position = {};
		uint32_t			  Clicks   = {};
		uint32_t			  MouseID  = {};
		MouseType			  Type	   = {};
	};

	struct MouseButtonReleasedEventArgs
	{
		MouseButton			  Button   = {};
		Nexus::Point2D<float> Position = {};
		uint32_t			  MouseID  = {};
		MouseType			  Type	   = {};
	};

	struct MouseScrolledEventArgs
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

	struct FileDropEventArgs
	{
		FileDropType   Type		 = {};
		Point2D<float> Position	 = {};
		std::string	   SourceApp = {};
		std::string	   Data		 = {};
	};

	struct WindowResizedEventArgs
	{
		Nexus::Point2D<uint32_t> Size = {};
	};

	struct WindowMovedEventArgs
	{
		Nexus::Point2D<int32_t> Position = {};
	};

	struct TextInputEventArgs
	{
		const char *Text = {};
	};

	struct TextEditEventArgs
	{
		const char *Text   = {};
		int32_t		Start  = {};
		int32_t		Length = {};
	};
}	 // namespace Nexus