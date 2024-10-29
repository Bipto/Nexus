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

		m_OnKeyboardAdded	= Platform::OnKeyboardAdded.Bind([this](uint32_t id) { m_KeyboardStates[id] = {}; }, EventPermissions::Locked);
		m_OnKeyboardRemoved = Platform::OnKeyboardRemoved.Bind([this](uint32_t id) { m_KeyboardStates.erase(id); }, EventPermissions::Locked);
		m_OnMouseAdded		= Platform::OnMouseAdded.Bind([this](uint32_t id) { m_MouseStates[id] = {}; }, EventPermissions::Locked);
		m_OnMouseRemoved	= Platform::OnMouseRemoved.Bind([this](uint32_t id) { m_MouseStates.erase(id); }, EventPermissions::Locked);

		window->OnKeyPressed.Bind(
		[this](const KeyPressedEventArgs &args)
		{
			ButtonState state = args.Repeat > 0 ? ButtonState::Held : ButtonState::Pressed;
			UpdateKeyboardState(args.KeyboardID, args.ScanCode, state);
		},
		EventPermissions::Locked);

		window->OnKeyReleased.Bind([this](const KeyReleasedEventArgs &args)
								   { UpdateKeyboardState(args.KeyboardID, args.ScanCode, ButtonState::Released); },
								   EventPermissions::Locked);
	}

	InputContext::~InputContext()
	{
		Platform::OnKeyboardAdded.Unbind(m_OnKeyboardAdded);
		Platform::OnKeyboardRemoved.Unbind(m_OnKeyboardRemoved);
		Platform::OnMouseAdded.Unbind(m_OnMouseAdded);
		Platform::OnMouseRemoved.Unbind(m_OnMouseRemoved);
	}

	void InputContext::UpdateKeyboardState(uint32_t id, ScanCode scancode, ButtonState state)
	{
		m_KeyboardStates[id].Keys[scancode] = state;
	}
}	 // namespace Nexus::InputNew