#pragma once

#include "Nexus-Core/Events/EventHandler.hpp"

#include "Nexus-Core/Input/InputEvent.hpp"
#include "Nexus-Core/Window.hpp"

namespace Nexus::Platform
{
	EventHandler<KeyPressedEvent, Nexus::Window *>	OnKeyPressed;
	EventHandler<KeyReleasedEvent, Nexus::Window *> OnKeyReleased;
}	 // namespace Nexus::Platform