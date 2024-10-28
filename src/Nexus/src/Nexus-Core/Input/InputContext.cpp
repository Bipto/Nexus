#include "InputContext.hpp"

#include "Nexus-Core/Platform.hpp"

#include "Nexus-Core/Window.hpp"

namespace Nexus::InputNew
{
	InputContext::InputContext(Nexus::Window *window) : m_Window(window)
	{
		const std::vector<InputNew::Keyboard> &keyboards = Platform::GetKeyboards();
		const std::vector<InputNew::Mouse>	  &mice		 = Platform::GetMice();
		const std::vector<InputNew::Gamepad>  &gamepads	 = Platform::GetGamepads();

		for (const auto &keyboard : keyboards) { m_KeyboardStates[keyboard.GetId()] = {}; }
		for (const auto &mouse : mice) { m_MouseStates[mouse.GetId()] = {}; };

		Platform::OnKeyboardAdded.Bind([&](uint32_t id) { m_KeyboardStates[id] = {}; }, EventPermissions::Locked);
		Platform::OnKeyboardRemoved.Bind([&](uint32_t id) { m_KeyboardStates.erase(id); }, EventPermissions::Locked);
		Platform::OnMouseAdded.Bind([&](uint32_t id) { m_MouseStates[id] = {}; }, EventPermissions::Locked);
		Platform::OnMouseRemoved.Bind([&](uint32_t id) { m_MouseStates[id] = {}; }, EventPermissions::Locked);
	}
}	 // namespace Nexus::InputNew