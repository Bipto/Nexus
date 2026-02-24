#pragma once

#include <utility>

#include "Platform/Events/Event.hpp"
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

	struct KeyPressedEventArgs : public Event
	{
		Nexus::KeyCode	KeyCode	   = {};
		Nexus::ScanCode ScanCode   = {};
		uint8_t			Repeat	   = {};
		int32_t			Unicode	   = {};
		uint16_t		Mods	   = {};
		uint32_t		KeyboardID = {};
	};

	struct KeyReleasedEventArgs : public Event
	{
		Nexus::KeyCode	KeyCode	   = {};
		Nexus::ScanCode ScanCode   = {};
		int32_t			Unicode	   = {};
		uint32_t		KeyboardID = {};
	};

	struct MouseMovedEventArgs : public Event
	{
		std::pair<float, float> Position	   = {};
		std::pair<float, float> ScreenPosition = {};
		std::pair<float, float> Movement	   = {};
		uint32_t				MouseID		   = {};
		MouseType				Type		   = {};
	};

	struct MouseButtonPressedEventArgs : public Event
	{
		MouseButton				Button		   = {};
		std::pair<float, float> Position	   = {};
		std::pair<float, float> ScreenPosition = {};
		uint32_t				Clicks		   = {};
		uint32_t				MouseID		   = {};
		MouseType				Type		   = {};
	};

	struct MouseButtonReleasedEventArgs : public Event
	{
		MouseButton				Button		   = {};
		std::pair<float, float> Position	   = {};
		std::pair<float, float> ScreenPosition = {};
		uint32_t				MouseID		   = {};
		MouseType				Type		   = {};
	};

	struct MouseScrolledEventArgs : public Event
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

	struct FileDropEventArgs : public Event
	{
		FileDropType			Type		   = {};
		std::pair<float, float> Position	   = {};
		std::pair<float, float> ScreenPosition = {};
		std::string				SourceApp	   = {};
		std::string				Data		   = {};
	};

	struct WindowResizedEventArgs : public Event
	{
		std::pair<uint32_t, uint32_t> Size = {};
	};

	struct WindowMovedEventArgs : public Event
	{
		std::pair<int32_t, int32_t> Position = {};
	};

	struct TextInputEventArgs : public Event
	{
		const char *Text = {};
	};

	struct TextEditEventArgs : public Event
	{
		const char *Text   = {};
		int32_t		Start  = {};
		int32_t		Length = {};
	};

	struct FingerDownEventArgs : public Event
	{
		std::pair<float, float> Position	  = {};
		std::pair<float, float> Movement	  = {};
		float					Pressure	  = {};
		uint64_t				TouchDeviceID = {};
		uint64_t				FingerID	  = {};
	};

	struct FingerUpEventArgs : public Event
	{
		std::pair<float, float> Position	  = {};
		std::pair<float, float> Movement	  = {};
		float					Pressure	  = {};
		uint64_t				TouchDeviceID = {};
		uint64_t				FingerID	  = {};
	};

	struct FingerMotionEventArgs : public Event
	{
		std::pair<float, float> Position	  = {};
		std::pair<float, float> Movement	  = {};
		float					Pressure	  = {};
		uint64_t				TouchDeviceID = {};
		uint64_t				FingerID	  = {};
	};

	struct FingerUpCancelledArgs : public Event
	{
		std::pair<float, float> Position	  = {};
		std::pair<float, float> Movement	  = {};
		float					Pressure	  = {};
		uint64_t				TouchDeviceID = {};
		uint64_t				FingerID	  = {};
	};
}	 // namespace Nexus