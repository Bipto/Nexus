#include "InputContext.hpp"

#include "Nexus-Core/Platform.hpp"

namespace Nexus::InputNew
{
	const std::vector<InputNew::Keyboard> &InputContext::GetKeyboards()
	{
		return m_Keyboards;
	}

	const std::vector<InputNew::Mouse> &InputContext::GetMice()
	{
		return m_Mice;
	}

	const std::vector<InputNew::Gamepad> &InputContext::GetGamepads()
	{
		return m_Gamepads;
	}

	void InputContext::Initialise()
	{
		m_Keyboards = Platform::GetKeyboards();
		m_Mice		= Platform::GetMice();
		m_Gamepads	= Platform::GetGamepads();
	}

	void InputContext::AddKeyboard(const InputNew::Keyboard &keyboard)
	{
		m_Keyboards.push_back(keyboard);
	}

	void InputContext::AddMouse(const InputNew::Mouse &mouse)
	{
		m_Mice.push_back(mouse);
	}

	void InputContext::AddGamepad(const InputNew::Gamepad &gamepad)
	{
		m_Gamepads.push_back(gamepad);
	}

	void InputContext::AddKeyboards(const std::vector<InputNew::Keyboard> &keyboards)
	{
		m_Keyboards.insert(m_Keyboards.end(), keyboards.begin(), keyboards.end());
	}

	void InputContext::RemoveKeyboard(uint32_t id)
	{
		int index = -1;

		for (size_t i = 0; i < m_Keyboards.size(); i++)
		{
			if (m_Keyboards[i].GetId() == id)
			{
				index = i;
				break;
			}
		}

		if (index != -1)
		{
			m_Keyboards.erase(m_Keyboards.begin() + index);
		}
	}

	void InputContext::RemoveMouse(uint32_t id)
	{
		int index = -1;

		for (size_t i = 0; i < m_Mice.size(); i++)
		{
			if (m_Mice[i].GetId() == id)
			{
				index = i;
				break;
			}
		}

		if (index != -1)
		{
			m_Mice.erase(m_Mice.begin() + index);
		}
	}

	void InputContext::RemoveGamepad(uint32_t id)
	{
		int index = -1;

		for (size_t i = 0; i < m_Gamepads.size(); i++)
		{
			if (m_Gamepads[i].GetId() == id)
			{
				index = i;
				break;
			}
		}

		if (index != -1)
		{
			m_Gamepads.erase(m_Gamepads.begin() + index);
		}
	}
}	 // namespace Nexus::InputNew