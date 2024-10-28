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
	enum class ButtonState
	{
		Released = 0,
		Pressed,
		Held
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
		std::map<ScanCode, ButtonState> Keys;
	};

	struct MouseState
	{
		Point2D<int32_t> Position = {};
		Point2D<int32_t> Movement = {};
		MouseButton		 Left	  = {};
		MouseButton		 Right	  = {};
		MouseButton		 Middle	  = {};
		MouseButton		 X1		  = {};
		MouseButton		 X2		  = {};
		Point2D<float>	 Scroll	  = {};
	};

	class InputContext
	{
	  public:
		InputContext(Nexus::Window *window);

	  private:
		Nexus::Window *m_Window = nullptr;

		std::map<uint32_t, KeyboardState> m_KeyboardStates;
		std::map<uint32_t, MouseState>	  m_MouseStates;
	};
}	 // namespace Nexus::InputNew