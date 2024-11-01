#pragma once

#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	class Window;
}

namespace Nexus::InputNew
{
	struct MouseInfo
	{
		Nexus::Point2D<float>					Position = {};
		std::map<MouseButton, MouseButtonState> Buttons	 = {};
	};

	class Keyboard
	{
	  public:
		Keyboard() = delete;
		Keyboard(uint32_t id, const std::string &name) : m_Id(id), m_Name(name)
		{
		}

		uint32_t GetId() const
		{
			return m_Id;
		}

		const std::string &GetName() const
		{
			return m_Name;
		}

	  private:
		uint32_t	m_Id;
		std::string m_Name;
	};

	class Mouse
	{
	  public:
		Mouse() = delete;
		Mouse(uint32_t id, const std::string &name) : m_Id(id), m_Name(name)
		{
		}

		uint32_t GetId() const
		{
			return m_Id;
		}

		const std::string &GetName() const
		{
			return m_Name;
		}

		EventHandler<uint8_t> OnButtonPressed;
		EventHandler<uint8_t> OnButtonReleased;

	  private:
		uint32_t	m_Id;
		std::string m_Name;
	};

	class Gamepad
	{
	  public:
		Gamepad() = delete;
		Gamepad(uint32_t id, const std::string &name) : m_Id(id), m_Name(name)
		{
		}

		uint32_t GetId() const
		{
			return m_Id;
		}

		const std::string &GetName() const
		{
			return m_Name;
		}

	  private:
		uint32_t	m_Id;
		std::string m_Name;
	};

	struct KeyboardState
	{
		std::map<ScanCode, bool> Keys = {};
	};

	struct MouseState
	{
		Point2D<float>				Position = {};
		Point2D<float>				Movement	   = {};
		std::map<MouseButton, bool> Buttons	 = {};
		Point2D<float>				Scroll	 = {};
		Point2D<float>				ScrollMovement = {};
	};

	class InputContext
	{
	  public:
		InputContext(Nexus::Window *window);
		virtual ~InputContext();

		bool		   IsMouseButtonDown(uint32_t id, MouseButton button);
		bool		   IsMouseButtonUp(uint32_t id, MouseButton button);
		bool		   IsKeyDown(uint32_t id, ScanCode scancode);
		bool		   IsKeyUp(uint32_t id, ScanCode scancode);
		Point2D<float> GetMousePosition(uint32_t id);
		Point2D<float> GetScroll(uint32_t id);

		Point2D<float> GetCursorPosition();
		Point2D<float> GetGlobalCursorPosition();

		void Reset();

	  private:
		void OnKeyPressed(const KeyPressedEventArgs &args);
		void OnKeyReleased(const KeyReleasedEventArgs &args);
		void OnMouseButtonPressed(const MouseButtonPressedEventArgs &args);
		void OnMouseButtonReleased(const MouseButtonReleasedEventArgs &args);
		void OnMouseMoved(const MouseMovedEventArgs &args);
		void OnScroll(const MouseScrolledEventArgs &args);

	  private:
		Nexus::Window *m_Window = nullptr;

		std::map<uint32_t, KeyboardState> m_KeyboardStates;
		std::map<uint32_t, MouseState>	  m_MouseStates;

		Point2D<float> m_CursorPosition = {};

		ScopedEvent<const KeyPressedEventArgs &>		  m_OnKeyPressed;
		ScopedEvent<const KeyReleasedEventArgs &>		  m_OnKeyReleased;
		ScopedEvent<const MouseButtonPressedEventArgs &>  m_OnMouseButtonPressed;
		ScopedEvent<const MouseButtonReleasedEventArgs &> m_OnMouseButtonReleased;
		ScopedEvent<const MouseMovedEventArgs &>		  m_OnMouseMoved;
		ScopedEvent<const MouseScrolledEventArgs &>		  m_OnMouseScrolled;
	};
}	 // namespace Nexus::InputNew