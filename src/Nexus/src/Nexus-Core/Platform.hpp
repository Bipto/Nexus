#pragma once

#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"
#include "Nexus-Core/Monitor.hpp"
#include "Nexus-Core/Window.hpp"

namespace Nexus::Platform
{
	std::vector<InputNew::Keyboard> GetKeyboards();
	std::vector<InputNew::Mouse>	GetMice();
	std::vector<InputNew::Gamepad>	GetGamepads();
	std::vector<Monitor>			GetMonitors();

	std::optional<InputNew::Keyboard> GetKeyboardById(uint32_t id);
	std::optional<InputNew::Mouse>	  GetMouseById(uint32_t id);
	std::optional<InputNew::Gamepad>  GetGamepadById(uint32_t id);

	void	Initialise();
	void	Shutdown();
	void	Update();
	Window *CreatePlatformWindow(const WindowSpecification				&windowProps,
								 Graphics::GraphicsAPI					 api,
								 const Graphics::SwapchainSpecification &swapchainSpec);

	GlobalMouseState		GetGlobalMouseState();
	std::optional<Window *> GetKeyboardFocus();
	std::optional<Window *> GetMouseFocus();

	inline EventHandler<uint32_t> OnKeyboardAdded;
	inline EventHandler<uint32_t> OnKeyboardRemoved;
	inline EventHandler<uint32_t> OnMouseAdded;
	inline EventHandler<uint32_t> OnMouseRemoved;
	inline EventHandler<uint32_t> OnGamepadAdded;
	inline EventHandler<uint32_t> OnGamepadRemoved;
}	 // namespace Nexus::Platform