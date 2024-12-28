#pragma once

#include "SDL.h"
#include "SDL_storage.h"
#include "SDL_surface.h"

#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"
#include "Nexus-Core/Monitor.hpp"

namespace Nexus::SDL3
{
	KeyCode				GetNexusKeyCodeFromSDLKeyCode(SDL_Keycode keycode);
	ScanCode			GetNexusScanCodeFromSDLScanCode(SDL_Scancode scancode);
	Keyboard::Modifiers GetNexusModifiersFromSDLModifiers(Uint16 modifiers);

	FileDropType					GetFileDropType(SDL_EventType type);
	std::tuple<MouseType, uint32_t> GetMouseInfo(SDL_MouseID mouseId);
	ScrollDirection					GetScrollDirection(SDL_MouseWheelDirection scrollDirection);

	std::optional<MouseButton> GetMouseButton(Uint8 mouseButton);
}	 // namespace Nexus::SDL3