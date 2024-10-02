#pragma once

#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/InputEvent.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"
#include "Nexus-Core/Monitor.hpp"

namespace Nexus::Platform
{
	std::vector<InputNew::Keyboard> GetKeyboards();
	std::vector<InputNew::Mouse>	GetMice();
	std::vector<InputNew::Gamepad>	GetGamepads();
	std::vector<Monitor>			GetMonitors();
}	 // namespace Nexus::Platform