#pragma once

#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	class Window;
}

namespace Nexus::InputNew
{
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

	class InputContext
	{
	  public:
		InputContext() = default;

	  private:
		void AddKeyboard(const InputNew::Keyboard &keyboard);
		void AddMouse(const InputNew::Mouse &mouse);
		void AddGamepad(const InputNew::Gamepad &gamepad);

		void AddKeyboards(const std::vector<InputNew::Keyboard> &keyboards);

		void RemoveKeyboard(uint32_t id);
		void RemoveMouse(uint32_t id);
		void RemoveGamepad(uint32_t id);

	  private:
		std::vector<InputNew::Keyboard> m_Keyboards = {};
		std::vector<InputNew::Mouse>	m_Mice		= {};
		std::vector<InputNew::Gamepad>	m_Gamepads	= {};

		friend class Nexus::Window;
	};
}	 // namespace Nexus::InputNew