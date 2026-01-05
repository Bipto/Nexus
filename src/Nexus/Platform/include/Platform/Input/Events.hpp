#pragma once

#include <utility>

#include "Platform/Input/Gamepad.hpp"
#include "Platform/Input/Keyboard.hpp"
#include "Platform/Input/Mouse.hpp"

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
		std::pair<float, float> Position	   = {};
		std::pair<float, float> ScreenPosition = {};
		std::pair<float, float> Movement	   = {};
		uint32_t				MouseID		   = {};
		MouseType				Type		   = {};
	};

	struct MouseButtonPressedEventArgs
	{
		MouseButton				Button		   = {};
		std::pair<float, float> Position	   = {};
		std::pair<float, float> ScreenPosition = {};
		uint32_t				Clicks		   = {};
		uint32_t				MouseID		   = {};
		MouseType				Type		   = {};
	};

	struct MouseButtonReleasedEventArgs
	{
		MouseButton				Button		   = {};
		std::pair<float, float> Position	   = {};
		std::pair<float, float> ScreenPosition = {};
		uint32_t				MouseID		   = {};
		MouseType				Type		   = {};
	};

	struct MouseScrolledEventArgs
	{
		std::pair<float, float> Scroll		   = {};
		std::pair<float, float> Position	   = {};
		std::pair<float, float> ScreenPosition = {};
		uint32_t				MouseID		   = {};
		MouseType				Type		   = {};
		ScrollDirection			Direction	   = {};
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
		FileDropType			Type		   = {};
		std::pair<float, float> Position	   = {};
		std::pair<float, float> ScreenPosition = {};
		std::string				SourceApp	   = {};
		std::string				Data		   = {};
	};

	struct WindowResizedEventArgs
	{
		std::pair<uint32_t, uint32_t> Size = {};
	};

	struct WindowMovedEventArgs
	{
		std::pair<int32_t, int32_t> Position = {};
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

	struct FingerDownEventArgs
	{
		std::pair<float, float> Position	  = {};
		std::pair<float, float> Movement	  = {};
		float					Pressure	  = {};
		uint64_t				TouchDeviceID = {};
		uint64_t				FingerID	  = {};
	};

	struct FingerUpEventArgs
	{
		std::pair<float, float> Position	  = {};
		std::pair<float, float> Movement	  = {};
		float					Pressure	  = {};
		uint64_t				TouchDeviceID = {};
		uint64_t				FingerID	  = {};
	};

	struct FingerMotionEventArgs
	{
		std::pair<float, float> Position	  = {};
		std::pair<float, float> Movement	  = {};
		float					Pressure	  = {};
		uint64_t				TouchDeviceID = {};
		uint64_t				FingerID	  = {};
	};

	struct FingerUpCancelledArgs
	{
		std::pair<float, float> Position	  = {};
		std::pair<float, float> Movement	  = {};
		float					Pressure	  = {};
		uint64_t				TouchDeviceID = {};
		uint64_t				FingerID	  = {};
	};
}	 // namespace Nexus