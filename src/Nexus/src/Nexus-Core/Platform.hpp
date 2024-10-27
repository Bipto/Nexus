#pragma once

#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/InputEvent.hpp"
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

	void Initialise();
	void Shutdown();
	void	Update();
	Window *CreatePlatformWindow(const WindowSpecification				&windowProps,
								 Graphics::GraphicsAPI					 api,
								 const Graphics::SwapchainSpecification &swapchainSpec);
}	 // namespace Nexus::Platform